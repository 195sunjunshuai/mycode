#ifndef __SOFT_IIC_H_ 
#define __SOFT_IIC_H_ 
 
#include "stm32f10x.h" 
 
void SOFT_IIC_Config(void); 
void SOFT_IIC_START(void); 
void SOFT_IIC_STOP(void); 
void SOFT_IIC_SendByte(uint8_t Byte); 
uint8_t SOFT_IIC_RecByte(void); 
void SOFT_IIC_SendACK(uint8_t ack); 
uint8_t SOFT_IIC_RecACK(void); 
 
#endif 