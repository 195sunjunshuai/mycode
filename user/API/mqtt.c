#include "mqtt.h"
#include "esp.h"
#include "mqtt.h"
#include "uart1.h"
#include "delay.h"
#include "dht.h"
#include "string.h"
#include "MQTTPacket.h"
#include "bh1750.h"
#include "adc.h"
#include "kqm.h"
HW hw;
//封装连接报文
void MQTT_Connect(void)
{
	uint16_t time_out=1000;
	MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
	int len = 0;  //打包之后的有效长度
	data.clientID.cstring = "68c9037232771f177b60a9d7_Device_sjs_0_0_2025091606";   //客户端ID
//	data.clientID.cstring = hw.ClientID;   //客户端ID
	data.keepAliveInterval = 60;   //保持连接时间
	data.cleansession = 1;   //是否清理会话
	data.username.cstring = "68c9037232771f177b60a9d7_Device_sjs";  //用户名
//	data.username.cstring = hw.Username;  //用户名
	data.password.cstring = "635c5c5980f545b4568fb648934c7a67518c768b5992a56f6bece15b795e90b0";  //密码
//	data.password.cstring = hw.Password;  //密码
	/*根据参数实现连接报文的打包
	参数1   打包数据存放的缓冲区地址
	参数2   打包数据存放的缓冲区长度，避免数组越界
	参数3   参数相关的结构体
	返回值  >0  打包之后的有效长度    <=0  打包失败，可能缓冲区长度不够
	*/
	len = MQTTSerialize_connect(esp.S_Buff, ESP_S_Buff_Length, &data);
	if(len<=0)
	{
		printf("连接报文打包失败\r\n");
		return;
	}
	ESP_Clear_R_Buff();  //清除一下接收缓冲区，方便重头接收
	UART1_SendArray(esp.S_Buff,len);//往串口1发一份，调试使用  
	ESP_SendArray(esp.S_Buff,len);  //将打包后的数据发送给ESP8266，ESP8266转发给服务器
	while(time_out--)
	{
		Delay_ms(1);
		if(esp.R_Buff[0]==0x20&&esp.R_Buff[1]==0x02&&esp.R_Buff[2]==0x00&&esp.R_Buff[3]==0x00)
		{
			printf("收到正确的连接应答\r\n");
			NET_State=4;
			return;
		}		
	}
}

//封装发布报文
void MQTT_Publish(void)
{
	int len = 0;  //打包之后的有效长度	
	uint8_t payload[300]={0};  //存放具体的有效载荷
	uint16_t payloadlen=0;	 //存放有效载荷的长度
	MQTTString topicString = MQTTString_initializer;
	topicString.cstring = Publish_Topic;
//	VOC=%.1f,CH20=%.2f,CO2=%d\r\n",Sensor.VOC,Sensor.CH2O,Sensor.CO2
	sprintf((char *)payload,
		"{\"services\":[{\"properties\":\
	  {\"temp\":%.1f,\
	  \"hum\":%d,\
	  \"light\":%.1f,\
	  \"smoke\":%.2f,\
	  \"voc\":%.1f,\
	  \"ch20\":%.2f,\
	  \"co2\":%d},\
	  \"service_id\":\"Device_sjs\",\"event_time\":null}]}",Dht.Tem,Dht.Hum,LightInten,smoke_value,
	                            Sensor.VOC,Sensor.CH2O,Sensor.CO2);
//	sprintf((char *)payload,
//		"{\"services\":[{\"properties\":\
//	  {\"temp\":%.1f,\
//		\"hum\":%d},\
//		\"service_id\":\"Device_sjs\",\"event_time\":null}]}",
//		Dht.Tem,Dht.Hum);
  payloadlen=strlen((char *)payload);  //计算字符串长度，不能用sizeof
	/*根据参数实现发布报文的打包
	参数1   打包数据存放的缓冲区地址
	参数2   打包数据存放的缓冲区长度，避免数组越界
	参数3   Dup  Qos==0,该位必须为0
	参数4   Qos  主动选择Qos==0  分为0 1 2  2最可靠
  参数5   retain   保留标志  0   类似于店门口，红外检测到人，播报"老铁，欢迎光临"
  参数6   报文标识符	Qos==0,没有报文标识符
  参数7   主题名
  参数8   有效载荷
  参数9   有效载荷的长度	
	返回值  >0  打包之后的有效长度    <=0  打包失败，可能缓冲区长度不够
	*/	
	len = MQTTSerialize_publish(esp.S_Buff, ESP_S_Buff_Length, 0, 0, 0, 0, topicString, (unsigned char*)payload, payloadlen);	
	if(len<=0)
	{
		printf("发布报文打包失败\r\n");
		return;
	}
	ESP_Clear_R_Buff();  //清除一下接收缓冲区，方便重头接收
	UART1_SendArray(esp.S_Buff,len);//往串口1发一份，调试使用  
	ESP_SendArray(esp.S_Buff,len);  //将打包后的数据发送给ESP8266，ESP8266转发给服务器
}

//订阅相关的主题
void MQTT_Subscribe(void)
{
	uint16_t time_out=1000;
	int len = 0;  //打包之后的有效长度
	int msgid = 1;  //报文标识符
	int req_qos = 0;//服务质量等级
	MQTTString topicString = MQTTString_initializer;
	topicString.cstring = Subscribe_Topic;	
	/*根据参数实现订阅报文的打包
	参数1   打包数据存放的缓冲区地址
	参数2   打包数据存放的缓冲区长度，避免数组越界
	参数3   Dup  Qos==0,该位必须为0
	参数4   报文标识符，平台回复也要有一致的报文标识符
  参数5   订阅主题的数量，该函数支持1次订阅多个主题
  参数6   主题名的结构体
  参数7   主题名的服务等级
	返回值  >0  打包之后的有效长度    <=0  打包失败，可能缓冲区长度不够
	*/		
	len = MQTTSerialize_subscribe(esp.S_Buff, ESP_S_Buff_Length, 0, msgid, 1, &topicString, &req_qos);	
	if(len<=0)
	{
		printf("订阅报文打包失败\r\n");
		return;
	}
	ESP_Clear_R_Buff();  //清除一下接收缓冲区，方便重头接收
	UART1_SendArray(esp.S_Buff,len);//往串口1发一份，调试使用  
	ESP_SendArray(esp.S_Buff,len);  //将打包后的数据发送给ESP8266，ESP8266转发给服务器
	while(time_out--)
	{
		Delay_ms(1);
		if(esp.R_Buff[0]==0x90&&esp.R_Buff[1]==0x03)
		{
			printf("收到正确的订阅应答\r\n");
			esp.R_Idle=0;
			NET_State=5;
			return;
		}		
	}
}

//打包发送心跳
void MQTT_Ping(void)
{
	int len = 0;  //打包之后的有效长度	
	/*根据参数实现订阅报文的打包
	参数1   打包数据存放的缓冲区地址
	参数2   打包数据存放的缓冲区长度，避免数组越界
	返回值  >0  打包之后的有效长度    <=0  打包失败，可能缓冲区长度不够
	*/		
	len = MQTTSerialize_pingreq(esp.S_Buff, ESP_S_Buff_Length);
	if(len<=0)
	{
		printf("心跳报文打包失败\r\n");
		return;
	}
	ESP_Clear_R_Buff();  //清除一下接收缓冲区，方便重头接收
	UART1_SendArray(esp.S_Buff,len);//往串口1发一份，调试使用  
	ESP_SendArray(esp.S_Buff,len);  //将打包后的数据发送给ESP8266，ESP8266转发给服务器
}
