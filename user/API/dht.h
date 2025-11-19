#ifndef __DHT_H_ 
#define __DHT_H_ 
#include "stm32f10x.h" 
void DHT_Config(void); 
uint8_t DHT_Handle(void);
typedef struct
{
  double Tem;
	uint16_t Hum;
}DHT;
extern DHT Dht;
#endif