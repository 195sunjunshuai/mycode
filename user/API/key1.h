#ifndef __KEY1_H_ 
#define __KEY1_H_ 
 
#include "stm32f10x.h" 
#define  KEY_SCAN_MODE  1  //0 ×èÈûÉ¨Ãè  1 ·Ç×èÈûÉ¨Ãè 
void KEY_Config(void);
uint8_t KEY_Scan();
void Key_Handle();
//extern  uint8_t msg[6];
//extern  uint8_t buf[]; 
//extern  const uint16_t len;
#endif 