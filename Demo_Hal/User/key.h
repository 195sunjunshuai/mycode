#ifndef __KEY_H_
#define __KEY_H_

//#include "stm32f10x.h"
#include "main.h"
#define  KEY_SCAN_MODE  1  //0 ×èÈûÉ¨Ãè  1 ·Ç×èÈûÉ¨Ãè
extern uint16_t KEY_Period[2]; 
void KEY_Config(void);
uint8_t KEY_Scan(void);
void Key_Handle(void);

#endif