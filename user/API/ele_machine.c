#include "ele_machine.h"
#include "main.h"
//电机初始化
void Machine_Config()
{
	#if (USE_STD_LIB==0)
  RCC->APB2ENR |=(0x01<<3);
	GPIOB->CRH &= ~(0x0F<<0);
	GPIOB->CRH |= (0x03<<0);
	GPIOB->CRH &= ~(0x0F<<4);
	GPIOB->CRH |= (0x03<<4);
	GPIOB->ODR &= ~(0x01<<8) ;
	GPIOB->ODR &= ~(0x01<<9) ;
	#elif (USE_STD_LIB==1)
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct={0}; //变量名无所谓
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_PP; //复用推挽输出
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_8|GPIO_Pin_9;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	GPIO_ResetBits(GPIOB,  GPIO_Pin_9);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct={0};
  TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;//计数模式  
  TIM_TimeBaseInitStruct.TIM_Period = 100-1;//重装载值 
  TIM_TimeBaseInitStruct.TIM_Prescaler = 720-1;//预分频值 
  TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStruct); 
  TIM_Cmd(TIM4,ENABLE);  
  TIM_OCInitTypeDef TIM_OCInitStruct={0};
  TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;//PWM模式 
  TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;//有效电平 
  TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;//输出是否使能 
  TIM_OCInitStruct.TIM_Pulse = 66;//比较值 0--ARR之间 TIM_TimeBaseInitStruct.TIM_Period = 100-1; 
  TIM_OC3Init(TIM4,&TIM_OCInitStruct);
 	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable); 
  TIM_ARRPreloadConfig(TIM4, ENABLE);
  TIM_SetCompare3(TIM4, 0);	
	#endif
}
void Machine_forward()
{
	#if (USE_STD_LIB==0)
	GPIOB->ODR |= (0x01<<8);
	GPIOB->ODR &= ~(0x01<<9);
	#elif (USE_STD_LIB==1)
	GPIO_SetBits(GPIOB,GPIO_Pin_8);
	GPIO_ResetBits(GPIOB,GPIO_Pin_9);
	TIM_SetCompare3(TIM4, 66);
	#endif
}
void Machine_forward_3()
{
	TIM_SetCompare3(TIM4, 100);
}
void Machine_forward_2()
{
	TIM_SetCompare3(TIM4, 66);
}
void Machine_forward_1()
{
	TIM_SetCompare3(TIM4, 33);
}
void Machine_forward_0()
{
	TIM_SetCompare3(TIM4, 0);
}
void Machine_Stop()
{
	#if (USE_STD_LIB==0)
  GPIOB->ODR &= ~(1<<8);
	GPIOB->ODR &= ~(1<<9);
	#elif (USE_STD_LIB==1)
	GPIO_ResetBits(GPIOB, GPIO_Pin_8 | GPIO_Pin_9);
	TIM_SetCompare3(TIM4, 0);
	#endif
}
void Machine_reverse()
{
	#if (USE_STD_LIB==0)
	GPIOB->ODR |= (0x01<<9);
	GPIOB->ODR &= ~(0x01<<8);
	#elif (USE_STD_LIB==1)
	GPIO_SetBits(GPIOB,GPIO_Pin_9);
	GPIO_ResetBits(GPIOB,GPIO_Pin_8);
	#endif
}
void Machine_TOGGLE()
{
	#if (USE_STD_LIB==0)
	GPIOB->ODR ^= (0x01<<8);
	GPIOB->ODR ^= (0x01<<9);
	#elif (USE_STD_LIB==1)
	GPIO_WriteBit(GPIOB,GPIO_Pin_8,(BitAction)(1-(GPIO_ReadOutputDataBit(GPIOB,GPIO_Pin_8))));
	GPIO_WriteBit(GPIOB,GPIO_Pin_9,(BitAction)(1-(GPIO_ReadOutputDataBit(GPIOB,GPIO_Pin_9))));
	#endif
}