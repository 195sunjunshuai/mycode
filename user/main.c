#include "main.h"
#include "stm32f10x.h"
#include "led.h"
#include "key.h"
#include "relay.h"
#include "delay.h"
#include "exti.h"
#include "tim6.h"
#include "tim7.h"
#include "uart1.h"
#include "stdio.h"
#include "string.h"
#include "kqm.h"
#include "su03t.h"
#include "dht.h"
#include "pwm.h"
#include "bh1750.h"
#include "spi.h"
#include "w25q64.h"
#include "adc.h"
#include "dma.h"
#include "rtc.h"
#include "time.h"
#include "lcd.h"
#include "esp.h"
#include "mqtt.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "event_groups.h"
#include "timers.h"
#include "lvgl.h"    //½Ó¿ÚµÄÊÊÅä  18
#include "lv_port_disp.h"    //½Ó¿ÚµÄÊÊÅä  19
#include "lvgl_app.h"
#include "Fontdownload.h"
#include "gui_guider.h" 

#include "lvgl.h"
#include <stdio.h>
#include "gui_guider.h"
#include "events_init.h"
#include "w25q_lvgl_img.h"
#include "iwdg.h"

void Delay(void);
//ÈÎÎñµÄ¾ä±ú(Ö¸ÏòÄ³¸öÄÚ´æÇøÓòµÄÖ¸Õë),Ïàµ±ÓÚÈÎÎñµÄÉí·İÖ¤,Ö¸ÏòÈÎÎñÓÅÏÈ¼¶¡¢Èë¿Úº¯ÊıµÈĞÅÏ¢¡£
//ºóĞøµÄºÜ¶à²Ù×÷¶¼ÊÇÒÔ¾ä±ú×÷Îª²Ù×÷¶ÔÏó
TaskHandle_t xHandle_LED1 = NULL; 
TaskHandle_t xHandle_RELAY = NULL; 
TaskHandle_t xHandle_KEY = NULL; 
TaskHandle_t xHandle_KQM = NULL;
TaskHandle_t xHandle_SMOKE = NULL;
TaskHandle_t xHandle_SU03T = NULL; 
TaskHandle_t xHandle_BH1750 = NULL;
TaskHandle_t xHandle_DHT11 = NULL;
TaskHandle_t xHandle_ESP = NULL;
TaskHandle_t xHandle_ESP_TIME = NULL;
TaskHandle_t xLvglTaskHandle = NULL;  //½Ó¿ÚµÄÊÊÅä  25
TaskHandle_t xHandle_UPDATE = NULL;
TaskHandle_t xHandle_IWDG = NULL;

//¾ä±ú,¾ä±ú¾ÍÊÇÀàËÆÓÚÉí·İÖ¤
SemaphoreHandle_t xSemaphore = NULL;    //´´½¨¶şÖµĞÅºÅÁ¿¾ä±ú
SemaphoreHandle_t xSemaphore_Count = NULL;    //´´½¨¼ÆÊıĞÅºÅÁ¿¾ä±ú
SemaphoreHandle_t xSemaphore_Mutex = NULL;    //´´½¨»¥³âĞÅºÅÁ¿¾ä±ú  
SemaphoreHandle_t xSemaphore_Mutex_Printf = NULL;    //´´½¨printf»¥³âĞÅºÅÁ¿¾ä±ú  

QueueHandle_t		xQueue=NULL;     //´´½¨ÏûÏ¢¶ÓÁĞµÄ¾ä±ú
QueueHandle_t   xQueue_AP=NULL;     //´´½¨APÄ£Ê½ÏûÏ¢¶ÓÁĞµÄ¾ä±ú
QueueHandle_t   xQueue_Update=NULL;     //´´½¨¸üĞÂÏûÏ¢¶ÓÁĞµÄ¾ä±ú

EventGroupHandle_t xCreatedEventGroup;     //´´½¨ÊÂ¼ş¾ä±ú

TimerHandle_t xTimer = NULL;   //Èí¼ş¶¨Ê±Æ÷¾ä±ú
TimerHandle_t xTimer1 = NULL;   //Èí¼ş¶¨Ê±Æ÷¾ä±ú
TimerHandle_t xTimer2 = NULL;   //Èí¼ş¶¨Ê±Æ÷¾ä±ú

uint8_t *temp=NULL;
uint8_t *temp1=NULL; 
#if (sss==0)
int main()
{

}
#elif (sss==1) 
int main(void)
{	
  BaseType_t xReturned;	
#if (USE_STD_LIB==0) //Ö»ÒªÊ¹ÓÃÖĞ¶Ï£¬±ØĞëµ÷ÓÃÖĞ¶Ï·Ö×é
	NVIC_SetPriorityGrouping(3);    //ÖĞ¶ÏÓÅÏÈ¼¶·Ö×é£¬¿ª»úµ÷ÓÃ1´Î
//core_cm3.h 1430ĞĞ    ·Ö×éÈ·¶¨Ö®ºó£¬ÇÀÕ¼ºÍ´Î¼¶ÓÅÏÈ¼¶ÒªÔÚÔÊĞí·¶Î§ÄÚ
#elif (USE_STD_LIB==1)	
//void NVIC_PriorityGroupConfig(uint32_t NVIC_PriorityGroup);	misc.h 196ĞĞ
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
#endif
	UART1_Config();	
	LED_Config();
  RELAY_Config();
	Machine_Config();
	KEY_Config();
	KQM_Config();
	SU03T_Config();
	ADC_Config();
	BH1750_Config();
	DHT_Config();
	TIM7_Config();
  SPI_Config();
	W25Q64_ReadID_0x90();
	PARA_Init();
	RTC_Config();
	lv_init();    //LVGLµÄ³õÊ¼»¯   //½Ó¿ÚµÄÊÊÅä  20
	lv_port_disp_init();    //LVGLÏÔÊ¾Éè±¸µÄ³õÊ¼»¯   //½Ó¿ÚµÄÊÊÅä  21
  ESP_Config();
	w25q_bg_decoder_register();
	IWDG_Init();
	
	//´´½¨ÈÎÎñµÄÊ±ºò£¬Õ»¿Õ¼ä·ÖÅä¶à´ó  https://www.freertos.org/zh-cn-cmn-s/Why-FreeRTOS/FAQs/Memory-usage-boot-times-context#how-big-should-the-stack-be 
  xReturned = xTaskCreate(
                  vTaskLED1,       //ÈÎÎñº¯ÊıµÄÈë¿ÚµØÖ·,ÈÎÎñ¶¼ÊÇ²»ÍË³öµÄÑ­»·
                  "vTaskLED1",     //ÈÎÎñµÄÃèÊöĞÔÃû³Æ,ÎŞÊµ¼ÊÒâ
									83,             //ÈÎÎñµÄ¶ÑÕ»¿Õ¼ä´óĞ¡  µ¥Î»:×Ö  1×Ö=4×Ö½Ú
                  NULL,            //ÈÎÎñ²ÎÊı
                  1,//ÈÎÎñµÄÓÅÏÈ¼¶,Êı×ÖÔ½´óÓÅÏÈ¼¶Ô½¸ß¡£ÓÅÏÈ¼¶Ä¬ÈÏÉÏÏŞÎª (configMAX_PRIORITIES - 1)
                  &xHandle_LED1 );      //ÈÎÎñµÄ¾ä±ú
	if(xReturned==pdPASS)
	{printf("LED1ÈÎÎñ´´½¨³É¹¦\r\n");
	}
  xReturned = xTaskCreate(vTaskRELAY,"vTaskLED2",80,NULL,2,&xHandle_RELAY ); 
	if(xReturned==pdPASS)
	{printf("RELAYÈÎÎñ´´½¨³É¹¦\r\n");
	}
  xReturned = xTaskCreate(vTaskKEY,"vTaskKEY",500,NULL,8,&xHandle_KEY ); 
	if(xReturned==pdPASS)
	{printf("KEYÈÎÎñ´´½¨³É¹¦\r\n");
	}		
  xReturned = xTaskCreate(vTaskKQM,"vTaskKQM",90,NULL,4,&xHandle_KQM ); 
	if(xReturned==pdPASS)
	{printf("KQMÈÎÎñ´´½¨³É¹¦\r\n");
	}	
  xReturned = xTaskCreate(vTaskSU03T,"vTaskSU03T",128,NULL,5,&xHandle_SU03T ); 
	if(xReturned==pdPASS)
	{printf("SU03TÈÎÎñ´´½¨³É¹¦\r\n");
	}
  xReturned = xTaskCreate(vTaskSMOKE,"vTaskSMOKE",88,NULL,6,&xHandle_SMOKE ); 
	if(xReturned==pdPASS)
	{printf("SMOKEÈÎÎñ´´½¨³É¹¦\r\n");
	}	
	xReturned = xTaskCreate(vTaskBH1750,"vTaskBH1750",88,NULL,3,&xHandle_BH1750 ); 
	if(xReturned==pdPASS)
	{printf("BH1750ÈÎÎñ´´½¨³É¹¦\r\n");
	}
	xReturned = xTaskCreate(vTaskESP,"vTaskESP",360,NULL,7,&xHandle_ESP ); 
	if(xReturned==pdPASS)
	{printf("ESPÈÎÎñ´´½¨³É¹¦\r\n");
	}
	xReturned = xTaskCreate(vTaskDHT11,"vTaskDHT11",88,NULL,9,&xHandle_DHT11 ); 
	if(xReturned==pdPASS)
	{printf("DHT11ÈÎÎñ´´½¨³É¹¦\r\n");
	}
	//½Ó¿ÚµÄÊÊÅä  28
	xReturned = xTaskCreate(vLvglTaskFunction, "lvgl_task", 512, NULL, 9, &xLvglTaskHandle);	
	if(xReturned==pdPASS)
	{printf("LVGLÈÎÎñ´´½¨³É¹¦\r\n");
	}
	xReturned = xTaskCreate(vTaskUPDATE, "vTaskUPDATE", 216, NULL, 9, &xHandle_UPDATE);	
	if(xReturned==pdPASS)
	{printf("¸üĞÂLVGLÈÎÎñ´´½¨³É¹¦\r\n");
	}
	xReturned = xTaskCreate(vTaskIWDG, "vTaskIWDG", 128, NULL, 1, &xHandle_IWDG);	
	if(xReturned==pdPASS)
	{printf("¸üĞÂLVGLÈÎÎñ´´½¨³É¹¦\r\n");
	}
	
	//·µ»ØÖµ  ¶şÖµĞÅºÅÁ¿¾ä±ú 
	xSemaphore = xSemaphoreCreateBinary();   //´´½¨¶şÖµĞÅºÅÁ¿
	if(xSemaphore!=NULL)
	{printf("KQM¶şÖµĞÅºÅÁ¿´´½¨³É¹¦\r\n");	
	}
	xSemaphoreGive(xSemaphore);   //ÊÍ·ÅĞÅºÅÁ¿,ÈÃ¸ßµÍÓÅÏÈ¼¶ÈÎÎñÉêÇë
	xSemaphore_Mutex = xSemaphoreCreateMutex();   //´´½¨»¥³âĞÅºÅÁ¿
	if(xSemaphore_Mutex!=NULL)
	{printf("»¥³âĞÅºÅÁ¿´´½¨³É¹¦\r\n");	
	}	
	xSemaphore_Mutex_Printf=xSemaphoreCreateMutex();   //´´½¨»¥³âĞÅºÅÁ¿
	if(xSemaphore_Mutex_Printf!=NULL)
	{safe_printf("printf»¥³âĞÅºÅÁ¿´´½¨³É¹¦\r\n");	
	}	
	//´´½¨ÏûÏ¢¶ÓÁĞ
	//²ÎÊı1    ¸Ã»º³åÇø×Ü¹²´æ·Å¶àÉÙ¸öÏûÏ¢
	//²ÎÊı2    Ã¿¸öÏûÏ¢µÄ´óĞ¡
	//·µ»ØÖµ   ÏûÏ¢¶ÓÁĞµÄ¾ä±ú   µ¥Î»:×Ö½Ú
  xQueue = xQueueCreate(1,8);
	if(xQueue != NULL)
	{printf("KQMÏûÏ¢¶ÓÁĞ´´½¨³É¹¦\r\n");		
	}	
  xQueue_AP = xQueueCreate(1,1); 
  if(xQueue_AP != NULL) 
  {printf("APÄ£Ê½ÏûÏ¢¶ÓÁĞ´´½¨³É¹¦\r\n");   
  }
  xQueue_Update=xQueueCreate(1,1);
  if(xQueue_Update != NULL) 
  {printf("¸üĞÂÏûÏ¢¶ÓÁĞ´´½¨³É¹¦\r\n");   
  } 	
	//´´½¨ÊÂ¼ş
	xCreatedEventGroup = xEventGroupCreate();
	if(xCreatedEventGroup!=NULL)
	{printf("SU03TÊÂ¼ş´´½¨³É¹¦\r\n");		
	}

	//²ÎÊı1  ¶¨Ê±Æ÷µÄ¿É¶ÁÎÄ±¾Ãû³Æ,Ã»ÓĞÉ¶Êµ¼ÊÒâÒå
	//²ÎÊı2  ¶¨Ê±Æ÷µÄÖÜÆÚ
	//²ÎÊı3  ¶¨Ê±Æ÷µÄÄ£Ê½  pdTRUE  ÖÜÆÚÄ£Ê½  pdFALSE  µ¥´ÎÄ£Ê½
	//²ÎÊı4  ·ÖÅä¸øÕıÔÚ´´½¨µÄ¶¨Ê±Æ÷µÄ±êÊ¶·û¡£Í¨³£Çé¿öÏÂ£¬µ±Í¬Ò»»Øµ÷º¯Êı·ÖÅä¸ø¶à¸ö¶¨Ê±Æ÷Ê±£¬¸Ãº¯Êı½«ÓÃÓÚ¶¨Ê±Æ÷»Øµ÷º¯Êı£¬ ÒÔÊ¶±ğÄÄ¸ö¶¨Ê±Æ÷¹ıÆÚ
	//²ÎÊı5  ¶¨Ê±Æ÷µÄ»Øµ÷º¯Êı
	//·µ»ØÖµ ¶¨Ê±Æ÷µÄ¾ä±ú
	xTimer1=xTimerCreate("xTimer1",1000*60*60,pdTRUE,( void * ) 1,vCallbackFunction_Time1);
	if(xTimer1!=NULL)
	{printf("Èí¼ş¶¨Ê±Æ÷1´´½¨³É¹¦\r\n");		
		//²ÎÊı1  Èí¼ş¶¨Ê±Æ÷¾ä±ú
		//²ÎÊı2  ×èÈûÊ±¼ä  ÔÚÒÔÏÂÇé¿öÏÂ£¬xBlockTime½«±»ºöÂÔ£ºxTimerStart()ÔÚ RTOS µ÷¶ÈÆ÷Æô¶¯Ö®Ç°¾Í±»µ÷ÓÃ¡£
		if(xTimerStart(xTimer1,0)==pdPASS)     //Æô¶¯Èí¼ş¶¨Ê±Æ÷
		{printf("Èí¼ş¶¨Ê±Æ÷1Æô¶¯³É¹¦\r\n");				
		}
	}
//	xTimer2=xTimerCreate("xTimer2",3000,pdFALSE,( void * ) 2,vCallbackFunction_Time2);	
//	if(xTimer2!=NULL)
//	{printf("Èí¼ş¶¨Ê±Æ÷2´´½¨³É¹¦\r\n");		
//		if(xTimerStart(xTimer2,0)==pdPASS)     //Æô¶¯Èí¼ş¶¨Ê±Æ÷
//		{
//			printf("Èí¼ş¶¨Ê±Æ÷2Æô¶¯³É¹¦\r\n");				
//		}	
//	}
	xTimer=xTimerCreate("xTimer",1000,pdTRUE,( void * ) 1,vCallbackFunction_Time);
	if(xTimer!=NULL)
	{printf("Èí¼ş¶¨Ê±Æ÷1´´½¨³É¹¦\r\n");		
		//²ÎÊı1  Èí¼ş¶¨Ê±Æ÷¾ä±ú
		//²ÎÊı2  ×èÈûÊ±¼ä  ÔÚÒÔÏÂÇé¿öÏÂ£¬xBlockTime½«±»ºöÂÔ£ºxTimerStart()ÔÚ RTOS µ÷¶ÈÆ÷Æô¶¯Ö®Ç°¾Í±»µ÷ÓÃ¡£
		if(xTimerStart(xTimer,0)==pdPASS)     //Æô¶¯Èí¼ş¶¨Ê±Æ÷
		{printf("Èí¼ş¶¨Ê±Æ÷Æô¶¯³É¹¦\r\n");				
		}
	}
	vTaskStartScheduler();  //´´½¨Íê³ÉÈÎÎñÖ®ºó,Æô¶¯µ÷¶ÈÆ÷,Æô¶¯µ÷¶ÈÆ÷Ö®ºóµÄ´úÂë¶¼²»»áÖ´ĞĞ
	while (1)
	{printf("´ó¼Ò¿´¿´ÊÇ·ñ¿ÉÒÔÖ´ĞĞµ½Õâ¾ä»°\r\n");
	}
}
 
void vTaskLED1( void * pvParameters )
{ //LEDµÆ
	
	for( ;; )
	{
					
		LED1_TOGGLE();
    if(LightInten<10)
		{
			LED3_ON();
		}
		else{
			LED3_OFF();
		}
		vTaskDelay(1000);  //ÏµÍ³µÄÑÓÊ±,»áÊÍ·ÅCPU  ²ÎÊı:ÒªÑÓÊ±µÄ½ÚÅÄÊı 
	}
}

void vTaskRELAY( void * pvParameters )
{ //¼ÌµçÆ÷
					  uint32_t freeheap=0;
	TaskStatus_t taskinfo;//ÈÎÎñ×´Ì¬
	for( ;; )
	{
//		//#define configTOTAL_HEAP_SIZE ( ( size_t ) ( 17 * 1024 ) ) //ÏµÍ³ËùÓĞ×ÜµÄ¶Ñ´óĞ¡  64K¿Õ¼ä·ÖÅä
//		freeheap=xPortGetFreeHeapSize();//»ñÈ¡ÏµÍ³Ê£Óà¶ÑÕ»×Ü¿Õ¼ä´óĞ¡
//		printf("Õû¸öÏµÍ³Ê£Óà¶ÑÕ»¿Õ¼ä´óĞ¡£º%d×Ö½Ú\r\n",freeheap);
////		//»ñÈ¡Ä³Ò»ÈÎÎñµÄ×´Ì¬ĞÅÏ¢
////		//ÔÚFreeRTOSConfig.hÎÄ¼şÖĞºêÖÃ1 #define configUSE_TRACE_FACILITY 1 //ÆôÓÃ¿ÉÊÓ»¯¸ú×Ùµ÷ÊÔ
//		vTaskGetInfo(xHandle_LED1,&taskinfo,pdTRUE,eInvalid);//ÈÎÎñ¾ä±ú »ñÈ¡µ½µÄÈÎÎñ×´Ì¬ĞÅÏ¢ »ñÈ¡µ½µÄ¶ÑÕ»Ê£Óà¿Õ¼ä 
//		printf("RELAYTask_HandleÊ£Óà¶ÑÕ»¿Õ¼ä´óĞ¡£º%d×Ö\r\n",taskinfo.usStackHighWaterMark);
//		RELAY_TOGGLE();
		vTaskDelay(1500);		
	}
}

void vTaskKEY( void * pvParameters )
{ //°´¼ü
  TickType_t xLastWakeTime;
  const TickType_t xFrequency = 10;  //ÏëÒªµÄ¾ø¶ÔÑÓÊ±ÖÜÆÚ£¬µ¥Î»£ºÏµÍ³½ÚÅÄ
  xLastWakeTime = xTaskGetTickCount();	 //»ñÈ¡µ±Ç°ÏµÍ³Ê±¼ä
	for( ;; )
	{
		Key_Handle();
		vTaskDelayUntil( &xLastWakeTime, xFrequency );	
	}
}

void vTaskKQM( void * pvParameters )
{ //KQM
//  BaseType_t xReturn_Value=0; 
//	while(1)
//	{
//	 xReturn_Value =xSemaphoreTake(xSemaphore,2000);  //Ò»Ö±µÈ,Ö±µ½µÈµ½ĞÅºÅÁ¿,´òÓ¡³É¹¦ 
//	 if(xReturn_Value==pdTRUE) 
//	 { 
//		 KQM_Handle(); 
//	 }
//  }
	uint8_t Buff[8]={0};
	uint8_t data_cs;
	BaseType_t xReturn_Value;	
	for( ;; )
	{
		xReturn_Value=xQueueReceive(xQueue,Buff,portMAX_DELAY);
		if(xReturn_Value==pdTRUE)
		{
			if(Buff[0]==0x5F)
			{//µØÖ·ÂëÕıÈ·
				data_cs=Buff[0]+Buff[1]+Buff[2]+Buff[3]+Buff[4]+Buff[5]+Buff[6];
				if(data_cs==Buff[7])
				{//Ğ£ÑéÍ¨¹ı
					Sensor.VOC = ((Buff[1]<<8)+Buff[2])*0.1;
	//				Sensor.VOC = (Buff[1]*0x100+Buff[2])*0.1;
					Sensor.CH2O = ((Buff[3]<<8)+Buff[4])*0.01;
					Sensor.CO2 = (Buff[5]<<8)+Buff[6];
//					printf("VOC=%.1f,CH2O=%.2f,CO2=%d\r\n",Sensor.VOC,Sensor.CH2O,Sensor.CO2);
				}
				else
				{//Ğ£Ñé²»Í¨¹ı
					printf("Ğ£Ñé²»Í¨¹ı\r\n");
				}
			}
			else
			{//µØÖ·Âë²»ÕıÈ·
				printf("µØÖ·Âë²»ÕıÈ·\r\n");
			}
			KQM_Clear_R_Buff();	
		}
		else
		{
			printf("KQM6600ÏûÏ¢¶ÓÁĞ»ñÈ¡Ê§°Ü\r\n");
		}	
	}
}

void vTaskSU03T( void * pvParameters )
{ //SU03T
	EventBits_t uxBits;	
	for( ;; )
	{
		//²ÎÊı1  ÊÂ¼ş¾ä±ú
		//²ÎÊı2  ¹ØÏµµÄÎ» 
		//²ÎÊı3  ÍË³öµÄÊ±ºò,ÊÇ·ñÇå³ı¶ÔÓ¦µÄÎ»  pdTRUE   pdFALSE  ²»Çå³ıĞèÒªµ÷ÓÃº¯ÊıxEventGroupClearBitsÊÖ¶¯Çå³ı
		//²ÎÊı4  ÊÇ·ñĞèÒª¹ØĞÄµÄÎ»¶¼Âú×ã   pdTRUE   pdFALSE
		//²ÎÊı5  µÈ´ıµÄÊ±¼ä £¨ÒÔµÎ´ğÎªµ¥Î»£©
						//²ÎÊı2ÓĞ3ÖÖĞ´·¨   1.0  ²»µÈ´ı   2.·Ç0ÕûÊı   µÈ¹Ì¶¨Ê±¼ä   3.portMAX_DELAY  Ò»Ö±µÈ,Ö±µ½µÈµ½ĞÅºÅÁ¿
		uxBits=xEventGroupWaitBits(xCreatedEventGroup,BIT_0,pdTRUE,pdTRUE,portMAX_DELAY);
		if(uxBits==BIT_0)
		{
			SU03T_Handle();
		}
		else 
		{
			printf("SU03TÊÂ¼şÃ»·¢Éú\r\n");
		}		
	}
}

void vTaskSMOKE( void * pvParameters )
{ //ÑÌÎí
	while(1)
	{
		ADC_Handle();
		vTaskDelay(2000);
	}
}
void vTaskBH1750( void * pvParameters )
{ //BH1750
	while(1)
	{
		BH1750_Handle();
		vTaskDelay(2000);
	}
}
void vTaskDHT11( void * pvParameters )
{ //DHT11
	while(1)
	{
		taskENTER_CRITICAL(); 
		DHT_Handle(); 
		taskEXIT_CRITICAL(); 
		vTaskDelay(2000); 
	}
}
void vTaskESP( void * pvParameters )
{	//ESP
	BaseType_t xReturned;
	uint8_t Message;
   while(1)
	 { 
		xReturned=xQueueReceive(xQueue_AP,&Message,0); 
   if(xReturned== pdTRUE) 
   { 
    NET_State=Message;    //NET_StateÇĞ»»³É6 
    NET_Step=0; 
//   printf("½ÓÊÕAPÏûÏ¢³É¹¦,×¼±¸ÇĞ»»³ÉAPÄ£Ê½\r\n"); 
    safe_printf("½ÓÊÕAPÏûÏ¢³É¹¦,×¼±¸ÇĞ»»³ÉAPÄ£Ê½\r\n"); 
   } 

		if(NET_State==0)
		{
			ESP_Connect_TIME();
		}
		else if(NET_State==1)
		{
			ESP_Connect_WEATHER();
		}
		else if(NET_State==2)
		{
			ESP_Connect_NET();
			
		}
		else if(NET_State==3)
		{
			MQTT_Connect();
		}
		else if(NET_State==4)
		{
			MQTT_Subscribe();
		}
		else if(NET_State==5)
		{
			if(PUBLISH_Period[0]>=PUBLISH_Period[1])
			{
				PUBLISH_Period[0]=0;
				MQTT_Publish();
			}
			else if(PING_Period[0]>=PING_Period[1])
			{
				PING_Period[0]=0;
				MQTT_Ping();				
			}
		}
		else if(NET_State==6)	
		{ 
     ESP_Connect_HW(); 
			printf("%d\r\n",NET_State);
    } 
		if(esp.R_Idle==1&&NET_State==5)
		{//ÍøÂç×´Ì¬µÈÓÚ3£¬²¢ÊÕµ½ÏÂĞĞÊı¾İ
			esp.R_Idle=0;
			if(esp.R_Buff[0]==0x30)
			{
//{"paras":{"relay_switch":"1"},"service_id":"Device_205","command_name":"led_switch"}
				printf("ÊÕµ½Æ½Ì¨µÄÏÂ·¢ÉèÖÃ±¨ÎÄ\r\n");
//temp=(uint8_t *)strstr((char *)Esp.R_Buff,"led_switch"); ²»ÄÜÕâÑù²éÕÒ£¬ÒòÎªÇ°ÃæÓĞ0
				//Èç¹ûÓĞ¶à¸ö¿ª¹Ø£¬¾ÍĞ´¶à¸öÕâÑù²éÕÒºÍ½âÎö
				temp=(uint8_t *)strstr((char *)&esp.R_Buff[4],"led_switch");
				temp1=(uint8_t *)strstr((char *)&esp.R_Buff[4],"relay_switch");
				if(temp!=NULL)
				{//²éÕÒµ½ÁË£¬·µ»Øled_switchÖĞµÄlËùÔÚµÄµØÖ·
					temp+=13; //Æ«ÒÆµ½µÆ×´Ì¬0/1ËùÔÚÎ»ÖÃ
					if(*temp=='1')
					{//¿ªµÆ
						LED2_ON();
					}
					else if(*temp=='0')
					{//¹ØµÆ
						LED2_OFF();						
					}
				}
					if(temp1!=NULL)
				{//²éÕÒµ½ÁË£¬·µ»Øled_switchÖĞµÄlËùÔÚµÄµØÖ·
					temp1+=15; //Æ«ÒÆµ½µÆ×´Ì¬0/1ËùÔÚÎ»ÖÃ
					if(*temp1=='1')
					{//¿ªµÆ
						RELAY_ON();
					}
					else if(*temp1=='0')
					{//¹ØµÆ
						RELAY_OFF();						
					}
				}
			}
			if(esp.R_Buff[0]==0xD0)
			{
				printf("ÊÕµ½Æ½Ì¨µÄĞÄÌø»Ø¸´\r\n");				
			}
			ESP_Clear_R_Buff();  //Çå³ıÒ»ÏÂ½ÓÊÕ»º³åÇø£¬·½±ãÖØÍ·½ÓÊÕ
		}
		vTaskDelay(2000);
	 }		
}
void vLvglTaskFunction( void * pvParameters )
{ //½Ó¿ÚµÄÊÊÅä  26
	TickType_t xLastWakeTime;
	const TickType_t xPeriod = pdMS_TO_TICKS( 5 );
	xLastWakeTime = xTaskGetTickCount(); 
//  setup_scr_screen(&guider_ui);    //´´½¨ÆÁÄ»1ºÍÆÁÄ»ÖĞµÄ¶ÔÏó 
  setup_scr_screen_1(&guider_ui);    //´´½¨ÆÁÄ»2ºÍÆÁÄ»ÖĞµÄ¶ÔÏó 
  lv_scr_load(guider_ui.screen_1);    //¼ÓÔØÆÁÄ»1  	
//	xTimerStart(xTimer,0);
//	Lvgl_Demo11();
	for(;;)
	{		
		lv_task_handler();
		vTaskDelayUntil( &xLastWakeTime, xPeriod );//¾ø¶ÔÑÓÊ±
	}
}


void vTaskUPDATE ( void * pvParameters )
{ //¸üĞÂÌìÆø
	uint8_t Message; 
	for( ; ; )
	{
		if(xQueueReceive(xQueue_Update,&Message,portMAX_DELAY)==pdPASS)
		{
			if(Message==1|Message==2)
			{
				ESP_Connect_WEATHER();
				NET_Step--;
				if(Message==2)
				{
					NET_Step++;
				}
				lv_label_set_text_fmt(guider_ui.screen_1_label_21,"%d",weather_low);
      	lv_label_set_text_fmt(guider_ui.screen_1_label_24,"%d",weather_high);
	      lv_label_set_text_fmt(guider_ui.screen_1_label_4_25,"%d",weather_nb_low);
	      lv_label_set_text_fmt(guider_ui.screen_1_label_1_25,"%d",weather_nb_high);
				if(weather_code>=0 && weather_code<=3)
				{
			//		lv_label_set_text(guider_ui.screen_1_tianqi, "æ™?");
					lv_img_set_src(guider_ui.screen_1_rain,&_sun_alpha_35x35);
				}
				else if(weather_code>=4 && weather_code<=9)
				{
			//		lv_label_set_text(guider_ui.screen_1_tianqi, "å¤šäº‘ ");
			//		lv_img_set_src(guider_ui.screen_1_img_13,&_213242C09CA4EE3A5E54359E297D2991_alpha_102x100);
					lv_img_set_src(guider_ui.screen_1_rain,&_yun_alpha_35x35);
				}
				else if(weather_code>=10 && weather_code<=20)
				{  
			//	  lv_label_set_text(guider_ui.screen_1_tianqi, "é›?");
			//		lv_img_set_src(guider_ui.screen_1_rain,&_rainsmall_alpha_35x35);
					lv_img_set_src(guider_ui.screen_1_rain,&_rainsmall_alpha_35x35);
				}
				if(weather_nb_code>=0 && weather_nb_code<=3)
				{
			//		lv_label_set_text(guider_ui.screen_1_tianqi, "æ™?");
					lv_img_set_src(guider_ui.screen_1_img_7_15,&_sun_alpha_35x35);
				}
				else if(weather_nb_code>=4 && weather_nb_code<=9)
				{
			//		lv_label_set_text(guider_ui.screen_1_tianqi, "å¤šäº‘ ");
			//		lv_img_set_src(guider_ui.screen_1_img_13,&_213242C09CA4EE3A5E54359E297D2991_alpha_102x100);
					lv_img_set_src(guider_ui.screen_1_img_7_15,&_yun_alpha_35x35);
				}
				else if(weather_nb_code>=10 && weather_nb_code<=20)
				{  
			//	  lv_label_set_text(guider_ui.screen_1_tianqi, "é›?");
			//		lv_img_set_src(guider_ui.screen_1_rain,&_rainsmall_alpha_35x35);
					lv_img_set_src(guider_ui.screen_1_img_7_15,&_rainsmall_alpha_35x35);
				}
			}
		}
  }	
}

//Èí¼ş¶¨Ê±Æ÷»Øµ÷º¯Êı£¬Ïàµ±ÓÚÖ®Ç°¶¨Ê±Æ÷µÄÖĞ¶Ï·şÎñº¯Êı  ²»ÊÇ²»ÍË³öµÄÑ­»·
void vCallbackFunction_Time1( TimerHandle_t xTimer )
{ //ÈÕ¸üÌìÆøÏûÏ¢
	uint8_t update_Message=1;
	xQueueSend(xQueue_Update,&update_Message,0);
}
void vTaskIWDG( void * pvParameters )
{ //Î¹¹·
	for( ;; )
	{			
		IWDG_Feed();
		vTaskDelay(2000);  //ÏµÍ³µÄÑÓÊ±,»áÊÍ·ÅCPU  ²ÎÊı:ÒªÑÓÊ±µÄ½ÚÅÄÊı 
	}
}

//Èí¼ş¶¨Ê±Æ÷»Øµ÷º¯Êı£¬Ïàµ±ÓÚÖ®Ç°¶¨Ê±Æ÷µÄÖĞ¶Ï·şÎñº¯Êı  ²»ÊÇ²»ÍË³öµÄÑ­»·
void vCallbackFunction_Time2( TimerHandle_t xTimer )
{printf("Èí¼ş¶¨Ê±Æ÷2µÄ»Øµ÷º¯Êı´¥·¢\r\n");
}
void vCallbackFunction_Time( TimerHandle_t xTimer )
{	//¶¨Ê±Æ÷¸üĞÂ
	uint8_t Buff[50];
  uint8_t Buff1[50];	
	uint8_t Buff2[50];
  Now_Time_Cnt=RTC_GetCounter();   
  //»ñÈ¡µ±Ç°µÄRTC¼ÆÊıÖµ 
  Now_Time=localtime(&Now_Time_Cnt);  //°Ñ»ñÈ¡µÄÊ±¼ä´Á×ª»»³ÉÊ±¼ä½á¹¹Ìå 
  //¸üĞÂÊ±¼ä 
  sprintf((char *)Buff,"%02d:%02d:%02d",Now_Time->tm_hour,Now_Time->tm_min,Now_Time->tm_sec);
  sprintf((char *)Buff1,"%04d",Now_Time->tm_year+1900);
	sprintf((char *)Buff2,"%02d.%02d",Now_Time->tm_mon+1,Now_Time->tm_mday);
	char *week_str[] = {"æ—¥ ","ä¸€ ","äºŒ ","ä¸‰ ","å›› ","äº” ","å…­ "};
	lv_label_set_text(guider_ui.screen_1_week,week_str[Now_Time->tm_wday]);
  lv_label_set_text(guider_ui.screen_1_time,(char *)Buff); 
	lv_label_set_text(guider_ui.screen_1_year,(char *)Buff1);
	lv_label_set_text(guider_ui.screen_1_monday,(char *)Buff2);
  //¸üĞÂÕûÊı·½·¨ 
  lv_label_set_text_fmt(guider_ui.screen_1_label_16,"%02d",Dht.Hum);
	lv_label_set_text_fmt(guider_ui.screen_1_label_12,"%02d",Sensor.CO2);
	//¸üĞÂĞ¡Êı·½·¨1 
  sprintf((char *)Buff,"%.1f",Dht.Tem); 
  // 
  lv_label_set_text(guider_ui.screen_1_label_10,(char *)Buff); 
  //¸üĞÂĞ¡Êı·½·¨2 
  //Èç¹ûÊ¹ÓÃ¸Ãº¯Êı¸üĞÂĞ¡Êı,ĞèÒª°Ñ#  define LV_SPRINTF_DISABLE_FLOAT 1¸Ä³É0 
  lv_label_set_text_fmt(guider_ui.screen_1_label_14,"%.1f",Dht.Tem);
	lv_label_set_text_fmt(guider_ui.screen_1_label_10,"%.1f",LightInten);
	
	if(LED1_FLAG==1)
	{
		lv_obj_set_hidden(guider_ui.screen_1_ledon,false); 
    lv_obj_set_hidden(guider_ui.screen_1_ledoff,true); 
	}
	else{
	  lv_obj_set_hidden(guider_ui.screen_1_ledon,true); 
    lv_obj_set_hidden(guider_ui.screen_1_ledoff,false); 
	}
	if(NET_State>=3)
	{
		lv_obj_set_hidden(guider_ui.screen_1_wifiON,false); 
    lv_obj_set_hidden(guider_ui.screen_1_wifiOFF,true); 
	}
	else{
	  lv_obj_set_hidden(guider_ui.screen_1_wifiON,true); 
    lv_obj_set_hidden(guider_ui.screen_1_wifiOFF,false); 
	}
}


void Delay(void)
{
	uint32_t i=4000000;
	while(i--){
	}
}

#endif
//ÈÎÎñÕ»ÇøÒç³ö¼ì²âº¯Êı  ÔÚFreeRTOSConfig.hÎÄ¼şÖĞÔö¼ÓÏÂÃæµÄºê  #define INCLUDE_uxTaskGetStackHighWaterMark 1
void vApplicationStackOverflowHook(TaskHandle_t xTask, char * pcTaskName )
{ printf("ÈÎÎñÕ»ÇøÒç³öÁË\r\n");
	UBaseType_t uxHighWaterMark;
	//»ñÈ¡Ä¿Ç°ÈÎÎñ»¹Ê£Óà¶àÉÙ¿Õ¼äÄÜÓÃ
	uxHighWaterMark = uxTaskGetStackHighWaterMark( xTask );
	//																				µ±Ç°ÏµÍ³Ê±¼ä					ÈÎÎñÃû×Ö			Ê£Óà¿Õ¼ä
	printf ("%u : StackOverflow:%s(%lu)\r\n", xTaskGetTickCount(), pcTaskName, uxHighWaterMark);	
	while(1)
	{
//		NVIC_SystemReset();  //ÈÃµ¥Æ¬»úÖØÆô
	}
}

//ÄÚ´æÉêÇëÊ§°Ü¹³×Óº¯Êı  ÔÚFreeRTOSConfig.hÎÄ¼şÖĞºêÖÃ1 #define configUSE_MALLOC_FAILED_HOOK 1 //Ê¹ÓÃÄÚ´æÉêÇëÊ§°Ü¹³×Óº¯Êı
void vApplicationMallocFailedHook (void)
{ printf("ÄÚ´æÉêÇëÊ§°Ü\r\n");
	while(1)
	{
//		NVIC_SystemReset();  //ÈÃµ¥Æ¬»úÖØÆô		
	}
}

//Ê±¼äÆ¬¹³×Óº¯Êı,Ã¿¸ö½ÚÅÄµ÷ÓÃ1´Î   #define configTICK_RATE_HZ ( ( TickType_t ) 1000 ) //RTOS Ïµ Í³ ½ÚÅÄÖĞ¶ÏµÄÆµÂÊ¡£
//¼´Ò»ÃëÖĞ¶ÏµÄ´ÎÊı£¬Ã¿´ÎÖĞ¶Ï RTOS ¶¼»á½øĞĞÈÎÎñµ÷¶È
//×¢Òâ:°Ñ¸ÃºêÖÃ1  #define configUSE_TICK_HOOK 1 //ÖÃ 1£ºÊ¹ÓÃÊ±¼äÆ¬¹³×Ó£¨Tick Hook£©£»ÖÃ 0£ººöÂÔÊ±¼äÆ¬¹³×Ó
//²»ÄÜÊÇËÀÑ­»·   
void vApplicationTickHook(void)
{//½Ó¿ÚµÄÊÊÅä  24
	lv_tick_inc(1);  //¸æËßLVGL¹ıÈ¥ÁË1ms
}

//#if (USE_STD_LIB==0) //Ö»ÒªÊ¹ÓÃÖĞ¶Ï£¬±ØĞëµ÷ÓÃÖĞ¶Ï·Ö×é
//	NVIC_SetPriorityGrouping(5);    //ÖĞ¶ÏÓÅÏÈ¼¶·Ö×é£¬¿ª»úµ÷ÓÃ1´Î
////core_cm3.h 1430ĞĞ    ·Ö×éÈ·¶¨Ö®ºó£¬ÇÀÕ¼ºÍ´Î¼¶ÓÅÏÈ¼¶ÒªÔÚÔÊĞí·¶Î§ÄÚ
//#elif (USE_STD_LIB==1)	
////void NVIC_PriorityGroupConfig(uint32_t NVIC_PriorityGroup);	misc.h 196ĞĞ
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
//#endif

////__STATIC_INLINE uint32_t SysTick_Config(uint32_t ticks)	misc.h 1827ĞĞ
//	SysTick_Config(SystemCoreClock/1000);
///*ÖĞ¶ÏÖÜÆÚµÄÈ·¶¨
//	1.	¸ù¾İÑ¡ÔñµÄÊ±ÖÓÔ´£¬CTRL¼Ä´æÆ÷µÄÎ»2ÖÃ1£¬Ñ¡ÔñAHB(72M)×÷ÎªÊ±ÖÓÔ´
//	2.	ËùÒÔ¼ÆÊıµÄÆµÂÊF=72M
//	3.	¸üĞÂ1´ÎÊı¾İµÄÊ±¼ät=1/F=1/72000000 s
//	4.	¶¨Ê±Æ÷µÄÖĞ¶Ï´¥·¢ÖÜÆÚT=ARR*t   ARR:Ö¸ÖØ×°ÔØÖµ£¬Ò²¾ÍÊÇLOAD¼Ä´æÆ÷Öµ
//	5.	¼ÙÈçÏëÒª1msµÄÖÜÆÚT£¬ĞèÒª¼Æ¶àÉÙ´ÎÊı£¿
//	T=ARR*t=1ms=0.001s=ARR*1/72000000 s
//	ARR= T/t=1ms/(1/72000000 s)=72000
//	ËùÒÔARR=72000
//ÖĞ¶Ïµ½À´Ö®ºó£¬×Ô¶¯µ÷ÓÃ  SysTick_Handler
//*/

/*GPIOE->CRL &= ~(0x0F<<20);
GPIOE->CRL  						  0100 0100 0100 0100 0100 0100 0100 0100
0x0F        							0000 0000 0000 0000 0000 0000 0000 1111
0x0F<<20     							0000 0000 1111 0000 0000 0000 0000 0000
~(0x0F<<20)               1111 1111 0000 1111 1111 1111 1111 1111 
GPIOE->CRL &= ~(0x0F<<20) 0100 0100 0000 0100 0100 0100 0100 0100


GPIOE->CRL |= (0x02<<20);
GPIOE->CRL  						  0100 0100 0000 0100 0100 0100 0100 0100
0x02                      0000 0000 0000 0000 0000 0000 0000 0010
0x02<<20 									0000 0000 0010 0000 0000 0000 0000 0000
GPIOE->CRL |= (0x02<<20)  0100 0100 0010 0100 0100 0100 0100 0100

GPIOA->IDR & (0x01<<0)
GPIOA->IDR                XXXX XXXX XXXX XXXX 
(0x01<<0)                 0000 0000 0000 0001
GPIOA->IDR & (0x01<<0)    0000 0000 0000 000X
¶ÔÓÚPA0°´¼ü  
µÃµ½µÄ½á¹û                0000 0000 0000 0001 °´ÏÂÁË
µÃµ½µÄ½á¹û                0000 0000 0000 0000 Î´°´ÏÂ

GPIOC->IDR & (0x01<<4)
GPIOC->IDR                XXXX XXXX XXXX XXXX 
(0x01<<4)                 0000 0000 0001 0000
GPIOC->IDR & (0x01<<4)    0000 0000 000X 0000
¶ÔÓÚPC4°´¼ü  
µÃµ½µÄ½á¹û                0000 0000 0001 0000 Î´°´ÏÂ
µÃµ½µÄ½á¹û                0000 0000 0000 0000 °´ÏÂÁË
*/
/*
ÑéÖ¤vTaskDelayºÍDelay_msµÄÇø±ğ		
vTaskDelay(1000);  //ÏµÍ³µÄÑÓÊ±  ²ÎÊı:ÒªÑÓÊ±µÄ½ÚÅÄÊı  
½ÚÅÄÓÉ¾ö¶¨£º#define configTICK_RATE_HZ ( ( TickType_t ) 1000 ) //RTOS Ïµ Í³ ½ÚÅÄÖĞ¶ÏµÄÆµÂÊ¡£¼´Ò»ÃëÖĞ¶ÏµÄ´ÎÊı£¬Ã¿´ÎÖĞ¶Ï RTOS ¶¼»á½øĞĞÈÎÎñµ÷¶È
Èç¹ûÕâ¸öºêÊÇ1000,ÄÇÃ´Ò»¸ö½ÚÅÄ¾ÍÊÇ1s/1000=1ms
	
vTaskDelay	 »áÖ÷¶¯ÊÍ·ÅCPU
Delay_ms     ²»»áÊÍ·ÅCPU£¬ÏëÒ»Ö±Õ¼ÓÃ*/


