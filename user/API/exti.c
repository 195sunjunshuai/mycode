#include "exti.h" 
#include "main.h" 
#include "led.h" 
#include "relay.h"
#include "delay.h"

void EXTI_Config1()
{
	//GPIO的配置（1，2）
	//1.开启X端口时钟
	RCC->APB2ENR |= (0x01<<2);  //开启A时钟
	RCC->APB2ENR |= (0x01<<4);  //开启C时钟
	
	//2.设置工作模式（PA0，PC4~6）
	GPIOA->CRL &= ~(0x0F<<0);
	GPIOA->CRL |= (0x04<<0);    //先将PA0清零，再配置为浮空输入
	
	GPIOC->CRL &= ~(0x0FFF<<16);
	GPIOC->CRL |= (0x0444<<16); //先将PC4~6清零，再配置为浮空输入
	
	
	//EXTI的配置（3,4,5,6）
	//3.开启AFIO时钟
	RCC->APB2ENR |= (0x01<<0);
	
	/*4.将PA0映射到EXTI0上，PC4映射到EXTI4上，
	    PC5映射到EXTI5上，PC6映射到EXTI6上*/
	AFIO->EXTICR[0] &= ~(0X0F<<0);
	AFIO->EXTICR[0] |= (0X00<<0);   //PA0
	
	AFIO->EXTICR[1] &= ~(0X0F<<0);
	AFIO->EXTICR[1] |= (0X02<<0);   //PC4
	
	AFIO->EXTICR[1] &= ~(0X0F<<4);
	AFIO->EXTICR[1] |= (0X02<<4);   //PC5
	
	AFIO->EXTICR[1] &= ~(0X0F<<8);
	AFIO->EXTICR[1] |= (0X02<<8);   //PC6
	
	//5.配置对应的边沿触发模式
	EXTI->FTSR |= (0X01<<0);    //PA0
	EXTI->RTSR |= (0X01<<4);    //PC4
	EXTI->RTSR |= (0X01<<5);    //PC5
	EXTI->RTSR |= (0X01<<6);    //PC6
	
	//6.使能对应的引脚中断
	EXTI->IMR |= (0X01<<0);
	EXTI->IMR |= (0X01<<4);
	EXTI->IMR |= (0X01<<5);
	EXTI->IMR |= (0X01<<6);
	
	//NVIC的配置（7,8）
	//7.NVIC中设置中断优先级
	/*选择中断优先级分组5，2位抢占，2位次级，抢占和次级优先级范围0--3 
假如            抢占        次级(子、响应)      优先级寄存器填入 
EXTI0_IRQn       0            2                   0b 00100000    PA0
EXTI4_IRQn       0            1                   0b 01010000    PC4
EXTI9_5_IRQn     1            2                   0b 01100000    PC5,PC6
NVIC_SetPriority第二个参数，会自动参数左移4位 
*/ 
  NVIC_SetPriority(EXTI0_IRQn,2);   //core_cm3.h 1439行     
	NVIC_SetPriority(EXTI4_IRQn,1);
  NVIC_SetPriority(EXTI9_5_IRQn,3);
	//8.NVIC中设置EXTI0、EXTI4、EXTI5、EXTI6中断使能 
  NVIC_EnableIRQ(EXTI0_IRQn);       //core_cm3.h 1432行  
  NVIC_EnableIRQ(EXTI4_IRQn);  	
  NVIC_EnableIRQ(EXTI9_5_IRQn);
}







//配置3大部分：GPIO   EXTI   NVIC 
//EXTI 的初始化 
void EXTI_Config(void) 
{ 
#if (USE_STD_LIB==0) 
	
//1.开启 GPIOA 和GPIOC的时钟   RCC_APB2ENR的位2置1和位4置1  
RCC->APB2ENR |= (0x05<<2);   //RCC_APB2ENR 的位 2置 1和位 4置1

	
//2.设置 PA0 PC4--PC6 的工作模式 选择浮空输入 
GPIOA->CRL &= ~(0x0F<<0);   //将 PA0 所在位3:0清0 
GPIOA->CRL |= (0x04<<0);    //将 PA0 所在位3:0写入0100  
GPIOC->CRL &= ~(0x0FFF<<16);   //将 PC4--6 所在位 27:16 清 0 
GPIOC->CRL |= (0x0444<<16);    //将 PC4--6 所在位27:16写入0100 0100 0100 
//3.开启 AFIO 时钟    参考手册6.3.7的RCC_APB2ENR的位0置1 
RCC->APB2ENR |= (0x01<<0); 
//4.选择引脚映射到对应EXTI上   参考手册8.4.3  -- 8.4.6 
AFIO->EXTICR[0] &= ~(0x0F<<0);       //参考手册8.4.3 位[3:0]清 0，PA0映射到EXTI0上
AFIO->EXTICR[1] &= ~(0X0F<<0);
AFIO->EXTICR[1] |= (0X02<<0);   //PC4
AFIO->EXTICR[1] &= ~(0X0F<<4);
AFIO->EXTICR[1] |= (0X02<<4);   //PC5	
AFIO->EXTICR[1] &= ~(0x0F<<8);       //参考手册8.4.4 位[3:0]清 0 
AFIO->EXTICR[1] |= (0x02<<8);        //参考手册8.4.4 位[3:0]写入0010，PC6 映射到EXTI6上
//5.配置对应的边沿触发模式，按键检测选择后面的边沿 
 EXTI->FTSR |= (0x01<<0);             //参考手册9.3.4位0置1，PA0使能EXTI0的下降沿 
 EXTI->RTSR |= (0X01<<4);    //PC4
EXTI->RTSR |= (0X01<<5);    //PC5
 EXTI->RTSR |= (0x01<<6);             //参考手册9.3.3位4置1，PC6使能EXTI4的上升沿   
  
 //6.使能对应的引脚中断 
 EXTI->IMR |= (0x01<<0);           //参考手册9.3.1位0置1，使能EXTI0的中断
	EXTI->IMR |= (0X01<<4);
	EXTI->IMR |= (0X01<<5); 
 EXTI->IMR |= (0x01<<6);           //参考手册9.3.1位4置1，使能EXTI6的中断   
 
 //7.NVIC中设置EXTI0、EXTI4中断优先级 
/*选择中断优先级分组5，2位抢占，2位次级，抢占和次级优先级范围0--3 
假如            抢占        次级(子、响应)      优先级寄存器填入 
EXTI0_IRQn       1            2                   0b 01100000 
EXTI4_IRQn       0            1                   0b 00010000 
NVIC_SetPriority第二个参数，会自动参数左移4位 
*/ 
 NVIC_SetPriority(EXTI0_IRQn,6);   //core_cm3.h 1439行     
	NVIC_SetPriority(EXTI4_IRQn,5);
  NVIC_SetPriority(EXTI9_5_IRQn,1);
	//8.NVIC中设置EXTI0、EXTI4、EXTI5、EXTI6中断使能 
  NVIC_EnableIRQ(EXTI0_IRQn);       //core_cm3.h 1432行  
  NVIC_EnableIRQ(EXTI4_IRQn);  	
  NVIC_EnableIRQ(EXTI9_5_IRQn);  
#elif (USE_STD_LIB==1) 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct={0};
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING ;  
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0; //待设置的引脚
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6;
	GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource4);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource5);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource6);
	
	EXTI_InitTypeDef EXTI_InitStructure={0};
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line4|EXTI_Line5|EXTI_Line6;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_Init(&EXTI_InitStructure);
	
	NVIC_InitTypeDef NVIC_InitStructure={0};
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01; //抢占优先级
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;        //次级优先级
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
#endif 
}
/* 
1.中断服务函数，自动加载，不需要调用和声明 
2.中断服务函数的名字，必须从启动文件startup_stm32f10x_hd.s复制 
3.中断服务函数，无参，无返回值 
4.中断服务函数，尽量精简，不要调用延时 
5.退出的时候，清除标志位 
*/ 
//EXTI0的中断服务函数 
void EXTI0_IRQHandler(void) 
{ 
 #if (USE_STD_LIB==0)
 //1.检测对应中断是否触发 
 if((EXTI->PR & (0x01<<0))!=0)  //检测参考手册9.3.6的位0是否位1 
 { 
  //2.执行具体的中断服务内容 
  LED1_ON();//LED2_ON(); 
  //3.退出清除标志位  //检测参考手册9.3.6的位0写1清0,手册寄存器描述 
  EXTI->PR |= (0x01<<0);   //写1清0 
 } 
 #elif (USE_STD_LIB==1)
 if(EXTI_GetITStatus(EXTI_Line0)==SET)
	{
		LED1_ON();		  
		EXTI_ClearITPendingBit(EXTI_Line0);
	}
 #endif
} 
/* 
EXTI->PR & (0x01<<0) 
产生了对应的边沿 
EXTI->PR               XXXX XXXX XXXX XXX1 
(0x01<<0)              0000 0000 0000 0001 
EXTI->PR & (0x01<<0)   0000 0000 0000 0001 
*/ 

//EXTI4的中断服务函数 
void EXTI4_IRQHandler(void) 
{ 
 #if (USE_STD_LIB==0)
 //1.检测对应中断是否触发 
 if((EXTI->PR & (0x01<<4))!=0)  //检测参考手册9.3.6的位0是否位6
 { 
  //2.执行具体的中断服务内容 
  LED1_OFF();  
  //3.退出清除标志位  //检测参考手册9.3.6的位6写1清0,手册寄存器描述 
  EXTI->PR |= (0x01<<4);   //写1清0 
 } 
 #elif (USE_STD_LIB==1)
 if(EXTI_GetFlagStatus(EXTI_Line4)==SET)
 {
	LED1_OFF();
	EXTI_ClearITPendingBit(EXTI_Line4);
 }
 #endif	
} 
//EXTI5,6的中断服务函数 
void EXTI9_5_IRQHandler(void) 
{
 #if (USE_STD_LIB==0)
 if((EXTI->PR & (0x01<<5))!=0)  
 { 
	RELAY_ON(); 
  EXTI->PR |= (0x01<<5);   //写1清0 
 }  	
 //1.检测对应中断是否触发 
 if((EXTI->PR & (0x01<<6))!=0)  //检测参考手册9.3.6的位0是否位6
 { 
  //2.执行具体的中断服务内容 
  RELAY_OFF();   
  //3.退出清除标志位  //检测参考手册9.3.6的位6写1清0,手册寄存器描述 
  EXTI->PR |= (0x01<<6);   //写1清0 
 }
	#elif (USE_STD_LIB==1)
	if(EXTI_GetFlagStatus(EXTI_Line5)==SET)
	{
		RELAY_ON();
		EXTI_ClearITPendingBit(EXTI_Line5);
	}
	if(EXTI_GetFlagStatus(EXTI_Line6)==SET)
	{
		RELAY_OFF();
		EXTI_ClearITPendingBit(EXTI_Line6);
	}
	#endif
}