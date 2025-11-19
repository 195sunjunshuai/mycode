#include "kqm.h"
#include "string.h"
#include "stdio.h"
#include "main.h"
KQM kqm;

SENSOR Sensor;

 
 
/* 
接收中断：每接收1个字节，触发1次接收中断 
空闲中断：当检测到总线空闲的时候，触发1次空闲中断 
总线空闲：1个完整字符(传输1个字节的时间)的高电平 
举个例子：对方连续发送0x31 0x32 0x33 0x34 0x35 0x36 
   总共触发7次中断，其中6次接收中断，1次空闲中断 
*/ 
//void USART2_IRQHandler(void) 
//{ 
// uint8_t Data; 
////ITStatus USART_GetITStatus(USART_TypeDef* USARTx, uint16_t USART_IT);  stm32f10x_usart1.h 392行 
////void USART_ClearITPendingBit(USART_TypeDef* USARTx, uint16_t USART_IT); 
// //1.判断接收中断是否触发  SR寄存器的位5是否为1 
// if(USART_GetITStatus(USART2,USART_IT_RXNE)==SET) 
// { 
//  //2.已经接收到数据，读取接收到的数据 
//  Data = USART_ReceiveData(USART2); 
//  kqm.R_BUFF[kqm.R_Cnt++]=Data; 
//  //3.清除SR寄存器的位5 根据参考手册 25.6.1的位5描述，清除方法有两种     
//  //(1)对USART_DR的读操作可以将该位清零  (2)RXNE位也可以通过写入0来清除 
//// USART_ClearITPendingBit(USART2,USART_IT_RXNE); 
//  if(kqm.R_Cnt>=KQM_R_Buff_Length) 
//  {//避免数组越界 
//   kqm.R_Cnt=0; 
//  } 
// } 
// //1.判断空闲中断是否触发  SR寄存器的位4是否为1 
// if(USART_GetITStatus(USART2,USART_IT_IDLE)==SET) 
// { 
//  //2.执行中断服务内容 
////  kqm.R_Idle=1;   //空闲标志位置1 
////	xSemaphoreGiveFromISR(xSemaphore,NULL);  //释放信号量
//	 xQueueSendFromISR(xQueue,&kqm.R_BUFF,0); 
////    if(xReturned==pdTRUE) 
////   { 
////    printf("按键1按下,发送消息0x12成功\r\n"); 
////   }
//  //3.清除SR寄存器的位4 根据参考手册 25.6.1的位4描述，清除方法只能通过软件序列清除 
//  //软件序列清除：先读USART_SR，然后读USART_DR 
//  Data = USART_ReceiveData(USART2);  //只是为了清除标志位 
//  Data = Data;    //防止编译器报警告 
// } 
//} 
 
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
//  //自己计算校验，检测校验是否通过  
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
////    printf("VOC=%.1f,CH20=%.2f,CO2=%d",Sensor.VOC,Sensor.CH2O,Sensor.CO2); 
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
  
 if(kqm.R_Idle==1) 
 {//接收完成 
  kqm.R_Idle=0; 
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
    printf("VOC=%.1f,CH20=%.2f,CO2=%d\r\n",Sensor.VOC,Sensor.CH2O,Sensor.CO2); 
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
 }
// else
// {
//	printf("未接收完成\r\n");
// }	  
}
