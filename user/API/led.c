#include "led.h"
#include "main.h"
#include "pwm.h"
uint8_t RGB_B_STATE; //0停止呼吸灯 1开始呼吸灯
uint8_t LED1_FLAG=0;
//LED初始化
void LED_Config()
{
#if (USE_STD_LIB==0)
	RCC->APB2ENR |= (0x01<<6);  //开启E时钟
	//PE2（LED1/D3）初始化
	GPIOE->CRL &= ~(0xf<<8);  	//清空 
	GPIOE->CRL |= (0x03<<8);    //通用推挽输出，50Mhz
	GPIOE->ODR |= (0x01<<2);    //置位(高电平),D3灯灭
	//PE3（LED2/D4）初始化
	GPIOE->CRL &= ~(0x0F<<12);  //清空
	GPIOE->CRL |= (0x03<<12);   //通用推挽输出，50Mhz
	GPIOE->ODR |= (0x01<<3);		//置位(高电平),D4灯灭
	//PE4（LED3/D5）初始化
	GPIOE->CRL &= ~(0X0F<<16);  //清空
	GPIOE->CRL |= (0X03<<16);		//通用推挽输出，50Mhz
	GPIOE->ODR |= (0X01<<4);    //置位(高电平),D5灯灭
	//PE5（LED4/D6）初始化
	GPIOE->CRL &= ~(0X0F<<20);  //清空
	GPIOE->CRL |= (0X03<<20);   //通用推挽输出，50Mhz
	GPIOE->ODR |= (0X01<<5);    //置位(高电平),D6灯灭
	
#elif (USE_STD_LIB==1)
	//1.开时钟     
/*中文固件库翻译手册  RCC_APB2PeriphClockCmd  15.2.22
	stm32f10x_rcc.h  693行*/	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE);
	
	//2.定义结构体,目的是给XXX_Init函数准备参数
//void GPIO_Init(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef* GPIO_InitStruct)	
/*中文固件库翻译手册  GPIO_Init  10.2.3
	stm32f10x_gpio.h  351行*/
	GPIO_InitTypeDef GPIO_InitStruct={0}; //变量名无所谓
	//3.给结构体赋值
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP; //通用推挽输出  
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5; //待设置的引脚
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;  //GPIO口的速率
	
	//4.调用XXX_Init函数，将参数写入到寄存器中
	GPIO_Init(GPIOE,&GPIO_InitStruct);
	//将四个灯置于灭的状态
	GPIO_WriteBit(GPIOE,GPIO_Pin_2,(BitAction)(1-(GPIO_ReadOutputDataBit(GPIOE,GPIO_Pin_2))));
	GPIO_WriteBit(GPIOE,GPIO_Pin_3,(BitAction)(1-(GPIO_ReadOutputDataBit(GPIOE,GPIO_Pin_3))));
	GPIO_WriteBit(GPIOE,GPIO_Pin_4,(BitAction)(1-(GPIO_ReadOutputDataBit(GPIOE,GPIO_Pin_4))));
	GPIO_WriteBit(GPIOE,GPIO_Pin_5,(BitAction)(1-(GPIO_ReadOutputDataBit(GPIOE,GPIO_Pin_5))));
#endif
}
void LED1_ON()
{
  #if (USE_STD_LIB==0)
	GPIOE->ODR &= ~(0x0f<<2);
	#elif (USE_STD_LIB==1)	
/*中文固件库翻译手册  GPIO_ResetBits  10.2.10
	stm32f10x_gpio.h  358行*/
	GPIO_ResetBits(GPIOE,GPIO_Pin_2);
	#endif
}
void LED2_ON()
{
	GPIO_ResetBits(GPIOE,GPIO_Pin_3);
}
void LED3_ON()
{
	GPIO_ResetBits(GPIOE,GPIO_Pin_4);
	LED1_FLAG=1;
}
void LED4_ON()
{
	GPIO_ResetBits(GPIOE,GPIO_Pin_5);
}
void LED1_OFF()
{
  #if (USE_STD_LIB==0)
		GPIOE->ODR |=(0x0f<<2);
	#elif (USE_STD_LIB==1)	
/*中文固件库翻译手册  GPIO_SetBits  10.2.9
	stm32f10x_gpio.h  357行*/	
	GPIO_SetBits(GPIOE,GPIO_Pin_2);
	#endif
}
void LED2_OFF()
{
	GPIO_SetBits(GPIOE,GPIO_Pin_3);
}
void LED3_OFF()
{
	GPIO_SetBits(GPIOE,GPIO_Pin_4);
	LED1_FLAG=0;
}
void LED4_OFF()
{
	GPIO_SetBits(GPIOE,GPIO_Pin_5);
}
void LED1_TOGGLE()
{
	#if (USE_STD_LIB==0)
	GPIOE->ODR ^= (0x0F<<2);
	#elif (USE_STD_LIB==1)
	/*uint8_t Bit_Value=0;
	Bit_Value=GPIO_ReadOutputDataBit(GPIOE,GPIO_Pin_2);
	if(Bit_Value==Bit_SET)
	{
		GPIO_ResetBits(GPIOE,GPIO_Pin_2);
	}
	else
	{
		GPIO_SetBits(GPIOE,GPIO_Pin_2);
	}*/
	GPIO_WriteBit(GPIOE,GPIO_Pin_2,(BitAction)(1-(GPIO_ReadOutputDataBit(GPIOE,GPIO_Pin_2))));
	#endif
}
void LED2_TOGGLE()
{
	#if (USE_STD_LIB==0)
	GPIOE->ODR ^= (0x0F<<2);
	#elif (USE_STD_LIB==1)
	/*uint8_t Bit_Value=0;
	Bit_Value=GPIO_ReadOutputDataBit(GPIOE,GPIO_Pin_2);
	if(Bit_Value==Bit_SET)
	{
		GPIO_ResetBits(GPIOE,GPIO_Pin_2);
	}
	else
	{
		GPIO_SetBits(GPIOE,GPIO_Pin_2);
	}*/
	GPIO_WriteBit(GPIOE,GPIO_Pin_3,(BitAction)(1-(GPIO_ReadOutputDataBit(GPIOE,GPIO_Pin_3))));
	#endif
}
void LED3_TOGGLE()
{
	#if (USE_STD_LIB==0)
	GPIOE->ODR ^= (0x0F<<2);
	#elif (USE_STD_LIB==1)
	/*uint8_t Bit_Value=0;
	Bit_Value=GPIO_ReadOutputDataBit(GPIOE,GPIO_Pin_2);
	if(Bit_Value==Bit_SET)
	{
		GPIO_ResetBits(GPIOE,GPIO_Pin_2);
	}
	else
	{
		GPIO_SetBits(GPIOE,GPIO_Pin_2);
	}*/
	GPIO_WriteBit(GPIOE,GPIO_Pin_4,(BitAction)(1-(GPIO_ReadOutputDataBit(GPIOE,GPIO_Pin_4))));
	#endif
}
void LED4_TOGGLE()
{
	#if (USE_STD_LIB==0)
	GPIOE->ODR ^= (0x0F<<2);
	#elif (USE_STD_LIB==1)
	/*uint8_t Bit_Value=0;
	Bit_Value=GPIO_ReadOutputDataBit(GPIOE,GPIO_Pin_2);
	if(Bit_Value==Bit_SET)
	{
		GPIO_ResetBits(GPIOE,GPIO_Pin_2);
	}
	else
	{
		GPIO_SetBits(GPIOE,GPIO_Pin_2);
	}*/
	GPIO_WriteBit(GPIOE,GPIO_Pin_5,(BitAction)(1-(GPIO_ReadOutputDataBit(GPIOE,GPIO_Pin_5))));
	#endif
}
void RGB_Config()
{
#if (USE_STD_LIB==0)
	RCC->APB2ENR |= (0x01<<2);  //开启A时钟
	//RGB灯初始化配置
	GPIOA->CRL &= ~(0xFF<<24);  //清空 PA6 PA7
	GPIOA->CRL |= (0x03<<24);   //通用推挽输出，50Mhz 
	GPIOA->CRL |= (0x03<<28);   //通用推挽输出，50Mhz  
	GPIOA->CRH &= ~(0x0F<<0);   //清空 PA8
	GPIOA->CRH |= (0x03<<0);   	//通用推挽输出，50Mhz
	GPIOA->ODR |= (0x07<<6);    //置位(高电平),三灯灭
#elif (USE_STD_LIB==1)
	//1.开时钟     
/*中文固件库翻译手册  RCC_APB2PeriphClockCmd  15.2.22
	stm32f10x_rcc.h  693行*/	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	//2.定义结构体,目的是给XXX_Init函数准备参数
//void GPIO_Init(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef* GPIO_InitStruct)	
/*中文固件库翻译手册  GPIO_Init  10.2.3
	stm32f10x_gpio.h  351行*/
	GPIO_InitTypeDef GPIO_InitStruct={0}; //变量名无所谓
	//3.给结构体赋值
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP; //通用推挽输出  
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8; //待设置的引脚
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;  //GPIO口的速率
	
	//4.调用XXX_Init函数，将参数写入到寄存器中
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	GPIO_WriteBit(GPIOA,GPIO_Pin_6,(BitAction)(1-(GPIO_ReadOutputDataBit(GPIOE,GPIO_Pin_6))));
	GPIO_WriteBit(GPIOA,GPIO_Pin_7,(BitAction)(1-(GPIO_ReadOutputDataBit(GPIOE,GPIO_Pin_7))));
	GPIO_WriteBit(GPIOA,GPIO_Pin_8,(BitAction)(1-(GPIO_ReadOutputDataBit(GPIOE,GPIO_Pin_8))));
#endif
}
void RGB_B_ON()
{
	#if (USE_STD_LIB==0)
	GPIOA->ODR &= ~(0x01<<6);
	#elif (USE_STD_LIB==1)
	GPIO_ResetBits(GPIOA,GPIO_Pin_6);
	#endif
}
void RGB_B_OFF()
{
	#if (USE_STD_LIB==0)
	GPIOA->ODR |= (0x01<<6);
	#elif (USE_STD_LIB==1)
	GPIO_SetBits(GPIOA,GPIO_Pin_6);
	#endif
}
void RGB_G_ON()
{
  #if (USE_STD_LIB==0)
	GPIOA->ODR &= ~(0x01<<7);
	#elif (USE_STD_LIB==1)
	GPIO_ResetBits(GPIOA,GPIO_Pin_7);
	#endif
}
void RGB_G_OFF()
{
	#if (USE_STD_LIB==0)
	GPIOA->ODR |= (0x01<<7);
	#elif (USE_STD_LIB==1)
	GPIO_SetBits(GPIOA,GPIO_Pin_7);
	#endif
}
void RGB_R_ON()
{
	#if (USE_STD_LIB==0)
	GPIOA->ODR &= ~(0x01<<8);
	#elif (USE_STD_LIB==1)
	GPIO_ResetBits(GPIOA,GPIO_Pin_8);
	#endif
}
void RGB_R_OFF()
{
	#if (USE_STD_LIB==0)
	GPIOA->ODR |= (0x01<<8);
	#elif (USE_STD_LIB==1)
	GPIO_SetBits(GPIOA,GPIO_Pin_8);
	#endif
}
void RGB_B_Show()
{
	static uint8_t RGB_B_Dir=0;
	static uint8_t RGB_B_Cnt=0;
	if(RGB_B_Dir==0)
	{
		RGB_B_Cnt++;
		if(RGB_B_Cnt>=RGB_CCR1)
		{
			RGB_B_Dir=1;
		}
	}
	else
	{
		RGB_B_Cnt--;
		if(RGB_B_Cnt<=0)
		{
			RGB_B_Dir=0;
		}
	}
	TIM_SetCompare1(TIM3, RGB_B_Cnt);
	TIM_SetCompare2(TIM3, 100);
	TIM_SetCompare1(TIM1, 100);
}