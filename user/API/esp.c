#include "esp.h"
#include "delay.h"
#include "string.h"
#include "stdlib.h"
#include "rtc.h"
#include "uart1.h"
#include "stdio.h"
#include "main.h"
#include "w25q64.h"
#include "mqtt.h"
ESP esp;
WIFI Wifi;
uint8_t weather_code=0;
uint8_t weather_high=0;
uint8_t weather_low=0;
uint8_t weather_nb_code=0;
uint8_t weather_nb_high=0;
uint8_t weather_nb_low=0;
/*
PB10   UART3_TX     ESP_RX     复用推挽输出
PB11   UART3_RX     ESP_TX     浮空输入
PE6                 ESP_EN     通用推挽输出
*/
void ESP_Config(void)
{
	//1.开时钟     
/*中文固件库翻译手册  RCC_APB2PeriphClockCmd  15.2.22
	stm32f10x_rcc.h  693行*/	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOE,ENABLE);  //中文固件库翻译手册  Table 373
		
	//2.定义结构体,目的是给XXX_Init函数准备参数
//void GPIO_Init(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef* GPIO_InitStruct)	
/*中文固件库翻译手册  GPIO_Init  10.2.3
	stm32f10x_gpio.h  351行*/
	GPIO_InitTypeDef GPIO_InitStruct={0};   //变量名无所谓
	
	//3.给结构体赋值
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出   中文固件库翻译手册  Table 185
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10; //待设置的引脚 中文固件库翻译手册  Table 183
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;  //GPIO口的速率(输入可以不配置)  中文固件库翻译手册  Table 184 
	
	//4.调用XXX_Init函数，将参数写入到寄存器中
	GPIO_Init(GPIOB,&GPIO_InitStruct);	
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING; //浮空输入   中文固件库翻译手册  Table 185	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11; //待设置的引脚 中文固件库翻译手册  Table 183	
	GPIO_Init(GPIOB,&GPIO_InitStruct);	
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP; //通用推挽输出   中文固件库翻译手册  Table 185	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6; //待设置的引脚 中文固件库翻译手册  Table 183	
	GPIO_Init(GPIOE,&GPIO_InitStruct);		
	
	//5.开启串口3的时钟
//void RCC_APB2PeriphClockCmd(uint32_t RCC_APB2Periph, FunctionalState NewState); stm32f10x_rcc.h 693行
//void RCC_APB1PeriphClockCmd(uint32_t RCC_APB1Periph, FunctionalState NewState);	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
	
	//6.定义串口的结构体
//void USART_Init(USART_TypeDef* USARTx, USART_InitTypeDef* USART_InitStruct);	stm32f10x_rcc.h 366行
	USART_InitTypeDef USART_InitStruct={0};
	
	//7.给结构体赋值
	USART_InitStruct.USART_BaudRate = 115200;//波特率  和通信的对方ESP8266保持一致
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //硬件流控制(无)
	USART_InitStruct.USART_Mode = USART_Mode_Rx|USART_Mode_Tx; //发送和接收模式
	USART_InitStruct.USART_Parity = USART_Parity_No; //校验方式
	USART_InitStruct.USART_StopBits = USART_StopBits_1; //停止位
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;//数据位
	
	//8.调用XXX_Init函数，将参数写入到寄存器中
	USART_Init(USART3,&USART_InitStruct);
	
	//9.使能空闲中断
//void USART_ITConfig(USART_TypeDef* USARTx, uint16_t USART_IT, FunctionalState NewState);	  stm32f10x_rcc.h 371行
	USART_ITConfig(USART3,USART_IT_IDLE,ENABLE);
	
	//10.使能接收中断
	USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);	
	
	//11.调用XXX_Cmd函数使能外设
//void USART_Cmd(USART_TypeDef* USARTx, FunctionalState NewState);	stm32f10x_rcc.h 370行
	USART_Cmd(USART3,ENABLE);
	
	//12.定义NVIC相关的结构体
//void NVIC_Init(NVIC_InitTypeDef* NVIC_InitStruct);	misc.h 197行
	NVIC_InitTypeDef NVIC_InitStruct={0};
	
	//13.给结构体赋值
	NVIC_InitStruct.NVIC_IRQChannel = USART3_IRQn;//中断通道
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;//通道是否使能
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;//抢占优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority =0;//子(次级)优先级
	
	//14.调用XXX_Init函数将参数写入到寄存器中
	NVIC_Init(&NVIC_InitStruct);	
	
	//15.把ESP8266的EN引脚拉低再拉高，使ESP8266重启
	GPIO_ResetBits(GPIOE,GPIO_Pin_6);
//	Delay_ms(500);  //稳定关机的电平信号
	GPIO_SetBits(GPIOE,GPIO_Pin_6);	
//	Delay_ms(2000);  //过滤一下开机的乱码	
}
void USART3_IRQHandler(void) 
{ 
 uint8_t Data; 
//ITStatus USART_GetITStatus(USART_TypeDef* USARTx, uint16_t USART_IT);  stm32f10x_usart1.h 392行 
//void USART_ClearITPendingBit(USART_TypeDef* USARTx, uint16_t USART_IT); 
 //1.判断接收中断是否触发  SR寄存器的位5是否为1 
 if(USART_GetITStatus(USART3,USART_IT_RXNE)==SET) 
 { 
//  //2.已经接收到数据，读取接收到的数据 
  Data = USART_ReceiveData(USART3); 
	USART_SendData(USART1,Data);
	esp.R_Buff[esp.R_Cnt++]=Data;
//  esp.R_Buff[esp.R_Cnt++]=Data; 
//  //3.清除SR寄存器的位5 根据参考手册 25.6.1的位5描述，清除方法有两种     
//  //(1)对USART_DR的读操作可以将该位清零  (2)RXNE位也可以通过写入0来清除 
//// USART_ClearITPendingBit(UART4,USART_IT_RXNE); 
//  if(esp.R_Cnt>=ESP_R_Buff_Length) 
//  {//避免数组越界 
//   esp.R_Cnt=0; 
//  } 
	 
 } 
 //1.判断空闲中断是否触发  SR寄存器的位4是否为1 
 if(USART_GetITStatus(USART3,USART_IT_IDLE)==SET) 
 { 
  //2.执行中断服务内容 
  esp.R_Idle=1;   //空闲标志位置1 
  //3.清除SR寄存器的位4 根据参考手册 25.6.1的位4描述，清除方法只能通过软件序列清除 
  //软件序列清除：先读USART_SR，然后读USART_DR 
  Data = USART_ReceiveData(USART3);  //只是为了清除标志位 
  Data = Data;    //防止编译器报警告 
 } 
}
void ESP_SendByte(uint8_t Byte) 
{ 
//FlagStatus USART_GetFlagStatus(USART_TypeDef* USARTx, uint16_t USART_FLAG);  stm32f10x_usart.h 390行 
//void USART_ClearFlag(USART_TypeDef* USARTx, uint16_t USART_FLAG);    stm32f10x_usart.h 391行 
 //1.判断上一次是否发送完成   复位第一次为什么满足条件：因为该位复位后默认值是1   
 while(USART_GetFlagStatus(USART3,USART_FLAG_TC)==RESET) 
 {//2.还未发送完成   USART1->SR寄存器的位6是0 
  
 } 
 //3.上一次数据发送完成了，发送新的数据 
//void USART_SendData(USART_TypeDef* USARTx, uint16_t Data);  stm3210x_usart1.h  378行 
 USART_SendData(USART3,Byte); 
 //4.清除标志位  参考手册25.6.1的位6描述 清除标志位两种方法 
 //(1)写0清0  (2)软件序列清除该位(先读USART_SR，然后写入USART_DR) 
// USART_ClearFlag(USART1,USART_FLAG_TC); 
} 
void ESP_SendArray(uint8_t *Buffer,uint16_t len)
{
	for(uint16_t i=0;i<len;i++)
	{
		ESP_SendByte(Buffer[i]);
	}
}
void ESP_SendStr(uint8_t *Str)
{
	while(*Str)
	{
		ESP_SendByte(*Str++);
	}
}
//清除接收缓冲区 
void ESP_Clear_R_Buff(void) 
{ 
 memset(esp.R_Buff,0,ESP_R_Buff_Length); 
 esp.R_Cnt=0; 
} 
/* 
strcmp   判断两个字符串是否完全一致 
strstr   判断母串中是否有子串 
*/ 
/* 
参数1   待发送的字符串 
参数2   收到的回复存放的地址 
参数3   期望的回复 
参数4   超时时间 
参数5   是否检测回复  0  不需要检测回复   1  需要检测回复 
返回值  0 未收到期望的回复   1  收到了期望的回复 
*/ 
uint8_t ESP_SendCmd_CheckRes(uint8_t *cmd,uint8_t *rec_buff,uint8_t *respond,uint16_t time_out,uint8_t flag) 
{ 
 //1.清除接收缓冲区 
 ESP_Clear_R_Buff(); 
 //2.发送指定的指令 
 ESP_SendStr(cmd); 
 //3.判断是否需要检测回复 
 if(flag==0) 
 {//不需要检测回复 
  Delay_ms(time_out); 
  return 1;//返回查找成功 
 } 
 while(strstr((char *)rec_buff,(char *)respond)==NULL) 
 {//未查找到，继续查找 
  Delay_ms(1); 
  time_out--; 
  if(time_out==0) 
  {//等待指定的时间，仍未检测到回复 
   return 0;//返回查找失败 
  } 
 } 
 return 1;//返回查找成功 
} 
uint8_t NET_State=0;   //设备的网络状态  0 设备正在连接服务器  1 设备已经连接服务器建立透传
uint8_t NET_Step=0;   //设备联网的步骤
//设备连接到指定服务器
uint8_t buff[100];
void ESP_Connect_NET(void)
{
	WIFI test;
	switch(NET_Step)
	{
		case 0:
			if(ESP_SendCmd_CheckRes((uint8_t *)"AT\r\n",esp.R_Buff,(uint8_t *)"OK",100,1)==1)
			{
				NET_Step++;
			}
			break;
		case 1:
			if(ESP_SendCmd_CheckRes((uint8_t *)"AT+CWMODE=1\r\n",esp.R_Buff,(uint8_t *)"OK",100,1)==1)
			{
				NET_Step++;
			}			
			break;		
		case 2:
			
			sprintf((char *)buff,"AT+CWJAP=\"%s\",\"%s\"\r\n",Wifi.Name,Wifi.Pwd);			
      if(ESP_SendCmd_CheckRes((uint8_t *)buff,esp.R_Buff,(uint8_t *)"OK",20000,1)==1) 
      { 
        NET_Step++; 
      }    
			else if(ESP_SendCmd_CheckRes((uint8_t *)"AT+CWJAP=\"suniphone\",\"987654321\"\r\n",esp.R_Buff,(uint8_t *)"OK",15000,1)==1)
			{
				NET_Step++;
			}				
			break;
		case 3:
			if(ESP_SendCmd_CheckRes((uint8_t *)"AT+CIPSTART=\"TCP\",\"de95ef99b4.st1.iotda-device.cn-north-4.myhuaweicloud.com\",1883\r\n",esp.R_Buff,(uint8_t *)"OK",1000,1)==1)
			{
				NET_Step++;
			}					
			break;	
		case 4:
			if(ESP_SendCmd_CheckRes((uint8_t *)"AT+CIPMODE=1\r\n",esp.R_Buff,(uint8_t *)"OK",100,1)==1)
			{
				NET_Step++;
			}					
			break;
		case 5:
			if(ESP_SendCmd_CheckRes((uint8_t *)"AT+CIPSEND\r\n",esp.R_Buff,(uint8_t *)">",100,1)==1)
			{
				NET_Step=0;
				NET_State=3;
			}					
			break;		
		case 6:
			break;
		case 7:
			break;		
	}
}
void ESP_Connect_TIME(void)
{
	switch(NET_Step)
	{
		char *str_temp=NULL;		
    uint8_t Buff[50]={0}; 

    case 0: 
    if(ESP_SendCmd_CheckRes((uint8_t *)"AT\r\n",esp.R_Buff,(uint8_t *)"OK",100,1)==1) 
    { 
     NET_Step++; 
    } 
break; 
case 1: 
if(ESP_SendCmd_CheckRes((uint8_t *)"AT+CWMODE=1\r\n",esp.R_Buff,(uint8_t *)"OK",100,1)==1) 
{ 
NET_Step++; 
} 
break;  
  case 2:  //需要修改的参数  
//   if(Send_CMD_Check_Rec((uint8_t *)"AT+CWJAP=\"OnePlus\",\"12345678\"\r\n",(uint8_t *)"OK",Esp.R_Buff,20000,1)==1) 
//   sprintf((char *)Buff,"AT+CWJAP=\"%s\",\"%s\"\r\n","suniphone","987654321"); 
//   if(ESP_SendCmd_CheckRes((uint8_t *)Buff,esp.R_Buff,(uint8_t *)"OK",20000,1)==1) 
//   { 
//    NET_Step++; 
//   }   
sprintf((char *)buff,"AT+CWJAP=\"%s\",\"%s\"\r\n",Wifi.Name,Wifi.Pwd);			
      if(ESP_SendCmd_CheckRes((uint8_t *)buff,esp.R_Buff,(uint8_t *)"OK",20000,1)==1) 
      { 
        NET_Step++; 
      }    
			else if(ESP_SendCmd_CheckRes((uint8_t *)"AT+CWJAP=\"suniphone\",\"987654321\"\r\n",esp.R_Buff,(uint8_t *)"OK",15000,1)==1)
			{
				NET_Step++;
			}		
   break;   
  case 3:  //需要修改的参数   
   if(ESP_SendCmd_CheckRes((uint8_t *)"AT+CIPSTART=\"TCP\",\"acs.m.taobao.com\",80\r\n",esp.R_Buff,(uint8_t 
*)"OK",1000,1)==1) 
   { 
    NET_Step++; 
   }      
   break; 
  case 4: 
   if(ESP_SendCmd_CheckRes((uint8_t *)"AT+CIPMODE=1\r\n",esp.R_Buff,(uint8_t *)"OK",100,1)==1) 
   { 
    NET_Step++; 
   }     
   break;  
  case 5: 
   if(ESP_SendCmd_CheckRes((uint8_t *)"AT+CIPSEND\r\n",esp.R_Buff,(uint8_t *)">",100,1)==1) 
   { 
    NET_Step++; 
   }     
   break;    
  case 6: 
   if(ESP_SendCmd_CheckRes((uint8_t *)"GET https://acs.m.taobao.com/gw/mtop.common.getTimestamp\r\n",esp.R_Buff,(uint8_t *)"}}",1000,1)==1) 
   { 
//{"api":"mtop.common.getTimestamp","v":"*","ret":["SUCCESS::鎺ュ[10:07:32.944]收←◆彛璋冪敤鎴愬姛"],"data":{"t":"1749434851951"}} 
    str_temp=strstr((char *)esp.R_Buff,"data"); 
    if(str_temp!=NULL) 
    { 
     str_temp+=12;  //偏移到时间戳所在的起始地址 
     memcpy(Buff,str_temp,10);  //将整数秒时间戳拷贝到数组中 
     Set_Time_Cnt=atoi((char *)Buff)+8*3600;  //将字符串转换成数字,在东八区,加8个小时 
     Time_Adjust2();  //将时间更新到RTC中 
     NET_Step++; 
    }
	}		
    else 
    { 
      printf("时间解析失败\r\n"); 
    } 
    
   break; 
  case 7: 
   if(ESP_SendCmd_CheckRes((uint8_t *)"+++",esp.R_Buff,NULL,1000,0)==1) 
   { 
    NET_Step=0; 
    NET_State=1; 
   }     
   break; 
  default: 
   break;
	 
	}
}
void ESP_Connect_WEATHER(void)
{
	switch(NET_Step)
	{
		char *str_temp=NULL; 
		char *str_temp1=NULL;
		char *str_temp2=NULL;
    uint8_t Buff[50]={0}; 

    case 0: 
    if(ESP_SendCmd_CheckRes((uint8_t *)"AT\r\n",esp.R_Buff,(uint8_t *)"OK",100,1)==1) 
    { 
     NET_Step++; 
    } 
    break; 
    case 1: 
		if(ESP_SendCmd_CheckRes((uint8_t *)"AT+CWMODE=1\r\n",esp.R_Buff,(uint8_t *)"OK",100,1)==1) 
		{ 
		NET_Step++; 
		} 
		break;  
    case 2:  //需要修改的参数  
//   if(Send_CMD_Check_Rec((uint8_t *)"AT+CWJAP=\"OnePlus\",\"12345678\"\r\n",(uint8_t *)"OK",Esp.R_Buff,20000,1)==1) 
//   sprintf((char *)Buff,"AT+CWJAP=\"%s\",\"%s\"\r\n","suniphone","987654321"); 
//   if(ESP_SendCmd_CheckRes((uint8_t *)Buff,esp.R_Buff,(uint8_t *)"OK",20000,1)==1) 
//   { 
//    NET_Step++; 
//   }    
    sprintf((char *)buff,"AT+CWJAP=\"%s\",\"%s\"\r\n",Wifi.Name,Wifi.Pwd);			
      if(ESP_SendCmd_CheckRes((uint8_t *)buff,esp.R_Buff,(uint8_t *)"OK",20000,1)==1) 
      { 
        NET_Step++; 
      }    
			else if(ESP_SendCmd_CheckRes((uint8_t *)"AT+CWJAP=\"suniphone\",\"987654321\"\r\n",esp.R_Buff,(uint8_t *)"OK",15000,1)==1)
			{
				NET_Step++;
			}	  
   break;   
   case 3:  //需要修改的参数   
   if(ESP_SendCmd_CheckRes((uint8_t *)"AT+CIPSTART=\"TCP\",\"api.seniverse.com\",80\r\n",esp.R_Buff,(uint8_t 
*)"OK",1000,1)==1) 
   { 
    NET_Step++; 
   }      
   break; 
  case 4: 
   if(ESP_SendCmd_CheckRes((uint8_t *)"AT+CIPMODE=1\r\n",esp.R_Buff,(uint8_t *)"OK",100,1)==1) 
   { 
    NET_Step++; 
   }     
   break;  
  case 5: 
   if(ESP_SendCmd_CheckRes((uint8_t *)"AT+CIPSEND\r\n",esp.R_Buff,(uint8_t *)">",100,1)==1) 
   { 
    NET_Step++; 
   }     
   break;    
  case 6: 
//   if(ESP_SendCmd_CheckRes((uint8_t *)"GET https://api.seniverse.com/v3/weather/now.json?key=SKvk3jp4QtyHQkJHp&location=zhengzhou&language=en&unit=c\r\n",esp.R_Buff,(uint8_t *)"code",1000,1)==1) 
	 if(ESP_SendCmd_CheckRes((uint8_t *)"GET https://api.seniverse.com/v3/weather/daily.json?key=SKvk3jp4QtyHQkJHp&location=zhengzhou&language=en&unit=c&start=0&days=1\r\n",esp.R_Buff
		 ,(uint8_t *)"code_day",1000,1)==1) 
   {
    char *end=NULL;		 
    str_temp = strstr((char *)esp.R_Buff, "\"code_day\":\"");
    if(str_temp) 
		{
			str_temp += strlen("\"code_day\":\"");  // 跳过 "code_day":" 部分
			end = strchr(str_temp, '"');  // 找到结尾的引号
			if(end) 
			{
					int len = end - str_temp;
					if(len > sizeof(Buff)-1) len = sizeof(Buff)-1;  // 防止溢出
					memcpy(Buff, str_temp, len);
					Buff[len] = '\0';   // 补结束符
					weather_code = atoi((char*)Buff);  // 转成数字
					printf("Weather code = %d\n", weather_code);
			}
    }
	  }		
    else 
    { 
      printf("天气解析失败\r\n"); 
    }
    str_temp1 = strstr((char *)esp.R_Buff, "\"high\":\"");
    if(str_temp1) 
		{
			str_temp1 += strlen("\"high\":\"");  // 跳过 "high":" 部分
			char *end1 = strchr(str_temp1, '"');  // 找到结尾的引号
			if(end1) 
			{
					int len = end1 - str_temp1;
					if(len > sizeof(Buff)-1) len = sizeof(Buff)-1;  // 防止溢出
					memcpy(Buff, str_temp1, len);
					Buff[len] = '\0';   // 补结束符
					weather_high = atoi((char*)Buff);  // 转成数字
					printf("weather_high = %d\n", weather_high);			
			}
  	}
		 str_temp2 = strstr((char *)esp.R_Buff, "\"low\":\"");
    if(str_temp2) 
		{
			str_temp2 += strlen("\"low\":\"");  // 跳过 "high":" 部分
			char *end1 = strchr(str_temp2, '"');  // 找到结尾的引号
			if(end1) 
			{
					int len = end1 - str_temp2;
					if(len > sizeof(Buff)-1) len = sizeof(Buff)-1;  // 防止溢出
					memcpy(Buff, str_temp2, len);
					Buff[len] = '\0';   // 补结束符
					weather_low = atoi((char*)Buff);  // 转成数字
					printf("weather_low = %d\n", weather_low);	
          NET_Step++;				
			}
  	}	
   break;
	   case 7:
			memset(esp.R_Buff,0,sizeof(esp.R_Buff));
		if(ESP_SendCmd_CheckRes((uint8_t *)"GET https://api.seniverse.com/v3/weather/daily.json?key=SKvk3jp4QtyHQkJHp&location=ningbo&language=en&unit=c&start=0&days=1\r\n",esp.R_Buff,(uint8_t *)"code_day",1000,1)==1) 
   {
    char *end=NULL;		 
    str_temp = strstr((char *)esp.R_Buff, "\"code_day\":\"");
    if(str_temp) 
		{
			str_temp += strlen("\"code_day\":\"");  // 跳过 "code_day":" 部分
			end = strchr(str_temp, '"');  // 找到结尾的引号
			if(end) 
			{
					int len = end - str_temp;
					if(len > sizeof(Buff)-1) len = sizeof(Buff)-1;  // 防止溢出
					memcpy(Buff, str_temp, len);
					Buff[len] = '\0';   // 补结束符
					weather_nb_code = atoi((char*)Buff);  // 转成数字
					printf("weather_nb_code = %d\n", weather_nb_code);
				  NET_Step++;	
			}
    }
	  }		
    else 
    { 
      printf("天气解析失败\r\n"); 
    }
    str_temp1 = strstr((char *)esp.R_Buff, "\"high\":\"");
    if(str_temp1) 
		{
			str_temp1 += strlen("\"high\":\"");  // 跳过 "high":" 部分
			char *end1 = strchr(str_temp1, '"');  // 找到结尾的引号
			if(end1) 
			{
					int len = end1 - str_temp1;
					if(len > sizeof(Buff)-1) len = sizeof(Buff)-1;  // 防止溢出
					memcpy(Buff, str_temp1, len);
					Buff[len] = '\0';   // 补结束符
					weather_nb_high = atoi((char*)Buff);  // 转成数字
					printf("weather_nb_high = %d\n", weather_nb_high);			
			}
  	}
		 str_temp2 = strstr((char *)esp.R_Buff, "\"low\":\"");
    if(str_temp2) 
		{
			str_temp2 += strlen("\"low\":\"");  // 跳过 "high":" 部分
			char *end1 = strchr(str_temp2, '"');  // 找到结尾的引号
			if(end1) 
			{
					int len = end1 - str_temp2;
					if(len > sizeof(Buff)-1) len = sizeof(Buff)-1;  // 防止溢出
					memcpy(Buff, str_temp2, len);
					Buff[len] = '\0';   // 补结束符
					weather_nb_low = atoi((char*)Buff);  // 转成数字
					printf("weather_nb_low = %d\n", weather_nb_low);			
			}
  	}
   uint8_t update_Message=2;
	 xQueueSend(xQueue_Update,&update_Message,0);		
  break;
  case 8: 
   if(ESP_SendCmd_CheckRes((uint8_t *)"+++",esp.R_Buff,NULL,1000,0)==1) 
   { 
    NET_Step=0; 
    NET_State=2; 
   }     
   break; 
  default: 
   break;
	 
	}
}
void ESP_Connect_AP(void) 
{ 
 char * pointer_begin=NULL; 
 char * pointer_end=NULL; 
 char length=0; 
 switch(NET_Step) 
 { 
  case 0: 
   //强制将ESP8266重启  -- 保证退出透传 
   GPIO_ResetBits(GPIOE,GPIO_Pin_6); 
   vTaskDelay(500); 
   GPIO_SetBits(GPIOE,GPIO_Pin_6);  //等完全开机完成 
   vTaskDelay(3000);   
   NET_Step++; 
   break; 
  case 1: 
   if(ESP_SendCmd_CheckRes((uint8_t *)"AT+CWMODE=2\r\n",esp.R_Buff,(uint8_t *)"OK",100,1)==1) 
    NET_Step++;    
   break;  
  case 2: 
   if(ESP_SendCmd_CheckRes((uint8_t *)"AT+CWSAP=\"test_sjs\",\"12345678\",6,4\r\n",esp.R_Buff,(uint8_t 
*)"OK",200,1)==1) 
    NET_Step++;     
   break; 
  case 3:                                                                              
   if(ESP_SendCmd_CheckRes((uint8_t *)"AT+CIPMUX=1\r\n",esp.R_Buff,(uint8_t *)"OK",2000,1)==1) 
    NET_Step++;     
   break;  
  case 4: 
   if(ESP_SendCmd_CheckRes((uint8_t *)"AT+CIPSERVER=1,1001\r\n",esp.R_Buff,(uint8_t *)"OK",2000,1)==1) 
    NET_Step++;    
   break;  
  case 5: //时间需要长一些 
   if(ESP_SendCmd_CheckRes((uint8_t *)"AT+CIFSR\r\n",esp.R_Buff,(uint8_t *)"CONNECT",60000,1)==1) 
   { 
    NET_Step++;  
    ESP_Clear_R_Buff();  //清除接收缓冲区,保证从头接收 
   } 
   break; 
  case 6:		
//+IPD,1,79:{"wifiname":"test","password":"12345678","mycity":"","alarm_h":"","alarm_m":""}  
//+IPD,0,45:{"wifiname":"Xiaomi 14","password":"wang624.k"}	
	if(strstr((char *)esp.R_Buff,"wifiname")!=NULL) 
   { 
		 printf("接收到wifi");
    //数据收完了 查找wifi名称 
    pointer_begin=strstr((char *)esp.R_Buff,"wifiname");  //w所在地址 
    pointer_begin+=11;  //找到wifi名称开始的头 
    pointer_end=pointer_begin; 
    while((*pointer_end)!='"') 
    { 
     pointer_end++; 
    } 
    length=pointer_end-pointer_begin; 
    memset(Wifi.Name,0,sizeof(Wifi.Name)); 
    memcpy(Wifi.Name,pointer_begin,length); 
    //查找wifi密码 
    pointer_begin=strstr((char *)esp.R_Buff,"password");  //p所在地址 
    pointer_begin+=11;  //找到wifi密码开始的头 
    pointer_end=pointer_begin; 
    while((*pointer_end)!='"') 
    { 
     pointer_end++; 
    } 
    length=pointer_end-pointer_begin; 
    memset(Wifi.Pwd,0,sizeof(Wifi.Pwd)); 
    memcpy(Wifi.Pwd,pointer_begin,length);  
    //将信息存储到w25q64中 
    Wifi.Cs=1; 
    W25Q64_SectorErase(WIFI_W25Q64_ADDR); 
    sFLASH_WriteBuffer((uint8_t *)&Wifi,WIFI_W25Q64_ADDR,sizeof(Wifi));   
    NET_Step++;
     printf("%s,%s已存入w25q64\r\n",Wifi.Name,Wifi.Pwd);		
   } 
   break;   
  case 7: 
   if(ESP_SendCmd_CheckRes((uint8_t *)"AT+RST\r\n",esp.R_Buff,NULL,4000,0)==1)  //退出透传 
   {
    printf("退出透传\r\n");		 
    NET_Step=0; 
    NET_State=0;  //回到获取时间状态 
   }     
   break;    
  default: 
   break;    
 }  
} 
void ESP_Connect_HW(void) 
{ 
 char * pointer_begin=NULL; 
 char * pointer_end=NULL; 
 char length=0; 
 switch(NET_Step) 
 { 
  case 0: 
   //强制将ESP8266重启  -- 保证退出透传 
   GPIO_ResetBits(GPIOE,GPIO_Pin_6); 
   vTaskDelay(500); 
   GPIO_SetBits(GPIOE,GPIO_Pin_6);  //等完全开机完成 
   vTaskDelay(3000);   
   NET_Step++; 
   break; 
  case 1: 
   if(ESP_SendCmd_CheckRes((uint8_t *)"AT+CWMODE=2\r\n",esp.R_Buff,(uint8_t *)"OK",100,1)==1) 
    NET_Step++;    
   break;  
  case 2: 
   if(ESP_SendCmd_CheckRes((uint8_t *)"AT+CWSAP=\"test_sjs\",\"12345678\",6,4\r\n",esp.R_Buff,(uint8_t 
*)"OK",200,1)==1) 
    NET_Step++;     
   break; 
  case 3:                                                                              
   if(ESP_SendCmd_CheckRes((uint8_t *)"AT+CIPMUX=1\r\n",esp.R_Buff,(uint8_t *)"OK",2000,1)==1) 
    NET_Step++;     
   break;  
  case 4: 
   if(ESP_SendCmd_CheckRes((uint8_t *)"AT+CIPSERVER=1,1001\r\n",esp.R_Buff,(uint8_t *)"OK",2000,1)==1) 
    NET_Step++;    
   break;  
  case 5: //时间需要长一些 
   if(ESP_SendCmd_CheckRes((uint8_t *)"AT+CIFSR\r\n",esp.R_Buff,(uint8_t *)"CONNECT",60000,1)==1) 
   { 
    NET_Step++;  
    ESP_Clear_R_Buff();  //清除接收缓冲区,保证从头接收 
   } 
   break; 
  case 6:		
//+IPD,1,79:{"wifiname":"test","password":"12345678","mycity":"","alarm_h":"","alarm_m":""}  
//+IPD,0,45:{"wifiname":"Xiaomi 14","password":"wang624.k"}
/*+IPD,0,45:{"ClientID":"68c9037232771f177b60a9d7_Device_sjs_0_0_2025091606",
	"Username":"68c9037232771f177b60a9d7_Device_sjs"
	"Password":"635c5c5980f545b4568fb648934c7a67518c768b5992a56f6bece15b795e90b0"}	
*/
	if(strstr((char *)esp.R_Buff,"}")!=NULL) 
   { 
		 printf("接收到华为云三要素");
    //数据收完了 查找hw.ClientID 
    pointer_begin=strstr((char *)esp.R_Buff,"ClientID");  //w所在地址 
    pointer_begin+=11;  //找到ClientID名称开始的头 
    pointer_end=pointer_begin; 
    while((*pointer_end)!='"') 
    { 
     pointer_end++; 
    } 
    length=pointer_end-pointer_begin; 
    memset(hw.ClientID,0,sizeof(hw.ClientID)); 
    memcpy(hw.ClientID,pointer_begin,length); 
    //查找hw.Username 
    pointer_begin=strstr((char *)esp.R_Buff,"Username");  //p所在地址 
    pointer_begin+=11;  //找到Username密码开始的头 
    pointer_end=pointer_begin; 
    while((*pointer_end)!='"') 
    { 
     pointer_end++; 
    } 
    length=pointer_end-pointer_begin; 
    memset(hw.Username,0,sizeof(hw.Username)); 
    memcpy(hw.Username,pointer_begin,length); 
    //查找hw.Password 
    pointer_begin=strstr((char *)esp.R_Buff,"Password");  //p所在地址 
    pointer_begin+=11;  //找到Password密码开始的头 
    pointer_end=pointer_begin; 
    while((*pointer_end)!='"') 
    { 
     pointer_end++; 
    } 
    length=pointer_end-pointer_begin; 
    memset(hw.Password,0,sizeof(hw.Password)); 
    memcpy(hw.Password,pointer_begin,length); 		
    //将信息存储到w25q64中 
    Wifi.Cs=1; 
    W25Q64_SectorErase(WIFI_huaweiyun_ADDR); 
    sFLASH_WriteBuffer((uint8_t *)&hw,WIFI_huaweiyun_ADDR,sizeof(hw));   
    NET_Step++;
     printf("%s,%s,%s已存入w25q64\r\n",hw.ClientID,hw.Username,hw.Password);		
   } 
   break;   
  case 7: 
   if(ESP_SendCmd_CheckRes((uint8_t *)"AT+RST\r\n",esp.R_Buff,NULL,4000,0)==1)  //退出透传 
   {
    printf("退出透传\r\n");		 
    NET_Step=0; 
    NET_State=0;  //回到获取时间状态 
   }     
   break;    
  default: 
   break;    
 }  
} 