 #ifndef __UART1_H_
#define __UART1_H_
#include "main.h"

void UART1_SendByte(uint8_t Byte);
void UART1_SendBuff(uint8_t *Buff , uint16_t Length) ;
void UART1_SendStr(uint8_t *Str);

extern uint8_t U1_Rec_Buff[100];    //接收缓冲区 
extern uint8_t U1_Rec_Cnt;          //已经接收到的数量 
extern uint8_t U1_Rec_Idle;         //空闲标志位 
extern uint8_t U1_Rec_Single;       //存放接收的单个数据 
extern uint8_t U2_Rec_Single;       //存放接收的单个数据 
#endif