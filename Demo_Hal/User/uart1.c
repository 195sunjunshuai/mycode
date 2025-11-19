#include "uart1.h" 
#include "usart.h" 
#include "stdio.h" 
#include "string.h" 
#include "tim.h"
#include "kqm.h"
 
uint8_t U1_Rec_Buff[100];    //接收缓冲区 
uint8_t U1_Rec_Cnt;          //已经接收到的数量 
uint8_t U1_Rec_Idle;         //空闲标志位 
uint8_t U1_Rec_Single;       //存放接收的单个数据 
uint8_t U2_Rec_Buff[100];    //接收缓冲区 
uint8_t U2_Rec_Cnt;          //已经接收到的数量 
uint8_t U2_Rec_Idle;         //空闲标志位 
uint8_t U2_Rec_Single;       //存放接收的单个数据 
 
//HAL_UART_Transmit   阻塞发送 
//HAL_UART_Receive    阻塞接收 
//HAL_UART_Transmit_IT  非阻塞发送 -- 中断中使用 
//HAL_UART_Receive_IT   非阻塞接收 -- 中断中使用 
//HAL_UART_Transmit_DMA DMA发送 
//HAL_UART_Receive_DMA  DMA接收 
 
//单字节发送 
void UART1_SendByte(uint8_t Byte) 
{ 
//HAL_StatusTypeDef HAL_UART_Transmit(UART_HandleTypeDef *huart, const uint8_t *pData, uint16_t Size,uint32_t Timeout)  
 //参数1  串口的句柄，也就是串口几 
 //参数2  待发送的数据 
 //参数3  待发送数据的长度 
 //参数4  超时时间，如果超过这个还未发送完成，就退出阻塞 
 HAL_UART_Transmit(&huart1,&Byte,1,0x100); 
} 
 
//发送数组 
//参数1  数组的首地址 
//参数2  待发送数组的长度 
void UART1_SendBuff(uint8_t *Buff , uint16_t Length) 
{//注意 Length和i的类型 
 for(uint16_t i=0;i<Length;i++) 
 { 
  //UART1_SendByte(Buff[i]);   //循环发送 
  UART1_SendByte(*Buff++);   
 } 
} 
 
//发送字符串 
//参数   字符串的首地址 
void UART1_SendStr(uint8_t *Str) 
{ 
 while(*Str != '\0') 
 { 
  UART1_SendByte(*Str++);   
 } 
} 
/*printf 函数实现 
注意：一定要勾选魔法棒的USE MicroLIB 
*/ 
int fputc(int ch, FILE *f) 
{ 
UART1_SendByte(ch); 
return ch; 
}
//接收回调函数，不管串口几，都走这个接收回调 
//触发条件：接收够该函数HAL_UART_Receive_IT的第三个参数长度 
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) 
{ 
	if(huart->Instance == USART1) 
	{ 
	#if (UART1_TYPE==0) 
	U1_Rec_Buff[U1_Rec_Cnt++]=U1_Rec_Single; 
	if(U1_Rec_Cnt>=100) 
	{//越界检查 
	U1_Rec_Cnt=0; 
	} 
	//参数1   串口的句柄，也就是串口几 
	//参数2   接收数据存放的地址  
	//参数3   待接收的长度    接收后该参数长度，就会触发回调函数  HAL_UART_RxCpltCallback  
	HAL_UART_Receive_IT(&huart1,&U1_Rec_Single,1);   
	#elif (UART1_TYPE==1) 
	memset(U1_Rec_Buff,0,sizeof(U1_Rec_Buff)); 
	//参数1   串口的句柄，也就是串口几 
	//参数2   接收数据存放的地址  
	//参数3   待接收的长度    接收后该参数长度，就会触发回调函数  HAL_UART_RxCpltCallback  
	HAL_UART_Receive_IT(&huart1,U1_Rec_Buff,8);   
	#endif 
	} 
	if(huart->Instance == USART2) 
	{
		#if (UART2_TYPE==0) 
		kqm.R_BUFF[kqm.R_Cnt++]=U2_Rec_Single; 
		if(kqm.R_Cnt>=KQM_R_Buff_Length) 
    {//避免数组越界 
      kqm.R_Cnt=0;
			kqm.R_Idle=1;
    } 
		//参数1   串口的句柄，也就是串口几 
		//参数2   接收数据存放的地址  
		//参数3   待接收的长度    接收后该参数长度，就会触发回调函数  HAL_UART_RxCpltCallback  
		HAL_UART_Receive_IT(&huart2,&U2_Rec_Single,1);   
		#elif (UART2_TYPE==1) 
		//参数1   串口的句柄，也就是串口几 
		//参数2   接收数据存放的地址  
		//参数3   待接收的长度    接收后该参数长度，就会触发回调函数  HAL_UART_RxCpltCallback  		
    kqm.R_Idle=1;
		HAL_UART_Receive_IT(&huart2,kqm.R_BUFF,8);	
    #elif (UART2_TYPE==3)	
    kqm.R_BUFF[kqm.R_Cnt++]=U2_Rec_Single; 
		if(kqm.R_Cnt>=KQM_R_Buff_Length) 
    {//避免数组越界 
      kqm.R_Cnt=0;
    } 
    HAL_TIM_Base_Start_IT(&htim6);     
    //启动定时器 
    __HAL_TIM_SET_COUNTER(&htim6,0);   //清除定时器的计数值 	
    HAL_UART_Receive_IT(&huart2,&U2_Rec_Single,1);		
		#endif 
	} 
	if(huart->Instance == USART3) 
	{ 
	}  
	if(huart->Instance == UART4) 
	{ 
	}   
}	
//空闲的回调函数 
//调用条件1:总线触发空闲   
//调用条件2:HAL_UARTEx_ReceiveToIdle_IT 该函数第三个参数接收缓冲区满了  
//参数1  串口的句柄 
//参数2  已经接收到的长度 
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) 
{ 
	if(huart->Instance == USART1) 
	{ 
	#if (UART1_TYPE==0)
  		
	#elif (UART1_TYPE==1) 
	#elif (UART1_TYPE==2) 
	printf("串口 1 的空闲中断触发\r\n"); 
	printf("共接收到%d 个字节，内容是%s\r\n",Size,U1_Rec_Buff); 
	memset(U1_Rec_Buff,0,sizeof(U1_Rec_Buff));  //只是测试使用  
	//参数1   串口的句柄 
	//参数2   接收数据存放的地址 
	//参数 3   最大接收缓冲区长度   调用HAL_UARTEx_RxEventCallback  调用条件1:总线触发空闲  调用条件2:接收缓冲区满了 
	HAL_UARTEx_ReceiveToIdle_IT(&huart1,U1_Rec_Buff,sizeof(U1_Rec_Buff));    
	#endif 
	} 
	if(huart->Instance == USART2) 
	{
		#if (UART2_TYPE==0)		
		#elif (UART2_TYPE==1) 
		#elif (UART2_TYPE==2) 
		kqm.R_Idle=1; 
		//参数1   串口的句柄 
		//参数2   接收数据存放的地址 
		//参数 3   最大接收缓冲区长度   调用HAL_UARTEx_RxEventCallback  调用条件1:总线触发空闲  调用条件2:接收缓冲区满了 
		HAL_UARTEx_ReceiveToIdle_IT(&huart2,kqm.R_BUFF,sizeof(kqm.R_BUFF));    
		#endif 
	} 
	if(huart->Instance == USART3) 
	{ 
	} 
	if(huart->Instance == UART4) 
	{ 
		 
	}    
} 
