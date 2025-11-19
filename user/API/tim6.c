#include "tim6.h"
#include "led.h" 
#include "main.h" 
/*想要1s的中断 
1. 定时器的频率CK_PSC  F=72000000HZ  
2. 定时器的计数频率 CK_CNT  f=F/PSC          PSC预分频值 
3. 更新一次数据的时间  t=1/f=PSC/F      
4. 计数的周期(触发中断的周期) T=ARR*t=ARR*PSC/F     ARR重装载值寄存器 
 
已知量:F=72000000HZ 
需要1s的中断周期：T=ARR*t=ARR*PSC/F=1S 
ARR*PSC=72000000 
ARR=7200   PSC=10000 
PSC=7200   ARR=10000 
ARR,PSC最大值65535 
*/ 


#if (USE_STD_LIB==0)
//分为2大部分：TIM   NVIC 
void TIM6_Config(void) 
{ 
 //1.开TIM6的时钟 6.3.8
 RCC->APB1ENR |= (0x01<<4);  //RCC_APB1ENR的位4置1 
  
 //2.定时器计数器使能 
 TIM6->CR1 |= (0x01<<0);    //参考手册15.4.1的位0置1   
 
 //3.定时器更新中断使能 
 TIM6->DIER |= (0x01<<0);   //参考手册15.4.3的位0置1   
  
 //4.设置定时器的预分频值   //参考手册15.4.7 
 TIM6->PSC = 7200-1; 
  
 //5.设置定时器的重装载值   //参考手册15.4.8 
 TIM6->ARR = 10000-1; 
 
 //6.NVIC中设置中断优先级 
 NVIC_SetPriority(TIM6_IRQn,6);   //core_cm3.h 1439行        
  
 //7.NVIC中设置中断使能 
 NVIC_EnableIRQ(TIM6_IRQn);       //core_cm3.h 1432行     
} 
 
void TIM6_IRQHandler(void) 
{ 
 //1.检测对应中断是否触发  
 if((TIM6->SR & (0x01<<0))!=0)  //判断参考手册15.4.4的位0是否为1 
 { 
  //2.执行具体的中断服务内容 
  LED1_TOGGLE();
  //3.退出清除标志位  
  TIM6->SR &= ~(0x01<<0);   //参考手册 15.4.4 位0写0清0 
 } 
} 
#elif (USE_STD_LIB==1)
void TIM6_Config(void)
{
    // 1. 打开 TIM6 时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

    // 2. 配置 TIM6 的时基单元
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);  // 先初始化为默认值

    TIM_TimeBaseStructure.TIM_Period = 10000 - 1;    // ARR = 10000-1
    TIM_TimeBaseStructure.TIM_Prescaler = 7200 - 1;  // PSC = 7200-1
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;  // 时钟不分频
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // 向上计数
    TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);

    // 3. 允许 TIM6 更新中断
    TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);

    // 4. 配置 NVIC
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // 抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;        // 次优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           // 使能
    NVIC_Init(&NVIC_InitStructure);

    // 5. 启动 TIM6
    TIM_Cmd(TIM6, ENABLE);
}

// TIM6 中断服务函数
void TIM6_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET)  // 检查更新中断标志
    {
        LED1_TOGGLE();   // LED 翻转
        TIM_ClearITPendingBit(TIM6, TIM_IT_Update);     // 清除中断标志位
    }
}
#endif