#include "key.h"
#include "delay.h"
#include "led.h"
#include "relay.h"
#include "main.h"
#include "uart1.h"
#include "stdio.h"
#include "string.h"
#include "pwm.h"
#include "w25q64.h"
#include "lvgl_app.h"
#include "gui_guider.h"

//按键初始化
void KEY_Config(void)
{
#if (USE_STD_LIB==0)
	//结合硬件电路，KEY1  KEY2分别接PA0  PC4  PC5  PC6
	//1.开启GPIOA和GPIOC的时钟
//	RCC->APB2ENR |= (0x01<<2);	  //参考手册 6.3.7  位2置1
//	RCC->APB2ENR |= (0x01<<4);	  //参考手册 6.3.7  位4置1	
	RCC->APB2ENR |= (0x05<<2);	  //参考手册 6.3.7  位2,4置1
	
	//2.设置工作模式   因为外部按键电路有稳定的高和低，所以配置浮空输入
	GPIOA->CRL &= ~(0x0F<<0);  //把位3:0清0
	GPIOA->CRL |= (0x04<<0);   //把位3:0设置成0100
	
	GPIOC->CRL &= ~(0x0FFF<<16);  //把位27:16清0
	GPIOC->CRL |= (0x0444<<16);   //把位27:16设置成0100
#elif (USE_STD_LIB==1)	
	//1.开时钟     
/*中文固件库翻译手册  RCC_APB2PeriphClockCmd  15.2.22
	stm32f10x_rcc.h  693行*/	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC,ENABLE);  //中文固件库翻译手册  Table 373
		
	//2.定义结构体,目的是给XXX_Init函数准备参数
//void GPIO_Init(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef* GPIO_InitStruct)	
/*中文固件库翻译手册  GPIO_Init  10.2.3
	stm32f10x_gpio.h  351行*/
	GPIO_InitTypeDef GPIO_InitStruct={0};   //变量名无所谓
	
	//3.给结构体赋值
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING; //浮空输入   中文固件库翻译手册  Table 185
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0; //待设置的引脚 中文固件库翻译手册  Table 183
//	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;  //GPIO口的速率(输入可以不配置)  中文固件库翻译手册  Table 184 
	
	//4.调用XXX_Init函数，将参数写入到寄存器中
	GPIO_Init(GPIOA,&GPIO_InitStruct);	
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6; //待设置的引脚 中文固件库翻译手册  Table 183	
	GPIO_Init(GPIOC,&GPIO_InitStruct);		
#endif
}

//按键扫描函数
/*返回值
0   无按键按下
1   按键1按下，并松手
2   按键2按下，并松手
3   按键3按下，并松手
4   按键4按下，并松手
*/
uint8_t KEY_Scan(void)
{
#if (USE_STD_LIB==0)	
	uint8_t Key_Value=0;  //按键值
	if((GPIOA->IDR & (0x01<<0))!=0x00)	
	{//PA0按下
		Delay_ms(10);   //延时消抖
		if((GPIOA->IDR & (0x01<<0))!=0x00)
		{//再次确认，依然按下
			while((GPIOA->IDR & (0x01<<0))!=0x00)
			{//未松手
			}
			Key_Value=1;
		}				
	}
	if((GPIOC->IDR & (0x01<<4))==0x00)
	{//PC4按下
		Delay_ms(10);   //延时消抖
		if((GPIOC->IDR & (0x01<<4))==0x00)
		{//再次确认，依然按下
			while((GPIOC->IDR & (0x01<<4))==0x00)
			{//未松手
			}
			Key_Value=2;
		}				
	}	
	if((GPIOC->IDR & (0x01<<5))==0x00)
	{//PC4按下
		Delay_ms(10);   //延时消抖
		if((GPIOC->IDR & (0x01<<5))==0x00)
		{//再次确认，依然按下
			while((GPIOC->IDR & (0x01<<5))==0x00)
			{//未松手
			}
			Key_Value=3;
		}				
	}	
	if((GPIOC->IDR & (0x01<<6))==0x00)
	{//PC4按下
		Delay_ms(10);   //延时消抖
		if((GPIOC->IDR & (0x01<<6))==0x00)
		{//再次确认，依然按下
			while((GPIOC->IDR & (0x01<<6))==0x00)
			{//未松手
			}
			Key_Value=4;
		}				
	}	
	return Key_Value;
#elif (USE_STD_LIB==1)
	#if (KEY_SCAN_MODE==0)
/*u8 GPIO_ReadInputDataBit(GPIO_TypeDef* GPIOx, u16 GPIO_Pin)
中文固件库翻译手册  10.2.5
stm32f10x_gpio.h  353行
*/
	uint8_t Key_Value=0;  //按键值
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)==Bit_SET)	
	{//PA0按下
		Delay_ms(10);   //延时消抖
		if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)==Bit_SET)
		{//再次确认，依然按下
			while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)==Bit_SET)
			{//未松手
			}
			Key_Value=1;
		}				
	}
	if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_4)==Bit_RESET)
	{//PC4按下
		Delay_ms(10);   //延时消抖
		if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_4)==Bit_RESET)
		{//再次确认，依然按下
			while(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_4)==Bit_RESET)
			{//未松手
			}
			Key_Value=2;
		}				
	}	
	if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_5)==Bit_RESET)
	{//PC5按下
		Delay_ms(10);   //延时消抖
		if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_5)==Bit_RESET)
		{//再次确认，依然按下
			while(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_5)==Bit_RESET)
			{//未松手
			}
			Key_Value=3;
		}				
	}	
	if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_6)==Bit_RESET)
	{//PC6按下
		Delay_ms(10);   //延时消抖
		if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_6)==Bit_RESET)
		{//再次确认，依然按下
			while(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_6)==Bit_RESET)
			{//未松手
			}
			Key_Value=4;
		}				
	}		
	return Key_Value;
	#elif (KEY_SCAN_MODE==1)
	static uint16_t KEY_Cnt[4]={0};  //用来统计按下的时间  
	uint8_t Key_Value=0;  //按键值	
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)==Bit_SET)
	{//按下的状态
		KEY_Cnt[0]++;  //按键按下时间++，10ms加1次
	}
	else
	{//松手的状态
		if(KEY_Cnt[0]<=1)
		{//一直没按下或者误触发
//			Key_Value=0;  
		}
		else if(KEY_Cnt[0]>1&&KEY_Cnt[0]<=100)
		{//按下的时间<=1s  短按
			Key_Value=1;
		}
		else
		{//按下的时间>1s  长按
			Key_Value=5;
		}	
		KEY_Cnt[0]=0;	
	}
	if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_4)==Bit_RESET)
	{//按下的状态
		KEY_Cnt[1]++;  //按键按下时间++，10ms加1次
	}
	else
	{//松手的状态
		if(KEY_Cnt[1]<=1)
		{//一直没按下或者误触发
//			Key_Value=0;	  		
		}
		else if(KEY_Cnt[1]>1&&KEY_Cnt[1]<=100)
		{//按下的时间<=1s  短按
			Key_Value=2;			
		}
		else
		{//按下的时间>1s  长按
			Key_Value=6;					
		}	
		KEY_Cnt[1]=0;			
	}	
	if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_5)==Bit_RESET)
	{//按下的状态
		KEY_Cnt[2]++;  //按键按下时间++，10ms加1次
	}
	else
	{//松手的状态
		if(KEY_Cnt[2]<=1)
		{//一直没按下或者误触发
//			Key_Value=0;	  		
		}
		else if(KEY_Cnt[2]>1&&KEY_Cnt[2]<=100)
		{//按下的时间<=1s  短按
			Key_Value=3;			
		}
		else
		{//按下的时间>1s  长按
			Key_Value=7;					
		}	
		KEY_Cnt[2]=0;			
	}	
	if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_6)==Bit_RESET)
	{//按下的状态
		KEY_Cnt[3]++;  //按键按下时间++，10ms加1次
	}
	else
	{//松手的状态
		if(KEY_Cnt[3]<=1)
		{//一直没按下或者误触发
//			Key_Value=0;	  		
		}
		else if(KEY_Cnt[3]>1&&KEY_Cnt[3]<=100)
		{//按下的时间<=1s  短按
			Key_Value=4;			
		}
		else
		{//按下的时间>1s  长按
			Key_Value=8;					
		}	
		KEY_Cnt[3]=0;			
	}		
	return Key_Value;
	#endif
#endif
}

//按键1长按  发送  字符"1"
//按键2长按  发送数组  {0x31 0x32 0x33 0x34 0x35 0x36}
//按键3长按  发送字符串  “good good study,day day up”
//按键4长按  通过printf实现发送  “STM32也不难呀”
uint8_t TEST_Buff[7]={0x31,0x32,0x33,0x34,0x35,0x36};
uint8_t READ_Buff[7];

typedef struct
{
  uint8_t data1;
  uint32_t data2;	
	uint8_t Buff[5];
}TEST;

TEST Test_W;
TEST Test_R;


//按键处理函数
void Key_Handle(void)
{
  BaseType_t	xReturned;
	uint8_t Key_Value=0;  //按键值
	Key_Value=KEY_Scan();  //调用按键扫描
	uint8_t AP_Message=0x06; 
	switch(Key_Value)
	{
		case 0:   //无按键按下
			
			break;
		case 1: //按键1短按、并松手 
//    lv_scr_load(guider_ui.screen);  
    break;   
    case 2: //按键2短按、并松手 
    lv_scr_load(guider_ui.screen_1);  
    break; 		
		case 3:   //按键3短按，并松手
//			lv_event_send(guider_ui.screen_btn_1, LV_EVENT_CLICKED, NULL);
			break;	
		case 4: //按键4短按并松手 
   //参数1   消息队列的句柄 
   //参数2   待发送消息的地址 
   //参数3   等待时间 参数2有3种写法   1.0  不等待   2.非0整数   等固定时间   3.portMAX_DELAY  一直等 
   xReturned=xQueueSend(xQueue_AP,&AP_Message,0); 
   if(xReturned== pdTRUE) 
   { 
//    printf("按键4按下,发送AP消息成功\r\n"); 
    safe_printf("按键4按下,发送AP消息成功\r\n");     
   } 
   else  
   { 
//    printf("按键4按下,发送AP消息失败\r\n");   
    safe_printf("按键4按下,发送AP消息失败\r\n");      
   }    
   break;
		case 5:   //按键1长按，并松手
//      lv_switch_on(guider_ui.screen_1_sw_1, LV_ANIM_ON);  //切换开关状态
			break;	
		case 6:   //按键2长按，并松手
//      lv_switch_off(guider_ui.screen_1_sw_1, LV_ANIM_OFF);  //切换开关状态
			break;
		case 7: //按键3长按，并松手 
   /* 手动触发按钮的短按点击事件 */ 
 
   // 向指定按钮对象发送一个短按点击事件 
   // btn: 目标按钮对象（lv_obj_t* 类型） 
   // LV_EVENT_SHORT_CLICKED: 事件类型，表示短按点击事件（区别于长按） 
   // NULL: 附加的事件参数，这里不需要传递额外数据所以设为NULL 
   lv_obj_set_hidden(guider_ui.screen_1_wifiON,true); 
   lv_obj_set_hidden(guider_ui.screen_1_wifiOFF,false); 
   break;   
  case 8: //按键4长按，并松手 
   lv_obj_set_hidden(guider_ui.screen_1_wifiON,false); 
   lv_obj_set_hidden(guider_ui.screen_1_wifiOFF,true);  
   break; 		
		default:
			break;		
	}
}


/*	if((GPIOA->IDR & (0x01<<0))!=0x00)	
uint32_t data = GPIOA->IDR;
(GPIOA->IDR & (0x01<<0)   按位与，把不关心的位清0，只留下关心的位
去判断关心的位是0还是1  
*/













