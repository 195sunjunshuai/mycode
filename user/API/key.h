#ifndef __KEY_H_
#define __KEY_H_

#include "stm32f10x.h"

#define  KEY_SCAN_MODE  1  //0 ื่ศ๛ษจร่  1 ทวื่ศ๛ษจร่

void KEY_Config(void);
uint8_t KEY_Scan(void);
void Key_Handle(void);

#endif
