#ifndef __ESP_H_
#define __ESP_H_
#include "stm32f10x.h"
#define ESP_R_Buff_Length  500
#define ESP_S_Buff_Length  350
#define WIFI_W25Q64_ADDR   0x344000
#define WIFI_huaweiyun_ADDR   0x345000

typedef struct
{
	uint8_t R_Buff[ESP_R_Buff_Length];
	uint8_t S_Buff[ESP_S_Buff_Length];	
	uint16_t R_Cnt;
	uint8_t R_Idle;
}ESP;
typedef struct
{
	uint8_t Name[100];
	uint8_t Pwd[100];
	uint8_t Cs;
}WIFI;
extern WIFI Wifi;
extern ESP esp;
extern uint8_t weather_code;
extern uint8_t weather_high;
extern uint8_t weather_low;
extern uint8_t weather_nb_code;
extern uint8_t weather_nb_high;
extern uint8_t weather_nb_low;
extern uint8_t NET_State;   //设备的网络状态  0 设备正在连接服务器  1 设备已经连接服务器建立透传
extern uint8_t NET_Step;   //设备联网的步骤
void ESP_Config(void);
void ESP_SendStr(uint8_t *Str);
void ESP_SendByte(uint8_t Byte);
void ESP_SendArray(uint8_t *Buffer,uint16_t len);
void ESP_Clear_R_Buff(void);
uint8_t ESP_SendCmd_CheckRes(uint8_t *cmd,uint8_t *rec_buff,uint8_t *respond,uint16_t time_out,uint8_t flag);
void ESP_Connect_NET(void);
void ESP_Connect_TIME(void);
void ESP_Connect_WEATHER(void);
void ESP_Connect_AP(void) ;
void ESP_Connect_HW(void) ;
#endif
