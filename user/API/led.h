#ifndef  __LED_H_
#define  __LED_H_
#include "stm32f10x.h" 
void LED_Config();
void LED1_ON();
void LED2_ON();
void LED3_ON();
void LED4_ON();
void LED1_OFF();
void LED2_OFF();
void LED3_OFF();
void LED4_OFF();
void LED1_TOGGLE();
void LED2_TOGGLE();
void LED3_TOGGLE();
void LED4_TOGGLE();
void RGB_Config();
void RGB_R_ON();
void RGB_R_OFF();
void RGB_G_ON();
void RGB_G_OFF();
void RGB_B_ON();
void RGB_B_OFF();
void RGB_B_Show();
extern uint8_t LED1_FLAG;
#endif