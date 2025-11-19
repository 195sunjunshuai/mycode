#ifndef __MQTT_H_ 
#define __MQTT_H_ 
 
#include "stm32f10x.h" 

//#define ClientID   "68c9037232771f177b60a9d7_Device_sjs_0_0_2025091606"
//#define Username   "68c9037232771f177b60a9d7_Device_sjs"
//#define Password   "635c5c5980f545b4568fb648934c7a67518c768b5992a56f6bece15b795e90b0"
typedef struct
{
	char ClientID[100];
	char Username[100];
	char Password[100];
}HW;
extern HW hw;
#define Publish_Topic   "$oc/devices/68c9037232771f177b60a9d7_Device_sjs/sys/properties/report"
#define Subscribe_Topic "$oc/devices/68c9037232771f177b60a9d7_Device_sjs/sys/commands/#" 

void MQTT_Connect(void);
void MQTT_Publish(void);
void MQTT_Subscribe(void);
void MQTT_Ping(void);

#endif