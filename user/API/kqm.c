#include "kqm.h"
#include "string.h"
#include "stdio.h"
#include "main.h"
KQM kqm;

SENSOR Sensor;
void KQM_Config(void) 
{  
  //1.开 GPIOA 的时钟      
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);   //stm32f10x_rcc.h 693行 
  //2.定义结构体    参考STM32固件库使用手册的中文翻译版的10.2.3 
  //目的：给XXX_Init函数准备参数，XXX_Init函数需要传递这个类型的变量地址 
  //void GPIO_Init(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef* GPIO_InitStruct);  stm32f10x_gpio.h 351行 
  GPIO_InitTypeDef GPIO_InitStructure={0};  
  //3.给结构体赋值  参考STM32固件库使用手册的中文翻译版的10.2.3  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;              //待设置的引脚 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;        //引脚模式 复用推挽输出 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;       //引脚输出速率 
  //4.调用XXX_Init将结构体的参数写入到寄存器中 
  GPIO_Init(GPIOA, &GPIO_InitStructure);  
 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;              //待设置的引脚 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  //引脚模式 浮空输入  
  GPIO_Init(GPIOA, &GPIO_InitStructure);  
  
  //5.开串口2的时钟 
  //void RCC_APB2PeriphClockCmd(uint32_t RCC_APB2Periph, FunctionalState NewState);  stm32f10x_rcc.h 693
  //void RCC_APB1PeriphClockCmd(uint32_t RCC_APB1Periph, FunctionalState NewState);  
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE); 
  
  //6.定义串口2的结构体   XXX_Init函数准备 
  //void USART_Init(USART_TypeDef* USARTx, USART_InitTypeDef* USART_InitStruct);  stm32f10x_usart.h 366行 
  USART_InitTypeDef USART_InitStruct={0}; 
  
  //7.给结构体赋值 
  USART_InitStruct.USART_BaudRate = 9600;  //波特率    9600 8 N 1要和KQM6600保持一致 
//  USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  //硬件流控制 
  USART_InitStruct.USART_Mode = USART_Mode_Rx|USART_Mode_Tx; //发送和接收模式 
  USART_InitStruct.USART_Parity = USART_Parity_No; //校验方式 
  USART_InitStruct.USART_StopBits = USART_StopBits_1; //停止位 
  USART_InitStruct.USART_WordLength =USART_WordLength_8b;//数据位 
  
  //8.调用XXX_Init将参数写入寄存器 
  USART_Init(USART2,&USART_InitStruct); 

 //9.开启接收中断 
//void USART_ITConfig(USART_TypeDef* USARTx, uint16_t USART_IT, FunctionalState NewState);   stm32f10x_usart.h 371行 
 USART_ITConfig(USART2,USART_IT_RXNE,ENABLE); 
  
 //10.开启空闲中断 
 USART_ITConfig(USART2,USART_IT_IDLE,ENABLE); 
  
 //11.将串口2外设使能 
//void USART_Cmd(USART_TypeDef* USARTx, FunctionalState NewState); stm32f10x_usart.h 370行 
 USART_Cmd(USART2,ENABLE); 
 
 //12.定义NVIC的结构体 
//void NVIC_Init(NVIC_InitTypeDef* NVIC_InitStruct);  misc.h 197行 
 NVIC_InitTypeDef NVIC_InitStruct={0}; 
  
 //13.给结构体赋值 
 NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;   //中断通道 
 NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;  //中断通道使能 
 NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;//抢占优先级  范围一定要再中断分组允许范围内 
 NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0; //次级优先级 
  
 //14.调用XXX_Init函数将参数写入到寄存器中 
 NVIC_Init(&NVIC_InitStruct);   
} 
 
 
/* 
接收中断：每接收1个字节，触发1次接收中断 
空闲中断：当检测到总线空闲的时候，触发1次空闲中断 
总线空闲：1个完整字符(传输1个字节的时间)的高电平 
举个例子：对方连续发送0x31 0x32 0x33 0x34 0x35 0x36 
   总共触发7次中断，其中6次接收中断，1次空闲中断 
*/ 
void USART2_IRQHandler(void) 
{ 
 uint8_t Data; 
//ITStatus USART_GetITStatus(USART_TypeDef* USARTx, uint16_t USART_IT);  stm32f10x_usart1.h 392行 
//void USART_ClearITPendingBit(USART_TypeDef* USARTx, uint16_t USART_IT); 
 //1.判断接收中断是否触发  SR寄存器的位5是否为1 
 if(USART_GetITStatus(USART2,USART_IT_RXNE)==SET) 
 { 
  //2.已经接收到数据，读取接收到的数据 
  Data = USART_ReceiveData(USART2); 
  kqm.R_BUFF[kqm.R_Cnt++]=Data; 
  //3.清除SR寄存器的位5 根据参考手册 25.6.1的位5描述，清除方法有两种     
  //(1)对USART_DR的读操作可以将该位清零  (2)RXNE位也可以通过写入0来清除 
// USART_ClearITPendingBit(USART2,USART_IT_RXNE); 
  if(kqm.R_Cnt>=KQM_R_Buff_Length) 
{//避免数组越界 
   kqm.R_Cnt=0; 
  } 
 } 
 //1.判断空闲中断是否触发  SR寄存器的位4是否为1 
 if(USART_GetITStatus(USART2,USART_IT_IDLE)==SET) 
 { 
  //2.执行中断服务内容 
//  kqm.R_Idle=1;   //空闲标志位置1 
//	xSemaphoreGiveFromISR(xSemaphore,NULL);  //释放信号量
	 xQueueSendFromISR(xQueue,&kqm.R_BUFF,0); 
//    if(xReturned==pdTRUE) 
//   { 
//    printf("按键1按下,发送消息0x12成功\r\n"); 
//   }
  //3.清除SR寄存器的位4 根据参考手册 25.6.1的位4描述，清除方法只能通过软件序列清除 
  //软件序列清除：先读USART_SR，然后读USART_DR 
  Data = USART_ReceiveData(USART2);  //只是为了清除标志位 
  Data = Data;    //防止编译器报警告 
 } 
} 
 
//清除接收缓冲区 
void KQM_Clear_R_Buff(void) 
{ 
 memset(kqm.R_BUFF,0,KQM_R_Buff_Length); 
 kqm.R_Cnt=0; 
} 
 
//KQM的处理函数 
void KQM_Handle(void) 
{ 
 uint8_t Data_CS=0;   
  //自己计算校验，检测校验是否通过  
 Data_CS=kqm.R_BUFF[0]+kqm.R_BUFF[1]+kqm.R_BUFF[2]+kqm.R_BUFF[3]+kqm.R_BUFF[4]+kqm.R_BUFF[5]
 +kqm.R_BUFF[6]; 
  if(Data_CS==kqm.R_BUFF[7]) 
  {//校验通过 
   if(kqm.R_BUFF[0]==0x5F) 
   { 
    Sensor.VOC = ((kqm.R_BUFF[1]<<8)+kqm.R_BUFF[2])*0.1; 
//    Sensor.VOC = ((Kqm.R_Buff[1]*256)+Kqm.R_Buff[2])*0.1; 
//    Sensor.VOC = ((Kqm.R_Buff[1]*0x100)+Kqm.R_Buff[2])*0.1; 
    Sensor.CH2O = ((kqm.R_BUFF[3]<<8)+kqm.R_BUFF[4])*0.01; 
    Sensor.CO2 = (kqm.R_BUFF[5]<<8)+kqm.R_BUFF[6]; 
//    printf("VOC=%.1f,CH20=%.2f,CO2=%d",Sensor.VOC,Sensor.CH2O,Sensor.CO2); 
   } 
   else 
   { 
    printf("KQM地址码不正确\r\n"); 
   } 
  } 
  else 
  { 
   printf("KQM校验不通过\r\n"); 
  } 
   
  KQM_Clear_R_Buff(); //清空接收缓冲区   
  
// if(kqm.R_Idle==1) 
// {//接收完成 
//  kqm.R_Idle=0; 
//  //自己计算校验，检测校验是否通过 
// 
// Data_CS=kqm.R_BUFF[0]+kqm.R_BUFF[1]+kqm.R_BUFF[2]+kqm.R_BUFF[3]+kqm.R_BUFF[4]+kqm.R_BUFF[5]
// +kqm.R_BUFF[6]; 
//  if(Data_CS==kqm.R_BUFF[7]) 
//  {//校验通过 
//   if(kqm.R_BUFF[0]==0x5F) 
//   { 
//    Sensor.VOC = ((kqm.R_BUFF[1]<<8)+kqm.R_BUFF[2])*0.1; 
////    Sensor.VOC = ((Kqm.R_Buff[1]*256)+Kqm.R_Buff[2])*0.1; 
////    Sensor.VOC = ((Kqm.R_Buff[1]*0x100)+Kqm.R_Buff[2])*0.1; 
//    Sensor.CH2O = ((kqm.R_BUFF[3]<<8)+kqm.R_BUFF[4])*0.01; 
//    Sensor.CO2 = (kqm.R_BUFF[5]<<8)+kqm.R_BUFF[6]; 
//    printf("VOC=%.1f,CH20=%.2f,CO2=%d\r\n",Sensor.VOC,Sensor.CH2O,Sensor.CO2); 
//   } 
//   else 
//   { 
//    printf("KQM地址码不正确\r\n"); 
//   } 
//  } 
//  else 
//  { 
//   printf("KQM校验不通过\r\n"); 
//  } 
//   
//  KQM_Clear_R_Buff(); //清空接收缓冲区 
// } 
// KQM_Clear_R_Buff();  //千万不能在这里调用 
} 
