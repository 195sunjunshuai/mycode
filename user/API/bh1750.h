#ifndef __BH1750_H_ 
#define __BH1750_H_ 
 
#include "stm32f10x.h" 
 
#define BH1750_IIC_TYPE   0//0  模拟IIC   1  硬件IIC 
 
#define BH1750_ADRR  0x23   //7位的器件地址 
//7位的器件地址+读写位 
#define BH1750_W     ((BH1750_ADRR<<1)+0)   //写 
#define BH1750_R     ((BH1750_ADRR<<1)+1)   //读 
 
void BH1750_Config(void); 
void BH1750_SendCmd(uint8_t Cmd); 
float BH1750_Handle(void); 
extern double LightInten;
#endif