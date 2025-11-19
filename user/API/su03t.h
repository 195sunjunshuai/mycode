#ifndef __SU03T_H_ 
#define __SU03T_H_ 
 
#include "stm32f10x.h" 
 
#define SU03T_R_Buff_Length  60  
#define SU03T_S_Buff_Length  13  
 
typedef struct 
{ 
 uint8_t R_Buff[SU03T_R_Buff_Length];  //接收缓冲区 
 uint8_t S_Buff[SU03T_S_Buff_Length];  //发送缓冲区 
 uint8_t R_Cnt;    //接收到的数量 
 uint8_t R_Idle;   //空闲标志位 
}SU03T; 


// 串口发送消息最大长度
#define UART_SEND_MAX      32
#define UART_MSG_HEAD_LEN  2
#define UART_MSG_FOOT_LEN  2

// 串口发送消息号
#define U_MSG_PlayVoc      1
#define U_MSG_PlayCh20      2
#define U_MSG_PlayCo2      3
#define U_MSG_PlayTime      4
#define U_MSG_PlayWendu      5
#define U_MSG_PlayShidu      6
#define U_MSG_PlayLightInten      7
#define U_MSG_PlayMachine      8
#define U_MSG_PlayMachine1      9
#define U_MSG_PlayMachine2      10
#define U_MSG_PlayMachine3      11

// 串口消息参数类型
typedef union {
  double d_double;
  int d_int;
  unsigned char d_ucs[8];
  char d_char;
  unsigned char d_uchar;
  unsigned long d_long;
  short d_short;
  float d_float;
}uart_param_t;

// action: PlayVoc
void _uart_PlayVoc(double VOC);

// action: PlayCh20
void _uart_PlayCh20(double CH20);

// action: PlayCo2
void _uart_PlayCo2(int CO2);

// action: PlayTime
void _uart_PlayTime(char Hour, char Min);

// action: PlayWendu
void _uart_PlayWendu(double Wendu);

// action: PlayShidu
void _uart_PlayShidu(double Shidu);

// action: PlayLightInten
void _uart_PlayLightInten(double LUX);

// action: PlayMachine
void _uart_PlayMachine(int Which_Machine);

// action: PlayMachine1
void _uart_PlayMachine1();

// action: PlayMachine2  最高档啦
void _uart_PlayMachine2();

// action: PlayMachine3  最低档啦
void _uart_PlayMachine3();

void SU03T_Config(void) ;
void SU03T_Handle(void) ;
#endif 