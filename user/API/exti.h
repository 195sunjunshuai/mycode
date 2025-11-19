#ifndef __EXTI_H_
#define __EXTI_H_
#include "stm32f10x.h"

void EXTI_Config(void) ;
void EXTI0_IRQHandler(void);
void EXTI4_IRQHandler(void);
void EXTI9_5_IRQHandler(void) ;
#endif