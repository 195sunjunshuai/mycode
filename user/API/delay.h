#ifndef __DELAY_H_
#define __DELAY_H_
#include "stm32f10x.h"

extern uint16_t LED_Period[2];
extern uint8_t KEY_Period[2];
extern uint16_t RGB_R_Period[2]; 
extern uint16_t RGB_G_Period[2]; 
extern uint16_t RGB_B_Period[2];
extern uint16_t DHT_Period[2];
extern uint16_t RGB_B_Breath[2];
extern uint16_t two_Period[2];
extern uint16_t PUBLISH_Period[2];
extern uint16_t PING_Period[2];
extern uint32_t onemin[2];
void Delay_1us();
void Delay_us(uint32_t time);
void Delay_ms(uint32_t time);
void Delay_systick_ms(uint32_t time);
void SysTick_Handler(void);
#endif