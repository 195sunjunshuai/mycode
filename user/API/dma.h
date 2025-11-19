#ifndef __DMA_H_ 
#define __DMA_H_ 
 
#include "stm32f10x.h"
#define ADC1_DR_Address    ((uint32_t)0x4001244C);

void ADC_DMA_Config();
void ADC_DMA_Handle();
#endif