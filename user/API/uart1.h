#ifndef __UART1_H_
#define __UART1_H_
#include "stm32f10x.h"
void UART1_Config();
void UART1_SendByte(uint8_t Byte);
void UART1_SendArray(uint8_t *Buffer,uint16_t len);
void UART1_SendStr(uint8_t *Str);
uint8_t UART1_RecByte(void) ;

#define KQM_R_Buff_Length 8

typedef struct 
{
	uint8_t R_BUFF[8];
	uint8_t R_Cnt;
	volatile uint8_t R_Idle;
}ABC;
void abc_Clear_R_Buff(void) ; 
void Time_Handle(void);
int safe_printf(const char *format, ...);
#endif