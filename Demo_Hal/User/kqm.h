#ifndef __KQM_H_ 
#define __KQM_H_ 
 
#include "main.h" 
#define KQM_R_Buff_Length  8  
typedef struct 
{
	uint8_t R_BUFF[8];
	uint8_t R_Cnt;
	uint8_t R_Idle;
}KQM;
extern KQM kqm;
typedef struct 
{ 
 float VOC; 
 float CH2O; 
 uint16_t CO2; 
}SENSOR; 
extern SENSOR Sensor; 
void KQM_Config(void); 
void KQM_Clear_R_Buff(void); 
void KQM_Handle(void);
#endif