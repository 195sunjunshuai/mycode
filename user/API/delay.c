#include "delay.h"
#include "rtctime.h"
/*
构造时间点任务作用：使while不需要调用阻塞延时
构造时间点任务步骤
1.定义数组
uint16_t LED_Period[2]={0,1000};     //成员1 周期初始值  成员2  周期值
2.在中断服务函数中累加第一个成员，可以选择系统定时器
  LED_Period[0]++;
3.在delay.h文件中，声明该数组
	extern uint16_t LED_Period[2];
4.在main函数，编写调用周期
		if(LED_Period[0]>=LED_Period[1])
		{
			LED_Period[0]=0;
		}	
5.对应的定时器一定要初始化，如果使用系统定时器，要调用系统定时器初始化
*/
//成员1 周期初始值  成员2  周期值


//延时一微秒
void Delay_1us()
{
		__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP(); 
		__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP(); 
		__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP(); 
		__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP(); 
		__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP(); 
		__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP(); 
		__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP(); 
		__NOP();__NOP(); 
}
//延时指定的us
void Delay_us(uint32_t time)
{
	for(uint32_t i=0;i<time;i++)
	{
		Delay_1us();
	}
}
//延迟指定的ms
void Delay_ms(uint32_t time)
{
	uint64_t Time=time*1000;
	for(uint64_t i=0;i<Time;i++)
	{
		Delay_1us();
	}
}
uint64_t system_time=0; //表示系统运行时间
//准确延时，阻塞
void Delay_systick_ms(uint32_t time)
{
	uint64_t time_temp=system_time+time;
	while(time_temp>=system_time)
	{//延时时间未到
	}
}
/*	while(time_temp>=system_time)
上述判断中，哪个变量在变，在哪里变，多长时间变1次
             system_time  systick中断    1ms
*/


/*
1.必须调用初始化   SysTick_Config(SystemCoreClock/1000);
2.按照指定周期自动调用
3.把stm32f10x_it.c中的相同函数名的函数屏蔽掉
*/
uint16_t LED_Period[2]={0,1000};   
uint8_t  KEY_Period[2]={0,10}; 
uint16_t RGB_R_Period[2]={0,1000}; 
uint16_t RGB_G_Period[2]={0,2000}; 
uint16_t RGB_B_Period[2]={0,3000};
uint16_t DHT_Period[2]={0,2000};
uint16_t RGB_B_Breath[2]={0,20};
uint16_t two_Period[2]={0,2000};
uint16_t onenine_Period[2]={0,1900};
uint16_t oneeight_Period[2]={0,1800};
uint16_t PUBLISH_Period[2]={0,19999};
uint16_t PING_Period[2]={0,14999};
uint32_t onemin[2]={0,1000*60*30};
//void SysTick_Handler(void)
//{
//	system_time++;
//	LED_Period[0]++;
//	KEY_Period[0]++;
//  RGB_R_Period[0]++;
//  RGB_G_Period[0]++;
//  RGB_B_Period[0]++;
//  DHT_Period[0]++;	
//	RGB_B_Breath[0]++;
//	two_Period[0]++;
//	PUBLISH_Period[0]++;
//	PUBLISH_Period[0]++;
//	static uint8_t tick10 = 0;
//    if (gMode == MODE_SW && sw_running) {
//        sw_ms++;     // 每 1 ms 加 1
//    }
//    // 也可以在这里做 10ms 的闪烁/动画等
//    if (++tick10 >= 10) tick10 = 0;
//   if(Font_Rec_Ing==1) 
//   {//正在接收 
//    Font_Rec_Timeout++; 
//   } 
//}

