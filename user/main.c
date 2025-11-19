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
#include "lvgl.h"    //�ӿڵ�����  18
#include "lv_port_disp.h"    //�ӿڵ�����  19
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
//����ľ��(ָ��ĳ���ڴ������ָ��),�൱�����������֤,ָ���������ȼ�����ں�������Ϣ��
//�����ĺܶ���������Ծ����Ϊ��������
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
TaskHandle_t xLvglTaskHandle = NULL;  //�ӿڵ�����  25
TaskHandle_t xHandle_UPDATE = NULL;
TaskHandle_t xHandle_IWDG = NULL;


SemaphoreHandle_t xSemaphore = NULL;    //������ֵ�ź������
SemaphoreHandle_t xSemaphore_Count = NULL;    //���������ź������
SemaphoreHandle_t xSemaphore_Mutex = NULL;    //���������ź������  
SemaphoreHandle_t xSemaphore_Mutex_Printf = NULL;    //����printf�����ź������  

QueueHandle_t		xQueue=NULL;     //������Ϣ���еľ��
QueueHandle_t   xQueue_AP=NULL;     //����APģʽ��Ϣ���еľ��
QueueHandle_t   xQueue_Update=NULL;     //����������Ϣ���еľ��

EventGroupHandle_t xCreatedEventGroup;     //�����¼����

TimerHandle_t xTimer = NULL;   //������ʱ�����
TimerHandle_t xTimer1 = NULL;   //������ʱ�����
TimerHandle_t xTimer2 = NULL;   //������ʱ�����

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
#if (USE_STD_LIB==0) //ֻҪʹ���жϣ���������жϷ���
	NVIC_SetPriorityGrouping(3);    //�ж����ȼ����飬��������1��
//core_cm3.h 1430��    ����ȷ��֮����ռ�ʹμ����ȼ�Ҫ��������Χ��
#elif (USE_STD_LIB==1)	
//void NVIC_PriorityGroupConfig(uint32_t NVIC_PriorityGroup);	misc.h 196��
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
	lv_init();    //LVGL�ĳ�ʼ��   //�ӿڵ�����  20
	lv_port_disp_init();    //LVGL��ʾ�豸�ĳ�ʼ��   //�ӿڵ�����  21
  ESP_Config();
	w25q_bg_decoder_register();
	IWDG_Init();
	
	//���������ʱ��ջ�ռ������  https://www.freertos.org/zh-cn-cmn-s/Why-FreeRTOS/FAQs/Memory-usage-boot-times-context#how-big-should-the-stack-be 
  xReturned = xTaskCreate(
                  vTaskLED1,       //����������ڵ�ַ,�����ǲ��˳���ѭ��
                  "vTaskLED1",     //���������������,��ʵ����
									83,             //����Ķ�ջ�ռ��С  ��λ:��  1��=4�ֽ�
                  NULL,            //�������
                  1,//��������ȼ�,����Խ�����ȼ�Խ�ߡ����ȼ�Ĭ������Ϊ (configMAX_PRIORITIES - 1)
                  &xHandle_LED1 );      //����ľ��
	if(xReturned==pdPASS)
	{printf("LED1���񴴽��ɹ�\r\n");
	}
  xReturned = xTaskCreate(vTaskRELAY,"vTaskLED2",80,NULL,2,&xHandle_RELAY ); 
	if(xReturned==pdPASS)
	{printf("RELAY���񴴽��ɹ�\r\n");
	}
  xReturned = xTaskCreate(vTaskKEY,"vTaskKEY",500,NULL,8,&xHandle_KEY ); 
	if(xReturned==pdPASS)
	{printf("KEY���񴴽��ɹ�\r\n");
	}		
  xReturned = xTaskCreate(vTaskKQM,"vTaskKQM",90,NULL,4,&xHandle_KQM ); 
	if(xReturned==pdPASS)
	{printf("KQM���񴴽��ɹ�\r\n");
	}	
  xReturned = xTaskCreate(vTaskSU03T,"vTaskSU03T",128,NULL,5,&xHandle_SU03T ); 
	if(xReturned==pdPASS)
	{printf("SU03T���񴴽��ɹ�\r\n");
	}
  xReturned = xTaskCreate(vTaskSMOKE,"vTaskSMOKE",88,NULL,6,&xHandle_SMOKE ); 
	if(xReturned==pdPASS)
	{printf("SMOKE���񴴽��ɹ�\r\n");
	}	
	xReturned = xTaskCreate(vTaskBH1750,"vTaskBH1750",88,NULL,3,&xHandle_BH1750 ); 
	if(xReturned==pdPASS)
	{printf("BH1750���񴴽��ɹ�\r\n");
	}
	xReturned = xTaskCreate(vTaskESP,"vTaskESP",360,NULL,7,&xHandle_ESP ); 
	if(xReturned==pdPASS)
	{printf("ESP���񴴽��ɹ�\r\n");
	}
	xReturned = xTaskCreate(vTaskDHT11,"vTaskDHT11",88,NULL,9,&xHandle_DHT11 ); 
	if(xReturned==pdPASS)
	{printf("DHT11���񴴽��ɹ�\r\n");
	}
	//�ӿڵ�����  28
	xReturned = xTaskCreate(vLvglTaskFunction, "lvgl_task", 512, NULL, 9, &xLvglTaskHandle);	
	if(xReturned==pdPASS)
	{
	}
	xReturned = xTaskCreate(vTaskUPDATE, "vTaskUPDATE", 216, NULL, 9, &xHandle_UPDATE);	
	if(xReturned==pdPASS)
	{printf("����LVGL���񴴽��ɹ�\r\n");
	}
	xReturned = xTaskCreate(vTaskIWDG, "vTaskIWDG", 128, NULL, 1, &xHandle_IWDG);	
	if(xReturned==pdPASS)
	{printf("����LVGL���񴴽��ɹ�\r\n");
	}
	
	//����ֵ  ��ֵ�ź������ 
	xSemaphore = xSemaphoreCreateBinary();   //������ֵ�ź���
	if(xSemaphore!=NULL)
	{printf("KQM��ֵ�ź��������ɹ�\r\n");	
	}
	xSemaphoreGive(xSemaphore);   //�ͷ��ź���,�øߵ����ȼ���������
	xSemaphore_Mutex = xSemaphoreCreateMutex();   //���������ź���
	if(xSemaphore_Mutex!=NULL)
	{printf("�����ź��������ɹ�\r\n");	
	}	
	xSemaphore_Mutex_Printf=xSemaphoreCreateMutex();   //���������ź���
	if(xSemaphore_Mutex_Printf!=NULL)
	{safe_printf("printf�����ź��������ɹ�\r\n");	
	}	
	//������Ϣ����
	//����1    �û������ܹ���Ŷ��ٸ���Ϣ
	//����2    ÿ����Ϣ�Ĵ�С
	//����ֵ   ��Ϣ���еľ��   ��λ:�ֽ�
  xQueue = xQueueCreate(1,8);
	if(xQueue != NULL)
	{printf("KQM��Ϣ���д����ɹ�\r\n");		
	}	
  xQueue_AP = xQueueCreate(1,1); 
  if(xQueue_AP != NULL) 
  {printf("APģʽ��Ϣ���д����ɹ�\r\n");   
  }
  xQueue_Update=xQueueCreate(1,1);
  if(xQueue_Update != NULL) 
  {printf("������Ϣ���д����ɹ�\r\n");   
  } 	
	//�����¼�
	xCreatedEventGroup = xEventGroupCreate();
	if(xCreatedEventGroup!=NULL)
	{printf("SU03T�¼������ɹ�\r\n");		
	}

	//����1  ��ʱ���Ŀɶ��ı�����,û��ɶʵ������
	//����2  ��ʱ��������
	//����3  ��ʱ����ģʽ  pdTRUE  ����ģʽ  pdFALSE  ����ģʽ
	//����4  ��������ڴ����Ķ�ʱ���ı�ʶ����ͨ������£���ͬһ�ص���������������ʱ��ʱ���ú��������ڶ�ʱ���ص������� ��ʶ���ĸ���ʱ������
	//����5  ��ʱ���Ļص�����
	//����ֵ ��ʱ���ľ��
	xTimer1=xTimerCreate("xTimer1",1000*60*60,pdTRUE,( void * ) 1,vCallbackFunction_Time1);
	if(xTimer1!=NULL)
	{printf("������ʱ��1�����ɹ�\r\n");		
		//����1  ������ʱ�����
		//����2  ����ʱ��  ����������£�xBlockTime�������ԣ�xTimerStart()�� RTOS ����������֮ǰ�ͱ����á�
		if(xTimerStart(xTimer1,0)==pdPASS)     //����������ʱ��
		{printf("������ʱ��1�����ɹ�\r\n");				
		}
	}
//	xTimer2=xTimerCreate("xTimer2",3000,pdFALSE,( void * ) 2,vCallbackFunction_Time2);	
//	if(xTimer2!=NULL)
//	{printf("������ʱ��2�����ɹ�\r\n");		
//		if(xTimerStart(xTimer2,0)==pdPASS)     //����������ʱ��
//		{
//			printf("������ʱ��2�����ɹ�\r\n");				
//		}	
//	}
	xTimer=xTimerCreate("xTimer",1000,pdTRUE,( void * ) 1,vCallbackFunction_Time);
	if(xTimer!=NULL)
	{printf("������ʱ��1�����ɹ�\r\n");		
		//����1  ������ʱ�����
		//����2  ����ʱ��  ����������£�xBlockTime�������ԣ�xTimerStart()�� RTOS ����������֮ǰ�ͱ����á�
		if(xTimerStart(xTimer,0)==pdPASS)     //����������ʱ��
		{printf("������ʱ�������ɹ�\r\n");				
		}
	}
	vTaskStartScheduler();  //�����������֮��,����������,����������֮��Ĵ��붼����ִ��
	while (1)
	{printf("��ҿ����Ƿ����ִ�е���仰\r\n");
	}
}
 
void vTaskLED1( void * pvParameters )
{ //LED��
	
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
		vTaskDelay(1000);  //ϵͳ����ʱ,���ͷ�CPU  ����:Ҫ��ʱ�Ľ����� 
	}
}

void vTaskRELAY( void * pvParameters )
{ //�̵���
					  uint32_t freeheap=0;
	TaskStatus_t taskinfo;//����״̬
	for( ;; )
	{
//		//#define configTOTAL_HEAP_SIZE ( ( size_t ) ( 17 * 1024 ) ) //ϵͳ�����ܵĶѴ�С  64K�ռ����
//		freeheap=xPortGetFreeHeapSize();//��ȡϵͳʣ���ջ�ܿռ��С
//		printf("����ϵͳʣ���ջ�ռ��С��%d�ֽ�\r\n",freeheap);
////		//��ȡĳһ�����״̬��Ϣ
////		//��FreeRTOSConfig.h�ļ��к���1 #define configUSE_TRACE_FACILITY 1 //���ÿ��ӻ����ٵ���
//		vTaskGetInfo(xHandle_LED1,&taskinfo,pdTRUE,eInvalid);//������ ��ȡ��������״̬��Ϣ ��ȡ���Ķ�ջʣ��ռ� 
//		printf("RELAYTask_Handleʣ���ջ�ռ��С��%d��\r\n",taskinfo.usStackHighWaterMark);
//		RELAY_TOGGLE();
		vTaskDelay(1500);		
	}
}

void vTaskKEY( void * pvParameters )
{ //����
  TickType_t xLastWakeTime;
  const TickType_t xFrequency = 10;  //��Ҫ�ľ�����ʱ���ڣ���λ��ϵͳ����
  xLastWakeTime = xTaskGetTickCount();	 //��ȡ��ǰϵͳʱ��
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
//	 xReturn_Value =xSemaphoreTake(xSemaphore,2000);  //һֱ��,ֱ���ȵ��ź���,��ӡ�ɹ� 
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
			{//��ַ����ȷ
				data_cs=Buff[0]+Buff[1]+Buff[2]+Buff[3]+Buff[4]+Buff[5]+Buff[6];
				if(data_cs==Buff[7])
				{//У��ͨ��
					Sensor.VOC = ((Buff[1]<<8)+Buff[2])*0.1;
	//				Sensor.VOC = (Buff[1]*0x100+Buff[2])*0.1;
					Sensor.CH2O = ((Buff[3]<<8)+Buff[4])*0.01;
					Sensor.CO2 = (Buff[5]<<8)+Buff[6];
//					printf("VOC=%.1f,CH2O=%.2f,CO2=%d\r\n",Sensor.VOC,Sensor.CH2O,Sensor.CO2);
				}
				else
				{//У�鲻ͨ��
					printf("У�鲻ͨ��\r\n");
				}
			}
			else
			{//��ַ�벻��ȷ
				printf("��ַ�벻��ȷ\r\n");
			}
			KQM_Clear_R_Buff();	
		}
		else
		{
			printf("KQM6600��Ϣ���л�ȡʧ��\r\n");
		}	
	}
}

void vTaskSU03T( void * pvParameters )
{ //SU03T
	EventBits_t uxBits;	
	for( ;; )
	{
		//����1  �¼����
		//����2  ��ϵ��λ 
		//����3  �˳���ʱ��,�Ƿ������Ӧ��λ  pdTRUE   pdFALSE  �������Ҫ���ú���xEventGroupClearBits�ֶ����
		//����4  �Ƿ���Ҫ���ĵ�λ������   pdTRUE   pdFALSE
		//����5  �ȴ���ʱ�� ���Եδ�Ϊ��λ��
						//����2��3��д��   1.0  ���ȴ�   2.��0����   �ȹ̶�ʱ��   3.portMAX_DELAY  һֱ��,ֱ���ȵ��ź���
		uxBits=xEventGroupWaitBits(xCreatedEventGroup,BIT_0,pdTRUE,pdTRUE,portMAX_DELAY);
		if(uxBits==BIT_0)
		{
			SU03T_Handle();
		}
		else 
		{
			printf("SU03T�¼�û����\r\n");
		}		
	}
}

void vTaskSMOKE( void * pvParameters )
{ //����
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
    NET_State=Message;    //NET_State�л���6 
    NET_Step=0; 
//   printf("����AP��Ϣ�ɹ�,׼���л���APģʽ\r\n"); 
    safe_printf("����AP��Ϣ�ɹ�,׼���л���APģʽ\r\n"); 
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
		{//����״̬����3�����յ���������
			esp.R_Idle=0;
			if(esp.R_Buff[0]==0x30)
			{
//{"paras":{"relay_switch":"1"},"service_id":"Device_205","command_name":"led_switch"}
				printf("�յ�ƽ̨���·����ñ���\r\n");
//temp=(uint8_t *)strstr((char *)Esp.R_Buff,"led_switch"); �����������ң���Ϊǰ����0
				//����ж�����أ���д����������Һͽ���
				temp=(uint8_t *)strstr((char *)&esp.R_Buff[4],"led_switch");
				temp1=(uint8_t *)strstr((char *)&esp.R_Buff[4],"relay_switch");
				if(temp!=NULL)
				{//���ҵ��ˣ�����led_switch�е�l���ڵĵ�ַ
					temp+=13; //ƫ�Ƶ���״̬0/1����λ��
					if(*temp=='1')
					{//����
						LED2_ON();
					}
					else if(*temp=='0')
					{//�ص�
						LED2_OFF();						
					}
				}
					if(temp1!=NULL)
				{//���ҵ��ˣ�����led_switch�е�l���ڵĵ�ַ
					temp1+=15; //ƫ�Ƶ���״̬0/1����λ��
					if(*temp1=='1')
					{//����
						RELAY_ON();
					}
					else if(*temp1=='0')
					{//�ص�
						RELAY_OFF();						
					}
				}
			}
			if(esp.R_Buff[0]==0xD0)
			{
				printf("�յ�ƽ̨�������ظ�\r\n");				
			}
			ESP_Clear_R_Buff();  //���һ�½��ջ�������������ͷ����
		}
		vTaskDelay(2000);
	 }		
}
void vLvglTaskFunction( void * pvParameters )
{ //�ӿڵ�����  26
	TickType_t xLastWakeTime;
	const TickType_t xPeriod = pdMS_TO_TICKS( 5 );
	xLastWakeTime = xTaskGetTickCount(); 
//  setup_scr_screen(&guider_ui);    //������Ļ1����Ļ�еĶ��� 
  setup_scr_screen_1(&guider_ui);    //������Ļ2����Ļ�еĶ��� 
  lv_scr_load(guider_ui.screen_1);    //������Ļ1  	
//	xTimerStart(xTimer,0);
//	Lvgl_Demo11();
	for(;;)
	{		
		lv_task_handler();
		vTaskDelayUntil( &xLastWakeTime, xPeriod );//������ʱ
	}
}


void vTaskUPDATE ( void * pvParameters )
{ //��������
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
			//		lv_label_set_text(guider_ui.screen_1_tianqi, "�?");
					lv_img_set_src(guider_ui.screen_1_rain,&_sun_alpha_35x35);
				}
				else if(weather_code>=4 && weather_code<=9)
				{
			//		lv_label_set_text(guider_ui.screen_1_tianqi, "多云 ");
			//		lv_img_set_src(guider_ui.screen_1_img_13,&_213242C09CA4EE3A5E54359E297D2991_alpha_102x100);
					lv_img_set_src(guider_ui.screen_1_rain,&_yun_alpha_35x35);
				}
				else if(weather_code>=10 && weather_code<=20)
				{  
			//	  lv_label_set_text(guider_ui.screen_1_tianqi, "�?");
			//		lv_img_set_src(guider_ui.screen_1_rain,&_rainsmall_alpha_35x35);
					lv_img_set_src(guider_ui.screen_1_rain,&_rainsmall_alpha_35x35);
				}
				if(weather_nb_code>=0 && weather_nb_code<=3)
				{
			//		lv_label_set_text(guider_ui.screen_1_tianqi, "�?");
					lv_img_set_src(guider_ui.screen_1_img_7_15,&_sun_alpha_35x35);
				}
				else if(weather_nb_code>=4 && weather_nb_code<=9)
				{
			//		lv_label_set_text(guider_ui.screen_1_tianqi, "多云 ");
			//		lv_img_set_src(guider_ui.screen_1_img_13,&_213242C09CA4EE3A5E54359E297D2991_alpha_102x100);
					lv_img_set_src(guider_ui.screen_1_img_7_15,&_yun_alpha_35x35);
				}
				else if(weather_nb_code>=10 && weather_nb_code<=20)
				{  
			//	  lv_label_set_text(guider_ui.screen_1_tianqi, "�?");
			//		lv_img_set_src(guider_ui.screen_1_rain,&_rainsmall_alpha_35x35);
					lv_img_set_src(guider_ui.screen_1_img_7_15,&_rainsmall_alpha_35x35);
				}
			}
		}
  }	
}

//������ʱ���ص��������൱��֮ǰ��ʱ�����жϷ�����  ���ǲ��˳���ѭ��
void vCallbackFunction_Time1( TimerHandle_t xTimer )
{ //�ո�������Ϣ
	uint8_t update_Message=1;
	xQueueSend(xQueue_Update,&update_Message,0);
}
void vTaskIWDG( void * pvParameters )
{ //ι��
	for( ;; )
	{			
		IWDG_Feed();
		vTaskDelay(2000);  //ϵͳ����ʱ,���ͷ�CPU  ����:Ҫ��ʱ�Ľ����� 
	}
}

//������ʱ���ص��������൱��֮ǰ��ʱ�����жϷ�����  ���ǲ��˳���ѭ��
void vCallbackFunction_Time2( TimerHandle_t xTimer )
{printf("������ʱ��2�Ļص���������\r\n");
}
void vCallbackFunction_Time( TimerHandle_t xTimer )
{	//��ʱ������
	uint8_t Buff[50];
  uint8_t Buff1[50];	
	uint8_t Buff2[50];
  Now_Time_Cnt=RTC_GetCounter();   
  //��ȡ��ǰ��RTC����ֵ 
  Now_Time=localtime(&Now_Time_Cnt);  //�ѻ�ȡ��ʱ���ת����ʱ��ṹ�� 
  //����ʱ�� 
  sprintf((char *)Buff,"%02d:%02d:%02d",Now_Time->tm_hour,Now_Time->tm_min,Now_Time->tm_sec);
  sprintf((char *)Buff1,"%04d",Now_Time->tm_year+1900);
	sprintf((char *)Buff2,"%02d.%02d",Now_Time->tm_mon+1,Now_Time->tm_mday);
	char *week_str[] = {"日 ","一 ","二 ","三 ","四 ","五 ","六 "};
	lv_label_set_text(guider_ui.screen_1_week,week_str[Now_Time->tm_wday]);
  lv_label_set_text(guider_ui.screen_1_time,(char *)Buff); 
	lv_label_set_text(guider_ui.screen_1_year,(char *)Buff1);
	lv_label_set_text(guider_ui.screen_1_monday,(char *)Buff2);
  //������������ 
  lv_label_set_text_fmt(guider_ui.screen_1_label_16,"%02d",Dht.Hum);
	lv_label_set_text_fmt(guider_ui.screen_1_label_12,"%02d",Sensor.CO2);
	//����С������1 
  sprintf((char *)Buff,"%.1f",Dht.Tem); 
  // 
  lv_label_set_text(guider_ui.screen_1_label_10,(char *)Buff); 
  //����С������2 
  //���ʹ�øú�������С��,��Ҫ��#  define LV_SPRINTF_DISABLE_FLOAT 1�ĳ�0 
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
//����ջ�������⺯��  ��FreeRTOSConfig.h�ļ�����������ĺ�  #define INCLUDE_uxTaskGetStackHighWaterMark 1
void vApplicationStackOverflowHook(TaskHandle_t xTask, char * pcTaskName )
{ printf("����ջ�������\r\n");
	UBaseType_t uxHighWaterMark;
	//��ȡĿǰ����ʣ����ٿռ�����
	uxHighWaterMark = uxTaskGetStackHighWaterMark( xTask );
	//																				��ǰϵͳʱ��					��������			ʣ��ռ�
	printf ("%u : StackOverflow:%s(%lu)\r\n", xTaskGetTickCount(), pcTaskName, uxHighWaterMark);	
	while(1)
	{
//		NVIC_SystemReset();  //�õ�Ƭ������
	}
}

//�ڴ�����ʧ�ܹ��Ӻ���  ��FreeRTOSConfig.h�ļ��к���1 #define configUSE_MALLOC_FAILED_HOOK 1 //ʹ���ڴ�����ʧ�ܹ��Ӻ���
void vApplicationMallocFailedHook (void)
{ printf("�ڴ�����ʧ��\r\n");
	while(1)
	{
//		NVIC_SystemReset();  //�õ�Ƭ������		
	}
}

//ʱ��Ƭ���Ӻ���,ÿ�����ĵ���1��   #define configTICK_RATE_HZ ( ( TickType_t ) 1000 ) //RTOS ϵ ͳ �����жϵ�Ƶ�ʡ�
//��һ���жϵĴ�����ÿ���ж� RTOS ��������������
//ע��:�Ѹú���1  #define configUSE_TICK_HOOK 1 //�� 1��ʹ��ʱ��Ƭ���ӣ�Tick Hook������ 0������ʱ��Ƭ����
//��������ѭ��   
void vApplicationTickHook(void)
{//�ӿڵ�����  24
	lv_tick_inc(1);  //����LVGL��ȥ��1ms
}

//#if (USE_STD_LIB==0) //ֻҪʹ���жϣ���������жϷ���
//	NVIC_SetPriorityGrouping(5);    //�ж����ȼ����飬��������1��
////core_cm3.h 1430��    ����ȷ��֮����ռ�ʹμ����ȼ�Ҫ��������Χ��
//#elif (USE_STD_LIB==1)	
////void NVIC_PriorityGroupConfig(uint32_t NVIC_PriorityGroup);	misc.h 196��
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
//#endif

////__STATIC_INLINE uint32_t SysTick_Config(uint32_t ticks)	misc.h 1827��
//	SysTick_Config(SystemCoreClock/1000);
///*�ж����ڵ�ȷ��
//	1.	����ѡ���ʱ��Դ��CTRL�Ĵ�����λ2��1��ѡ��AHB(72M)��Ϊʱ��Դ
//	2.	���Լ�����Ƶ��F=72M
//	3.	����1�����ݵ�ʱ��t=1/F=1/72000000 s
//	4.	��ʱ�����жϴ�������T=ARR*t   ARR:ָ��װ��ֵ��Ҳ����LOAD�Ĵ���ֵ
//	5.	������Ҫ1ms������T����Ҫ�ƶ��ٴ�����
//	T=ARR*t=1ms=0.001s=ARR*1/72000000 s
//	ARR= T/t=1ms/(1/72000000 s)=72000
//	����ARR=72000
//�жϵ���֮���Զ�����  SysTick_Handler
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
����PA0����  
�õ��Ľ��                0000 0000 0000 0001 ������
�õ��Ľ��                0000 0000 0000 0000 δ����

GPIOC->IDR & (0x01<<4)
GPIOC->IDR                XXXX XXXX XXXX XXXX 
(0x01<<4)                 0000 0000 0001 0000
GPIOC->IDR & (0x01<<4)    0000 0000 000X 0000
����PC4����  
�õ��Ľ��                0000 0000 0001 0000 δ����
�õ��Ľ��                0000 0000 0000 0000 ������
*/
/*
��֤vTaskDelay��Delay_ms������		
vTaskDelay(1000);  //ϵͳ����ʱ  ����:Ҫ��ʱ�Ľ�����  
�����ɾ�����#define configTICK_RATE_HZ ( ( TickType_t ) 1000 ) //RTOS ϵ ͳ �����жϵ�Ƶ�ʡ���һ���жϵĴ�����ÿ���ж� RTOS ��������������
����������1000,��ôһ�����ľ���1s/1000=1ms
	
vTaskDelay	 �������ͷ�CPU
Delay_ms     �����ͷ�CPU����һֱռ��*/


