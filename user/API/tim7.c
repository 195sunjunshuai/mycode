#include "tim7.h"
#include "relay.h"
#include "led.h"
#include "main.h"
#include "delay.h"
#include "esp.h"
#include "rtc.h"
#include "time.h"
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
 
//分为2大部分：TIM   NVIC 
void TIM7_Config(void) 
{ 
 #if (USE_STD_LIB==0)
 //1.开TIM7的时钟 6.3.8
 RCC->APB1ENR |= (0x01<<5);  //RCC_APB1ENR的位5置1 
  
 //2.定时器计数器使能 
 TIM7->CR1 |= (0x01<<0);    //参考手册15.4.1的位0置1   
 
 //3.定时器更新中断使能 
 TIM7->DIER |= (0x01<<0);   //参考手册15.4.3的位0置1   
  
 //4.设置定时器的预分频值   //参考手册15.4.7 
 TIM7->PSC = 7200-1; 
  
 //5.设置定时器的重装载值   //参考手册15.4.8 
 TIM7->ARR = 20000-1; 
 
 //6.NVIC中设置中断优先级 
 NVIC_SetPriority(TIM7_IRQn,6);   //core_cm3.h 1439行        
  
 //7.NVIC中设置中断使能 
 NVIC_EnableIRQ(TIM7_IRQn);       //core_cm3.h 1432行 
 #elif (USE_STD_LIB==1)
    // 1. 打开 TIM7 时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);
    // 2. 配置 TIM7 的时基单元
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);  // 先初始化为默认值

    TIM_TimeBaseStructure.TIM_Period = 10000 - 1;    // ARR = 10000-1
    TIM_TimeBaseStructure.TIM_Prescaler = 14400 - 1;  // PSC = 14400-1
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;  // 时钟不分频
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // 向上计数
    TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure);
    // 3. 允许 TIM7 更新中断
    TIM_ITConfig(TIM7, TIM_IT_Update, ENABLE);

    // 4. 配置 NVIC
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // 抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;        // 次优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           // 使能
    NVIC_Init(&NVIC_InitStructure);
    // 5. 启动 TIM7
    TIM_Cmd(TIM7, ENABLE);
 #endif
} 
 
void TIM7_IRQHandler(void) 
{ 
 #if(USE_STD_LIB==0)
 //1.检测对应中断是否触发  
 if((TIM7->SR & (0x01<<0))!=0)  //判断参考手册15.4.4的位0是否为1 
 { 
  //2.执行具体的中断服务内容 
	RELAY_TOGGLE();
  //3.退出清除标志位  
  TIM7->SR &= ~(0x01<<0);   //参考手册 15.4.4 位0写0清0 
 } 
 #elif (USE_STD_LIB==1)
	 if (TIM_GetITStatus(TIM7, TIM_IT_Update) != RESET)  // 检查更新中断标志
    {
      PUBLISH_Period[0]+=1000;
	    PING_Period[0]+=1000;
	    onemin[0]+=100;
	    if(onemin[0]>=onemin[1])
	    {
	      onemin[0]=0;
		    NET_State=0;
	    }
	      Now_Time_Cnt=RTC_GetCounter();  //获取当前计数值
	      Now_Time=localtime(&Now_Time_Cnt); //将时间戳转换成时间结构体
        TIM_ClearITPendingBit(TIM7, TIM_IT_Update);     // 清除中断标志位
    }
 #endif
} 