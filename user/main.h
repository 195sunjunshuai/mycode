#ifndef __MAIN_H_
#define __MAIN_H_
#include "stm32f10x.h" 
#include "led.h"
#include "key1.h"
#include "ele_machine.h"
#include "relay.h"
#include "delay.h"
#include "main.h"
#include "exti.h" 
#include "tim6.h"
#include "tim7.h"
#include "uart1.h"
#include "stdio.h"
#include "string.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h" 
#include "event_groups.h"
#define BIT_0 ( 1 << 0 ) 
#define BIT_4 ( 1 << 4 ) 
extern SemaphoreHandle_t xSemaphore;
extern SemaphoreHandle_t xSemaphore_Count; 
extern SemaphoreHandle_t xSemaphore_Mutex ;          //创建互斥信号量句柄 
extern SemaphoreHandle_t xSemaphore_Mutex_Printf ;   //创建printf互斥信号量句柄
extern QueueHandle_t   xQueue;     //创建消息队列的句柄 
extern QueueHandle_t   xQueue_AP; 
extern QueueHandle_t   xQueue_Update; 
extern EventGroupHandle_t xCreatedEventGroup; 

#define sss 1
void Creat_Task();
void vTaskLED1( void * pvParameters );
void vTaskRELAY( void * pvParameters );
void vTaskKEY( void * pvParameters );
void vTaskTime(void * pvParameters);
void vTaskSU03T( void * pvParameters );
void vTaskKQM( void * pvParameters );
void vTaskSMOKE(void * pvParameters);
void vTaskBH1750(void * pvParameters);
void vTaskESP( void * pvParameters );
void vTaskDHT11(void * pvParameters);
void vTaskDelete( TaskHandle_t xTask );
void vCallbackFunction_Time1( TimerHandle_t xTimer ); 
void vCallbackFunction_Time2( TimerHandle_t xTimer );
void vCallbackFunction_Time( TimerHandle_t xTimer );
void vLvglTaskFunction( void * pvParameters );
void vTaskUPDATE ( void * pvParameters );
void vTaskIWDG ( void * pvParameters );
void vApplicationTickHook (void);

extern BaseType_t xReturned;
extern TaskHandle_t xHandle_LED1 ; 
extern TaskHandle_t xHandle_RELAY ;
extern TaskHandle_t xHandle_KEY ;
#define USE_STD_LIB 1 //0 寄存器编程  1  库函数编程
#define SSS 66    //0.LED灯，RGB灯 1.串口1 2.kqm 3.串口发送指令作业 4.电机  6.TIM6  
                  //7.TIM7  8.su03t 9.DHT11 10.PWM 11.bh1750 12 spi/W25Q64 
									//13.ADC  14.火焰 15.屏幕 16ESP 17.MQTT 66.空

#endif