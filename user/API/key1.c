#include "key1.h"
#include "delay.h"
#include "led.h"
#include "relay.h"
#include "ele_machine.h"
#include "main.h"
#include "uart1.h"
#include "stdio.h"
#include "string.h"
#include "rtctime.h"
#include "main.h"
#include "lvgl_app.h"
void Key_Handle()
{
 uint8_t Message1=0x12; 
 uint8_t Message2=0x34;  
 uint8_t Message3=0x56; 
 uint8_t Message4=0x78;  
 BaseType_t xReturned;   //接收返回值 
 uint8_t KEY_Value=KEY_Scan();//按键值 
 switch(KEY_Value) 
 { 
  case 0:
   break; 
  case 1://按键1短按，并松手 
   //参数1   消息队列的句柄 
   //参数2   待发送消息的地址 
   //参数3   等待时间 参数2有3种写法   1.0  不等待   2.非0整数   等固定时间   3.portMAX_DELAY  一直等 
   lv_obj_set_pos(par,100,100);   //修改父对象的位置 
   break;  
  case 2://按键2短按，并松手 
  lv_obj_set_pos(obj1,-20,-10);   //修改子对象的位置
   break;      
  case 3://按键3短按，并松手 
   lv_obj_del(obj1);   //删除子对象 
   break;  
  case 4://按键4短按，并松手 
   lv_obj_del(par);   //删除父对象
   break;  
  case 5://按键1长按，并松手 
   
   break;  
  case 6://按键2长按，并松手 
 
   break;   
  case 7://按键3长按，并松手 
		 break;  
  case 8://按键4长按，并松手 
 
   break;    
  default: 
   break; 
 } 
}
//void Key_Handle()
//{
//	BaseType_t xReturned;   //接收返回值 
//	uint8_t Key_Vaule=0;
//	Key_Vaule=KEY_Scan();
//	uint8_t msg[6]={0x31,0x32,0x33,0x34,0x35,0x36};
//	uint8_t buf[]="good good study,day day up\r\n";
//	const uint16_t len=sizeof(buf)-1;
//	switch(Key_Vaule)
//	{
//		case 0:   //无按键按下

//			break;
//		case 1:   //按键1短按，风扇最快
////      Keys_Process_FLAG=1;
//		xSemaphoreGive(xSemaphore);   //释放信号量
//			break;	
//		case 2:   //按键2短按，风扇稍快
////      Keys_Process_FLAG=2;
//		xReturned=xSemaphoreGive(xSemaphore_Count);   //释放信号量 
//   if(xReturned == pdTRUE) 
//   { 
//    printf("按键2按下,成功释放1个信号量\r\n"); 
//   } 
//   else  
//   { 
//    printf("按键2按下,成功释放信号量失败\r\n");     
//   } 
//			break;
//		case 3:   //按键3短按，风扇一般
////      Keys_Process_FLAG=3;
//   xReturned=xSemaphoreTake(xSemaphore_Count,0); 
//   if(xReturned == pdTRUE) 
//   { 
//    printf("按键3按下,获取到计数信号量成功\r\n"); 
//   } 
//   else  
//   { 
//    printf("按键3按下,获取到计数信号量失败\r\n");    
//   }  
//			break;	
//		case 4:   //按键4短按，风扇停转
////      TIM_SetCompare3(TIM4, 0);
//		vTaskSuspend(xHandle_RELAY);
//			break;
//		case 5:   //按键1长按，青
//		vTaskSuspend(NULL);	
//			break;	
//		case 6:   //按键2长按，蓝
//			vTaskResume(xHandle_LED1);
//			break;
//		case 7:   //按键3长按，紫
//			vTaskResume(xHandle_RELAY);
//			break;
//    case 8:

//      break;		
//		default:
//			break;	
//	}
//}
//	uint8_t i;
//	switch(Key_Vaule)
//	{
//		case 0:   //无按键按下

//			break;
//		case 1:   //按键1短按，红
//			TIM_SetCompare1(TIM1, 0);
//		  TIM_SetCompare2(TIM3, 100);
//		  TIM_SetCompare1(TIM3, 100);
//			break;	
//		case 2:   //按键2短按，橙
//			TIM_SetCompare1(TIM1, 100);
//		  TIM_SetCompare2(TIM3, 100);
//		  TIM_SetCompare1(TIM3, 100);
//			break;
//		case 3:   //按键3短按，黄
//			TIM_SetCompare1(TIM1, 0);
//		  TIM_SetCompare2(TIM3, 0);
//		  TIM_SetCompare1(TIM3, 100);
//			break;	
//		case 4:   //按键4短按，蓝呼吸灯
//			TIM_SetCompare1(TIM1, 100);
//		  TIM_SetCompare2(TIM3, 100);
//			while(i<100)
//			{
//				TIM_SetCompare1(TIM3, i);
//				i++;
//				Delay_ms(10);
//			}
//			while(i>0)
//			{
//				TIM_SetCompare1(TIM3, i);
//				i--;
//				Delay_ms(10);
//			}		
//			break;
//		case 5:   //按键1长按，青
//			TIM_SetCompare1(TIM1, 100);
//		  TIM_SetCompare2(TIM3, 0);
//		  TIM_SetCompare1(TIM3, 0);
//			break;	
//		case 6:   //按键2长按，蓝
//			TIM_SetCompare1(TIM1, 100);
//		  TIM_SetCompare2(TIM3, 100);
//		  TIM_SetCompare1(TIM3, 0);
//			break;
//		case 7:   //按键3长按，紫
//			TIM_SetCompare1(TIM1, 0);
//		  TIM_SetCompare2(TIM3, 100);
//		  TIM_SetCompare1(TIM3, 0);
//			break;	
//		case 8:   //按键4长按，蓝
//			TIM_SetCompare1(TIM1, 100);
//		  TIM_SetCompare2(TIM3, 100);
//			TIM_SetCompare1(TIM3, 0);
//			break;		
//		default:
//			break;	
//	}
//}

//按键初始化 
void KEY_Config(void) 
{ 
	#if (USE_STD_LIB==0)
 //1.开启GPIOA和GPIOC的时钟   RCC_APB2ENR的位2置1和位4置1  
  RCC->APB2ENR |= (0x01<<2); 
  RCC->APB2ENR |= (0x01<<4); 
 //2.设置PA0 PC4的工作模式 选择浮空输入 
  GPIOA->CRL &= ~(0x0F<<0);   //将PA0所在位3:0清0 
  GPIOA->CRL |= (0x04<<0);    //将PA0所在位3:0写入0100  
  
  GPIOC->CRL &= ~(0x0F<<16);   //将PC4所在位19:16清0 
  GPIOC->CRL |= (0x04<<16);    //将PC4所在位19:16写入0100 
  GPIOC->CRL &= ~(0x0F<<20);   //将PC5所在位23:20清0
	GPIOC->CRL |= (0x04<<20);    //将PC5所在位23:20写入0100 
	GPIOC->CRL &= ~(0x0F<<24);   //将PC6所在位27:24清0
  GPIOC->CRL |= (0x04<<24);    //将PC6所在位27:24写入0100 
	#elif (USE_STD_LIB==1)
  	
	//1.开时钟     
/*中文固件库翻译手册  RCC_APB2PeriphClockCmd  15.2.22  表格373
	stm32f10x_rcc.h  693行*/	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC,ENABLE);
	//2.定义结构体,目的是给XXX_Init函数准备参数
//void GPIO_Init(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef* GPIO_InitStruct)	
/*中文固件库翻译手册  GPIO_Init  10.2.3
	stm32f10x_gpio.h  351行*/
	GPIO_InitTypeDef GPIO_InitStruct={0}; //变量名无所谓
	//3.给结构体赋值
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING; //浮空输入  Table185
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0; //待设置的引脚
	//GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;  //GPIO口的速率  Table184
	
	//4.调用XXX_Init函数，将参数写入到寄存器中
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6;
	GPIO_Init(GPIOC,&GPIO_InitStruct);
	#endif
}
//按键扫描函数
/*0无按键按下
*/
uint8_t KEY_Scan(void)
{
	//uint8_t KEY_Value = 0;//?????
	#if(KEY_SCAN_MODE == 0)
	{
		uint8_t KEY_Value = 0;//?????
		if((GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)) != SET)
		{
			Delay_ms(10);
			if((GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)) != SET)
			{
				while((GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)) != SET)
				{
					
				}
				KEY_Value = 1;
			}
		}
		if((GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_4)) != RESET)
		{
			Delay_ms(10);
			if((GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_4)) != RESET)
			{
				while((GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_4)) != RESET)
				{
					
				}
				KEY_Value = 2;
			}
		}
		if((GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_5)) != RESET)
		{
			Delay_ms(10);
			if((GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_5)) != RESET)
			{
				while((GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_5)) != RESET)
				{
					
				}
				KEY_Value = 3;
			}
		}
		if((GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_6)) != RESET)
		{
			Delay_ms(10);
			if((GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_6)) != RESET)
			{
				while((GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_6)) != RESET)
				{
					
				}
				KEY_Value = 4;
			}
		}
	return KEY_Value;
	}
	#elif(KEY_SCAN_MODE == 1)
	{
		static uint16_t KEY_Cnt[4] = {0};
		uint8_t KEY_Value = 0;//?????
		if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0) == Bit_SET)
		{
			KEY_Cnt[0]++;
		}
		else
		{ 
			if(KEY_Cnt[0] <= 1)
			{
				
			}
			else if(KEY_Cnt[0] > 1 && KEY_Cnt[0] <=100)
			{
				KEY_Value = 1;
			}
			else
			{
				KEY_Value = 5;
			}
			KEY_Cnt[0] = 0;
		}
		if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_4) == Bit_RESET)
		{
			KEY_Cnt[1]++;
		}
		else
		{
			if(KEY_Cnt[1] <= 1)
			{
			
			}
			else if(KEY_Cnt[1] > 1 && KEY_Cnt[1] <=100)
			{
				KEY_Value = 2;
			}
			else
			{
				KEY_Value = 6;
			}
			KEY_Cnt[1] = 0;
		}
		if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_5) == Bit_RESET)
		{
			KEY_Cnt[2]++;
		}
		else
		{
			if(KEY_Cnt[2] <= 1)
			{
			
			}
			else if(KEY_Cnt[2] > 1 && KEY_Cnt[2] <=100)
			{
				KEY_Value = 3;
			}
			else
			{
				KEY_Value = 7;
			}
			KEY_Cnt[2] = 0;
		}
		if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_6) == Bit_RESET)
		{
			KEY_Cnt[3]++;
		}
		else
		{
			if(KEY_Cnt[3] <=1)
			{
			
			}
			else if(KEY_Cnt[3] > 1 && KEY_Cnt[3] <= 100)
			{
				KEY_Value = 4;
			}
			else
			{
				KEY_Value = 8;
			}
			KEY_Cnt[3] = 0;
		}
		return KEY_Value;
	}
	#endif
}



