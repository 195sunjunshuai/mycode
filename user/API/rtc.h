#ifndef __RTC_H_ 
#define __RTC_H_ 
 
#include "stm32f10x.h"
extern uint8_t RTC_Second_FLAG;
extern uint32_t  Now_Time_Cnt;  //当前时间戳秒数
extern struct tm *Now_Time;
extern struct tm Set_Time;
extern struct tm Set_Time;
extern uint32_t  Set_Time_Cnt;
extern uint8_t RTC_Second_FLAG;
void RTC_IRQHandler(void);
void Time_Adjust(void);
void Time_Adjust2(void);
void RTC_Config();
#endif