#include "uart1.h"
#include "main.h"
#include "led.h"
#include "relay.h"
#include "stdio.h"
#include "stdint.h"
#include "stdbool.h"
#include "rtc.h"
#include "time.h"
#include "main.h"
#include "w25q64.h"
#include "Fontdownload.h"
/*
PA9     TX  复用推挽输出
PA10    RX  浮空输入
*/

ABC abc;
uint8_t U1_R_Buff[8];
uint8_t U1_R_Length ;
//uint16_t Font_Rec_Timeout;
//uint8_t Font_Rec_Ing;
//uint16_t Font_Rec_Cont;
//串口1的初始化
void UART1_Config()
{
	//1.开启GPIOA的时钟
	RCC->APB2ENR |= (0x01<<2);
	//2.3. PA9，PA10模式
	GPIOA->CRH &= ~(0xFF<<4);
	GPIOA->CRH |= (0x0A<<4);
	GPIOA->CRH |= (0x04<<8);
	//4.开UART1的时钟  6.3.7
	RCC->APB2ENR |= (0x01<<14);
  //5.使能串口1      25.6.4
  USART1->CR1 |= (0x01<<13);  
  //6.设置数据位     25.6.4
  USART1->CR1 &= ~(0x01<<12);    //位12清0，8个数据位 
  //7.设置校验方式，无校验 
  USART1->CR1 &= ~(0x01<<10);    //USART1->CR1寄存器的位10清0 	
  //8.设置停止位     25.6.5
  USART1->CR2 &= ~(0x03<<12);    //位13:12清0
	//9.设置波特率     25.3.4
  USART1->BRR =0x0271;
/*  想要波特率 9600 
    串口1挂载APB1fck=72000000,其他串口挂载APB2fck=36000000 
    USARTDIV = fck / (波特率 * 16) = 72000000/(16*9600)=468.75 
    整数部分：468=0x1D4 
    小数部分：0.75*16=12=0xC 
    拼接到一块  0x1D4C写入到BRR寄存器 
*/ 
  //10.设置发送使能 
  USART1->CR1 |= (0x01<<3);    //USART1->CR1 寄存器的位3置1
  //11.设置接收使能 	
	USART1->CR1 |= (0x01<<2); 
	//12.开启接收中断 
	USART1->CR1 |= (0x01<<5);    	//USART1->CR1 寄存器的位5置1  
	//13.开启空闲中断 
	USART1->CR1 |= (0x01<<4);    	//USART1->CR1 寄存器的位4置1   
	//14.NVIC 中设置中断优先级 
	NVIC_SetPriority(USART1_IRQn,6);   	//core_cm3.h 1439 行        
	//14.NVIC 中设置中断使能 
	NVIC_EnableIRQ(USART1_IRQn);       	//core_cm3.h 1432 行  	

}
void UART1_SendByte(uint8_t Byte) 
{ 
  //1.判断上一次是否发送完成   复位第一次为什么满足条件：因为该位复位后默认值是1   
  while((USART1->SR & (0x01<<6))==0) 
  {//2.还未发送完成   USART1->SR寄存器的位6是0
  } 
  //3.上一次数据发送完成了，发送新的数据 
  USART1->DR = Byte; 
  //4.清除标志位  参考手册25.6.1的位6描述 清除标志位两种方法 
  //(1)写 0 清 0  (2)软件序列清除该位(先读USART_SR，然后写入USART_DR) 
  USART1->SR &= ~(0x01<<6);
}
void UART1_SendArray(uint8_t *Buffer,uint16_t len)
{
	for(uint16_t i=0;i<len;i++)
	{
		UART1_SendByte(Buffer[i]);
	}
}
void UART1_SendStr(uint8_t *Str)
{
	while(*Str)
	{
		UART1_SendByte(*Str++);
	}
}
//重写fputc，实现printf
int fputc(int ch, FILE *f) 
{ 
UART1_SendByte(ch); 
return ch; 
} 
//阻塞接收 
/*uint8_t UART1_RecByte(void) 
{ 
	uint8_t Byte; 
	//1.判断 SR寄存器的位5是否为1    1  表示接收到数据  0  表示未接收到数据 
	while((USART1->SR & (0x01<<5))==0) 
	{//未接收到数据 
	} 
	//2.已经接收到数据，读取数据 
	Byte=USART1->DR; 
	//3.清除 SR 寄存器的位5 清除方法有两种 
	//(1)对 USART_DR 的读操作可以将该位清零  (2)RXNE位也可以通过写入0来清除 
	// USART1->SR &= ~(0x01<<5); 
	//4.返回读取的结果 
	return Byte; 
} */
 uint8_t Data; 
uint8_t data;
void USART1_IRQHandler(void) 
{
	if(USART_GetITStatus(USART1,USART_IT_RXNE)==SET) 
 { 
  //2.执行中断服务函数内容 
//uint16_t USART_ReceiveData(USART_TypeDef* USARTx);  stm32f10x_usart.h 379行 
  data=USART_ReceiveData(USART1);   
  U1_R_Buff[U1_R_Length++]=data;  //将数据放到接收缓冲区 
#if(FONT_Down_Num==16) 
  W25Q64_Write_Array(&data,FONT_16_Addr+Font_Rec_Cont,1);  //将接收到的数据写入W25Q64 
#elif(FONT_Down_Num==24) 
  W25Q64_Write_Array(&data,FONT_24_Addr+Font_Rec_Cont,1);  //将接收到的数据写入W25Q64 
  
#endif 
  Font_Rec_Cont++;  //接收数量+1 
  Font_Rec_Ing=1;  //表示正在接收字库数据 
  Font_Rec_Timeout=0;  //将溢出时间清0 
  if(U1_R_Length>=sizeof(U1_R_Buff)) 
  {//避免越界 
   U1_R_Length=0; 
  } 
  //3.清除中断标志位 
 //清除标志位   参考手册25.6.1的位5描述中，清除有两种方法 
   //(1)对USART_DR的读操作可以将该位清零。   (2)写0清0 
// USART_ClearITPendingBit(USART1,USART_IT_RXNE);    
 }

 //1.判断接收中断是否触发  SR寄存器的位5是否为1 
 if((USART1->SR & (0x01<<5))!=0) 
 { 
	Data=USART_ReceiveData(USART1);   
  USART_SendData(USART3,Data);
  //2.已经接收到数据，读取接收到的数据 
//  Data = USART1->DR; 
  abc.R_BUFF[abc.R_Cnt++]=Data; 
  //3.清除SR寄存器的位5 根据参考手册 25.6.1的位5描述，清除方法有两种     
  //(1)对USART_DR的读操作可以将该位清零  (2)RXNE位也可以通过写入0来清除 
// USART1->SR &= ~(0x01<<5); 
  if(abc.R_Cnt>=sizeof(abc.R_BUFF)) 
  {//避免数组越界 
   abc.R_Cnt=0; 
  } 
 }
  //1.判断空闲中断是否触发  SR寄存器的位4是否为1 
 if((USART1->SR & (0x01<<4))!=0) 
 { 
  //2.执行中断服务内容 
  abc.R_Idle=1;   //空闲标志位置1 
  //3.清除SR寄存器的位4 根据参考手册 25.6.1的位4描述，清除方法只能通过软件序列清除 
  //软件序列清除：先读USART_SR，然后读USART_DR 
  Data = USART1->DR;  //只是为了清除标志位
	Data = Data;        //防止编译器报警告 
 } 
}
//清除接收缓冲区 
void abc_Clear_R_Buff(void) 
{ 
 memset(abc.R_BUFF,0,KQM_R_Buff_Length); 
 abc.R_Cnt=0; 
} 
 
//abc的处理函数 
void Time_Handle(void) 
{ 
 uint8_t Data_CS1=0;
uint8_t Data_CS2=0;	
 uint8_t num;
 if(abc.R_Idle==1) 
 {//接收完成 
  abc.R_Idle=0; 
  //自己计算校验，检测校验是否通过 
   Data_CS1=abc.R_BUFF[0]+abc.R_BUFF[1]+abc.R_BUFF[2]+abc.R_BUFF[3]
	 +abc.R_BUFF[4]+abc.R_BUFF[5]+abc.R_BUFF[6];
  if(Data_CS1==abc.R_BUFF[7]) 
  {//校验通过 
   if(abc.R_BUFF[0]==0x5A) 
   { 
		if(abc.R_BUFF[1]==0x06)
		{
			 Set_Time.tm_sec  = 0;
       Set_Time.tm_min  = abc.R_BUFF[6];
       Set_Time.tm_hour = abc.R_BUFF[5];
       Set_Time.tm_mday = abc.R_BUFF[4];
       Set_Time.tm_mon  = abc.R_BUFF[3] - 1;               // 0~11
       Set_Time.tm_year = (2000 + abc.R_BUFF[2]) - 1900;   // 年份偏移
       Time_Adjust();
	  }
		else
		{
			printf("剩余长度不正确\r\n");
		}
   } 
   else 
   { 
    printf("abc地址码不正确\r\n"); 
   } 
  }
  else 
  { 
   printf("校验不通过\r\n"); 
  } 
  
  abc_Clear_R_Buff(); //清空接收缓冲区 
 } 
}
#include <stdarg.h>

//互斥机制pirntf  抄的deepseek的
int safe_printf(const char *format, ...)
{
    va_list args;
    int len;
    // 获取互斥信号量
    if(xSemaphoreTake(xSemaphore_Mutex_Printf, portMAX_DELAY) == pdTRUE)
    {
        va_start(args, format);
        len = vprintf(format, args);
        va_end(args);   
        // 释放互斥信号量
        xSemaphoreGive(xSemaphore_Mutex_Printf);
        return len;
    }
    return -1;
}


