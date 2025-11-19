//#include "pwm.h" 
/* 
R   PA8   TIM1_CH1 
G   PA7   TIM3_CH2 
B   PA6   TIM3_CH1 
*/ 
#include "pwm.h"
#include "main.h"

void PWM_Config(void) 
{ 
//1.开时钟   
/*中文固件库翻译手册  RCC_APB2PeriphClockCmd  15.2.22 
 stm32f10x_rcc.h  693行*/  
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE); //中文固件库翻译手册  Table 373 
  
 //2.定义结构体,目的是给XXX_Init函数准备参数 
//void GPIO_Init(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef* GPIO_InitStruct)  
/*中文固件库翻译手册  GPIO_Init  10.2.3 
 stm32f10x_gpio.h  351行*/ 
 GPIO_InitTypeDef GPIO_InitStruct={0};   //变量名无所谓 
  
 //3.给结构体赋值 
 GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出   中文固件库翻译手册  Table 185 
 GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 ; //待设置的引脚 中文固件库翻译手册  Table 183 
 GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;  //GPIO口的速率  中文固件库翻译手册  Table 184 
  
 //4.调用XXX_Init函数，将参数写入到寄存器中 
 GPIO_Init(GPIOA,&GPIO_InitStruct);  
 
 //5.开启定时器的时钟 
//void RCC_APB2PeriphClockCmd(uint32_t RCC_APB2Periph, FunctionalState NewState); stm32f10x_rcc.h 693行 
//void RCC_APB1PeriphClockCmd(uint32_t RCC_APB1Periph, FunctionalState NewState); 
 RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE); 
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);   
 //6.定义定时器的结构体 
//void TIM_TimeBaseInit(TIM_TypeDef* TIMx, TIM_TimeBaseInitTypeDef* TIM_TimeBaseInitStruct);  stm32f10x_rcc.h 1055行 
 TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct={0}; 
  
 //7.给结构体赋值 
// TIM_TimeBaseInitStruct.TIM_ClockDivision = //分频因子  输入捕获使用  
 TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;//计数模式  
 TIM_TimeBaseInitStruct.TIM_Period = RGB_ARR;//重装载值 
 TIM_TimeBaseInitStruct.TIM_Prescaler = RGB_PSC;//预分频值 
// TIM_TimeBaseInitStruct.TIM_RepetitionCounter //只有TIM1和TIM8使用，作业也不需要 
  
 //8.调用XXX_Init，将参数写入到寄存器中 
 TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStruct); 
 TIM_TimeBaseInit(TIM1,&TIM_TimeBaseInitStruct);  
 //9.定时器使能 
//void TIM_Cmd(TIM_TypeDef* TIMx, FunctionalState NewState);  stm32f10x_tim.h 1067行 
 TIM_Cmd(TIM3,ENABLE);  
 TIM_Cmd(TIM1,ENABLE);  
 //10.定义结构体   TIM1--TIM5+TIM8，每个定时器有4个输出比较或者输入捕获通道，一个通道对应一个函数 
//void TIM_OC1Init(TIM_TypeDef* TIMx, TIM_OCInitTypeDef* TIM_OCInitStruct); 
//void TIM_OC2Init(TIM_TypeDef* TIMx, TIM_OCInitTypeDef* TIM_OCInitStruct);
//void TIM_OC3Init(TIM_TypeDef* TIMx, TIM_OCInitTypeDef* TIM_OCInitStruct); 
//void TIM_OC4Init(TIM_TypeDef* TIMx, TIM_OCInitTypeDef* TIM_OCInitStruct);  
 TIM_OCInitTypeDef TIM_OCInitStruct={0};   
 //11.给结构体赋值 
 TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;//PWM模式 
 TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;//有效电平 
 TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;//输出是否使能 
 TIM_OCInitStruct.TIM_Pulse = 66;//比较值 0--ARR之间 TIM_TimeBaseInitStruct.TIM_Period = 100-1; 
// TIM_OCInitStruct.TIM_OCIdleState  //只用于高级定时器，作业也不需要设置 
// TIM_OCInitStruct.TIM_OutputNState  
// TIM_OCInitStruct.TIM_OCNIdleState 
// TIM_OCInitStruct.TIM_OCNPolarity 
  
 //12.将参数写入到寄存器中 
 TIM_OC1Init(TIM3,&TIM_OCInitStruct); 
 TIM_OC2Init(TIM3,&TIM_OCInitStruct); 
 TIM_OC1Init(TIM1,&TIM_OCInitStruct); 
 //13.使能捕获/比较寄存器的预装载功能 
  TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);  
  TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);  
  TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable); 
 //14.使能重装载寄存器的预装载功能 
  TIM_ARRPreloadConfig(TIM3, ENABLE);  
  TIM_ARRPreloadConfig(TIM1, ENABLE);   
 //15.高级定时器需要加上这句话，才能输出PWM 
//void TIM_CtrlPWMOutputs(TIM_TypeDef* TIMx, FunctionalState NewState);  
  TIM_CtrlPWMOutputs(TIM1, ENABLE); 
	TIM_SetCompare1(TIM1, 100);
	TIM_SetCompare2(TIM3, 100);
	TIM_SetCompare1(TIM3, 100);
} 
 
/* 
重点关注： 
   ARR  100（TIMx_ARR）    比较值  66(TIMx_CCR ) 
   PWM模式 （TIMx_CCMRx位6:4）有效电平(TIMx_CCER   位CCxP位1) 
 
PWM1  有效电平是高电平 -- TIMx_CCMRx位6:4填入110 ; TIMx_CCER位1填入 0 
  向上计数 0—66  输出有效电平(高电平)   66—100 输出无效电平(低电平)  占空比 CCR/ARR 
  向下计数 100—66 输出无效电平(低电平)  66—0 输出有效电平(高电平)    占空比 CCR/ARR 
PWM1  有效电平是低电平 -- TIMx_CCMRx位6:4填入110 ; TIMx_CCER位1填入 1 
 向上计数 0—66  输出有效电平(低电平)   66—100 输出无效电平(高电平)  占空比(ARR-CCR)/ARR 
  向下计数 100—66 输出无效电平(高电平)  66—0   输出有效电平(低电平)  占空比(ARR-CCR)/ARR 
PWM2  有效电平是高电平 
 
PWM2  有效电平是低电平 
 向上计数 0—66  输出无效电平(高电平)   66—100 输出有效电平(低电平)   占空比 CCR/ARR 
  向下计数 100—66输出有效电平(低电平)   66—0 输出无效电平(高电平)     占空比 CCR/ARR 
注意: CCR/ARR并不一定是占空比,取决于PWM模式和有效电平，如果想CCR/ARR就是占空比，应该选择
PWM1，有效电平高电平或者PWM2，有效电平低电平 
*/ 
 
//修改比较值的函数  1个通道用1个 
//void TIM_SetCompare1(TIM_TypeDef* TIMx, uint16_t Compare1); 
//void TIM_SetCompare2(TIM_TypeDef* TIMx, uint16_t Compare2); 
//void TIM_SetCompare3(TIM_TypeDef* TIMx, uint16_t Compare3); 
//void TIM_SetCompare4(TIM_TypeDef* TIMx, uint16_t Compare4); 