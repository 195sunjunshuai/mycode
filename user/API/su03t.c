#include "su03t.h" 
#include "string.h" 
#include "stdio.h" 
#include "led.h" 
#include "ele_machine.h"
#include "kqm.h"
#include "dht.h"
#include "relay.h"
#include "bh1750.h"
#include "main.h"
#include "rtc.h"
#include "time.h"
uint8_t REV;
SU03T Su03t; 
extern SENSOR Sensor;
extern uint8_t Buff[5];
//extern double sum;
void SU03T_Config(void) 
{ 
//中文固件库翻译手册路径：授课大纲资料\1.项目分析与环境搭建\资料\芯片相关资料\STM32固件库使用手册的中文翻译版 
//1.开 GPIOC 的时钟      STM32 固件库使用手册的中文翻译版的15.2.22
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);   //stm32f10x_rcc.h 693行 
  
 //2.定义结构体    参考STM32固件库使用手册的中文翻译版的10.2.3 
 //目的：给XXX_Init函数准备参数，XXX_Init函数需要传递这个类型的变量地址 
//void GPIO_Init(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef* GPIO_InitStruct);  stm32f10x_gpio.h 351行 
 GPIO_InitTypeDef GPIO_InitStructure={0};  
  
 //3.给结构体赋值  参考STM32固件库使用手册的中文翻译版的10.2.3  
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //待设置的引脚 
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //引脚模式 复用推挽输出 
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;  //引脚输出速率 
  
 //4.调用XXX_Init将结构体的参数写入到寄存器中 
 GPIO_Init(GPIOC, &GPIO_InitStructure);  
 
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; //待设置的引脚 
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  //引脚模式 浮空输入  
  
 GPIO_Init(GPIOC, &GPIO_InitStructure);  
  
 //5.开串口4的时钟 
//void RCC_APB2PeriphClockCmd(uint32_t RCC_APB2Periph, FunctionalState NewState);  stm32f10x_rcc.h 693行 
//void RCC_APB1PeriphClockCmd(uint32_t RCC_APB1Periph, FunctionalState NewState);  
 RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE); 
  
 //6.定义串口4的结构体   XXX_Init函数准备 
//void USART_Init(USART_TypeDef* USARTx, USART_InitTypeDef* USART_InitStruct);  stm32f10x_usart.h 366行 
 USART_InitTypeDef USART_InitStruct={0}; 
  
 //7.给结构体赋值 
 USART_InitStruct.USART_BaudRate = 9600;  //波特率    9600 8 N 1要和SU03T保持一致 
 USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  //硬件流控制 
 USART_InitStruct.USART_Mode = USART_Mode_Rx|USART_Mode_Tx; //发送和接收模式 
 USART_InitStruct.USART_Parity = USART_Parity_No; //校验方式 
 USART_InitStruct.USART_StopBits = USART_StopBits_1; //停止位 
 USART_InitStruct.USART_WordLength =USART_WordLength_8b;//数据位 
  
 //8.调用XXX_Init将参数写入寄存器 
 USART_Init(UART4,&USART_InitStruct); 
  
 //9.将外设使能 
//void USART_Cmd(USART_TypeDef* USARTx, FunctionalState NewState); stm32f10x_usart.h 370行 
 USART_Cmd(UART4,ENABLE);
 //10.开启接收中断 
//void USART_ITConfig(USART_TypeDef* USARTx, uint16_t USART_IT, FunctionalState NewState);   stm32f10x_usart.h 371行 
 USART_ITConfig(UART4,USART_IT_RXNE,ENABLE); 
  
 //11.开启空闲中断 
 USART_ITConfig(UART4,USART_IT_IDLE,ENABLE);   
 
 //12.定义NVIC的结构体 
//void NVIC_Init(NVIC_InitTypeDef* NVIC_InitStruct);  misc.h 197行 
 NVIC_InitTypeDef NVIC_InitStruct={0}; 
  
 //13.给结构体赋值 
 NVIC_InitStruct.NVIC_IRQChannel = UART4_IRQn;   //中断通道 
 NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;  //中断通道使能 
 NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;//抢占优先级  范围一定要再中断分组允许范围内 
 NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2; //次级优先级 
  
 //14.调用XXX_Init函数将参数写入到寄存器中 
 NVIC_Init(&NVIC_InitStruct);   
} 
 
 
//单字节发送 
/*读参考手册 25.3.2的单字节通信部分理解 
读参考手册  25.6.1的位6和位7理解 
根据参考手册25.6.1的寄存器复位值，TC位复位就是1 
*/ 
void UART4_SendByte(uint8_t Byte) 
{ 
//FlagStatus USART_GetFlagStatus(USART_TypeDef* USARTx, uint16_t USART_FLAG);  stm32f10x_usart.h 390行 
//void USART_ClearFlag(USART_TypeDef* USARTx, uint16_t USART_FLAG);    stm32f10x_usart.h 391行 
 //1.判断上一次是否发送完成   复位第一次为什么满足条件：因为该位复位后默认值是1   
 while(USART_GetFlagStatus(UART4,USART_FLAG_TC)==RESET) 
 {//2.还未发送完成   USART1->SR寄存器的位6是0 
  
 } 
 //3.上一次数据发送完成了，发送新的数据 
//void USART_SendData(USART_TypeDef* USARTx, uint16_t Data);  stm3210x_usart1.h  378行 
 USART_SendData(UART4,Byte); 
 //4.清除标志位  参考手册25.6.1的位6描述 清除标志位两种方法 
 //(1)写0清0  (2)软件序列清除该位(先读USART_SR，然后写入USART_DR) 
// USART_ClearFlag(USART1,USART_FLAG_TC); 
} 
 
 
//发送数组 
//参数1  数组的首地址 
//参数2  待发送数组的长度 
void UART4_SendBuff(uint8_t *Buff , uint16_t Length) 
{//注意 Length和i的类型 
 for(uint16_t i=0;i<Length;i++) 
 { 
  //UART4_SendByte(Buff[i]);   //循环发送 
  UART4_SendByte(*Buff++);   
 } 
} 
 
void UART4_IRQHandler(void) 
{ 
 uint8_t Data; 
//ITStatus USART_GetITStatus(USART_TypeDef* USARTx, uint16_t USART_IT);  stm32f10x_usart1.h 392行 
//void USART_ClearITPendingBit(USART_TypeDef* USARTx, uint16_t USART_IT); 
 //1.判断接收中断是否触发  SR寄存器的位5是否为1 
 if(USART_GetITStatus(UART4,USART_IT_RXNE)==SET) 
 { 
  //2.已经接收到数据，读取接收到的数据 
  Data = USART_ReceiveData(UART4); 
  Su03t.R_Buff[Su03t.R_Cnt++]=Data; 
  //3.清除SR寄存器的位5 根据参考手册 25.6.1的位5描述，清除方法有两种     
  //(1)对USART_DR的读操作可以将该位清零  (2)RXNE位也可以通过写入0来清除 
// USART_ClearITPendingBit(UART4,USART_IT_RXNE); 
  if(Su03t.R_Cnt>=SU03T_R_Buff_Length) 
  {//避免数组越界 
   Su03t.R_Cnt=0; 
  } 
 } 
 //1.判断空闲中断是否触发  SR寄存器的位4是否为1 
 if(USART_GetITStatus(UART4,USART_IT_IDLE)==SET) 
 { 
  //2.执行中断服务内容 
//  Su03t.R_Idle=1;   //空闲标志位置1 
	 xEventGroupSetBitsFromISR(xCreatedEventGroup,BIT_0,NULL);
//	 xEventGroupSetBits(xCreatedEventGroup,BIT_0); 
  //3.清除SR寄存器的位4 根据参考手册 25.6.1的位4描述，清除方法只能通过软件序列清除 
  //软件序列清除：先读USART_SR，然后读USART_DR 
  Data = USART_ReceiveData(UART4);  //只是为了清除标志位 
  Data = Data;    //防止编译器报警告 
 } 
} 
 
//清除接收缓冲区 
void SU03T_Clear_R_Buff(void) 
{ 
 memset(Su03t.R_Buff,0,SU03T_R_Buff_Length); 
 Su03t.R_Cnt=0; 
} 
 


// 串口通信消息头
const unsigned char g_uart_send_head[] = {
  0xaa, 0x55
};

// 串口通信消息尾
const unsigned char g_uart_send_foot[] = {
  0x55, 0xaa
};

// 串口发送函数实现
void _uart_send_impl(unsigned char* buff, int len) {
  // TODO: 调用项目实际的串口发送函数
  /*
  int i = 0;
  unsigned char c;
  for (i = 0; i < len; i++) {
    c = buff[i];
    printf("%02X ", c);
  }
  printf("\n");
  */
	UART4_SendBuff(buff,len);
}
// 十六位整数转32位整数
void _int16_to_int32(uart_param_t* param) {
  if (sizeof(int) >= 4)
    return;
  unsigned long value = param->d_long;
  unsigned long sign = (value >> 15) & 1;
  unsigned long v = value;
  if (sign)
    v = 0xFFFF0000 | value;
  uart_param_t p;
  p.d_long = v;
  param->d_ucs[0] = p.d_ucs[0];
  param->d_ucs[1] = p.d_ucs[1];
  param->d_ucs[2] = p.d_ucs[2];
  param->d_ucs[3] = p.d_ucs[3];
}

// 浮点数转双精度 
void _float_to_double(uart_param_t* param) {
  if (sizeof(int) >= 4)
    return;
  unsigned long value = param->d_long;
  unsigned long sign = value >> 31;
  unsigned long M = value & 0x007FFFFF;
  unsigned long e =  ((value >> 23 ) & 0xFF) - 127 + 1023;
  uart_param_t p0, p1;  
  p1.d_long = ((sign & 1) << 31) | ((e & 0x7FF) << 20) | (M >> 3);
  p0.d_long = (M & 0x7) << 29;
  param->d_ucs[0] = p0.d_ucs[0];
  param->d_ucs[1] = p0.d_ucs[1];
  param->d_ucs[2] = p0.d_ucs[2];
  param->d_ucs[3] = p0.d_ucs[3];
  param->d_ucs[4] = p1.d_ucs[0];
  param->d_ucs[5] = p1.d_ucs[1];
  param->d_ucs[6] = p1.d_ucs[2];
  param->d_ucs[7] = p1.d_ucs[3];
}
// action: PlayVoc
void _uart_PlayVoc(double VOC) {
  uart_param_t param;
  int i = 0;
  unsigned char buff[UART_SEND_MAX] = {0};
  for (i = 0; i < UART_MSG_HEAD_LEN; i++) {
      buff[i + 0] = g_uart_send_head[i];
  }
  buff[2] = U_MSG_PlayVoc;
  param.d_double = VOC;
  _float_to_double(&param);
  buff[3] = param.d_ucs[0];
  buff[4] = param.d_ucs[1];
  buff[5] = param.d_ucs[2];
  buff[6] = param.d_ucs[3];
  buff[7] = param.d_ucs[4];
  buff[8] = param.d_ucs[5];
  buff[9] = param.d_ucs[6];
  buff[10] = param.d_ucs[7];
  for (i = 0; i < UART_MSG_FOOT_LEN; i++) {
      buff[i + 11] = g_uart_send_foot[i];
  }
  _uart_send_impl(buff, 13);
}

// action: PlayCh20
void _uart_PlayCh20(double CH20) {
  uart_param_t param;
  int i = 0;
  unsigned char buff[UART_SEND_MAX] = {0};
  for (i = 0; i < UART_MSG_HEAD_LEN; i++) {
      buff[i + 0] = g_uart_send_head[i];
  }
  buff[2] = U_MSG_PlayCh20;
  param.d_double = CH20;
  _float_to_double(&param);
  buff[3] = param.d_ucs[0];
  buff[4] = param.d_ucs[1];
  buff[5] = param.d_ucs[2];
  buff[6] = param.d_ucs[3];
  buff[7] = param.d_ucs[4];
  buff[8] = param.d_ucs[5];
  buff[9] = param.d_ucs[6];
  buff[10] = param.d_ucs[7];
  for (i = 0; i < UART_MSG_FOOT_LEN; i++) {
      buff[i + 11] = g_uart_send_foot[i];
  }
  _uart_send_impl(buff, 13);
}

// action: PlayCo2
void _uart_PlayCo2(int CO2) {
  uart_param_t param;
  int i = 0;
  unsigned char buff[UART_SEND_MAX] = {0};
  for (i = 0; i < UART_MSG_HEAD_LEN; i++) {
      buff[i + 0] = g_uart_send_head[i];
  }
  buff[2] = U_MSG_PlayCo2;
  param.d_int = CO2;
  _int16_to_int32(&param);
  buff[3] = param.d_ucs[0];
  buff[4] = param.d_ucs[1];
  buff[5] = param.d_ucs[2];
  buff[6] = param.d_ucs[3];
  for (i = 0; i < UART_MSG_FOOT_LEN; i++) {
      buff[i + 7] = g_uart_send_foot[i];
  }
  _uart_send_impl(buff, 9);
}

// action: PlayTime
void _uart_PlayTime(char Hour, char Min) {
  uart_param_t param;
  int i = 0;
  unsigned char buff[UART_SEND_MAX] = {0};
  for (i = 0; i < UART_MSG_HEAD_LEN; i++) {
      buff[i + 0] = g_uart_send_head[i];
  }
  buff[2] = U_MSG_PlayTime;
  param.d_char = Hour;
  buff[3] = param.d_uchar;
  param.d_char = Min;
  buff[4] = param.d_uchar;
  for (i = 0; i < UART_MSG_FOOT_LEN; i++) {
      buff[i + 5] = g_uart_send_foot[i];
  }
  _uart_send_impl(buff, 7);
}

// action: PlayWendu
void _uart_PlayWendu(double Wendu) {
  uart_param_t param;
  int i = 0;
  unsigned char buff[UART_SEND_MAX] = {0};
  for (i = 0; i < UART_MSG_HEAD_LEN; i++) {
      buff[i + 0] = g_uart_send_head[i];
  }
  buff[2] = U_MSG_PlayWendu;
  param.d_double = Wendu;
  _float_to_double(&param);
  buff[3] = param.d_ucs[0];
  buff[4] = param.d_ucs[1];
  buff[5] = param.d_ucs[2];
  buff[6] = param.d_ucs[3];
  buff[7] = param.d_ucs[4];
  buff[8] = param.d_ucs[5];
  buff[9] = param.d_ucs[6];
  buff[10] = param.d_ucs[7];
  for (i = 0; i < UART_MSG_FOOT_LEN; i++) {
      buff[i + 11] = g_uart_send_foot[i];
  }
  _uart_send_impl(buff, 13);
}

// action: PlayShidu
void _uart_PlayShidu(double Shidu) {
  uart_param_t param;
  int i = 0;
  unsigned char buff[UART_SEND_MAX] = {0};
  for (i = 0; i < UART_MSG_HEAD_LEN; i++) {
      buff[i + 0] = g_uart_send_head[i];
  }
  buff[2] = U_MSG_PlayShidu;
  param.d_double = Shidu;
  _float_to_double(&param);
  buff[3] = param.d_ucs[0];
  buff[4] = param.d_ucs[1];
  buff[5] = param.d_ucs[2];
  buff[6] = param.d_ucs[3];
  buff[7] = param.d_ucs[4];
  buff[8] = param.d_ucs[5];
  buff[9] = param.d_ucs[6];
  buff[10] = param.d_ucs[7];
  for (i = 0; i < UART_MSG_FOOT_LEN; i++) {
      buff[i + 11] = g_uart_send_foot[i];
  }
  _uart_send_impl(buff, 13);
}

// action: PlayLightInten
void _uart_PlayLightInten(double LUX) {
  uart_param_t param;
  int i = 0;
  unsigned char buff[UART_SEND_MAX] = {0};
  for (i = 0; i < UART_MSG_HEAD_LEN; i++) {
      buff[i + 0] = g_uart_send_head[i];
  }
  buff[2] = U_MSG_PlayLightInten;
  param.d_double = LUX;
  _float_to_double(&param);
  buff[3] = param.d_ucs[0];
  buff[4] = param.d_ucs[1];
  buff[5] = param.d_ucs[2];
  buff[6] = param.d_ucs[3];
  buff[7] = param.d_ucs[4];
  buff[8] = param.d_ucs[5];
  buff[9] = param.d_ucs[6];
  buff[10] = param.d_ucs[7];
  for (i = 0; i < UART_MSG_FOOT_LEN; i++) {
      buff[i + 11] = g_uart_send_foot[i];
  }
  _uart_send_impl(buff, 13);
}

// action: PlayMachine
void _uart_PlayMachine(int Which_Machine) {
  uart_param_t param;
  int i = 0;
  unsigned char buff[UART_SEND_MAX] = {0};
  for (i = 0; i < UART_MSG_HEAD_LEN; i++) {
      buff[i + 0] = g_uart_send_head[i];
  }
  buff[2] = U_MSG_PlayMachine;
  param.d_int = Which_Machine;
  _int16_to_int32(&param);
  buff[3] = param.d_ucs[0];
  buff[4] = param.d_ucs[1];
  buff[5] = param.d_ucs[2];
  buff[6] = param.d_ucs[3];
  for (i = 0; i < UART_MSG_FOOT_LEN; i++) {
      buff[i + 7] = g_uart_send_foot[i];
  }
  _uart_send_impl(buff, 9);
}

// action: PlayMachine1
void _uart_PlayMachine1() {
  uart_param_t param;
  int i = 0;
  unsigned char buff[UART_SEND_MAX] = {0};
  for (i = 0; i < UART_MSG_HEAD_LEN; i++) {
      buff[i + 0] = g_uart_send_head[i];
  }
  buff[2] = U_MSG_PlayMachine1;
  for (i = 0; i < UART_MSG_FOOT_LEN; i++) {
      buff[i + 3] = g_uart_send_foot[i];
  }
  _uart_send_impl(buff, 5);
}

// action: PlayMachine2
void _uart_PlayMachine2() {
  uart_param_t param;
  int i = 0;
  unsigned char buff[UART_SEND_MAX] = {0};
  for (i = 0; i < UART_MSG_HEAD_LEN; i++) {
      buff[i + 0] = g_uart_send_head[i];
  }
  buff[2] = U_MSG_PlayMachine2;
  for (i = 0; i < UART_MSG_FOOT_LEN; i++) {
      buff[i + 3] = g_uart_send_foot[i];
  }
  _uart_send_impl(buff, 5);
}

// action: PlayMachine3
void _uart_PlayMachine3() {
  uart_param_t param;
  int i = 0;
  unsigned char buff[UART_SEND_MAX] = {0};
  for (i = 0; i < UART_MSG_HEAD_LEN; i++) {
      buff[i + 0] = g_uart_send_head[i];
  }
  buff[2] = U_MSG_PlayMachine3;
  for (i = 0; i < UART_MSG_FOOT_LEN; i++) {
      buff[i + 3] = g_uart_send_foot[i];
  }
  _uart_send_impl(buff, 5);
}



//SU03T的处理函数 
void SU03T_Handle(void) 
{ 
  for (int i=0;i<Su03t.R_Cnt;i++) printf("%02X ", Su03t.R_Buff[i]);
    printf("\r\n");
 if(Su03t.R_Buff[0]==0xAA&&Su03t.R_Buff[1]==0x55&&Su03t.R_Buff[4]==0x55&&Su03t.R_Buff[5]==0xAA) 
  { 
   switch(Su03t.R_Buff[3]) 
   { 
		 case 0:
		 break;
    case 1: 
     //唤醒
     break; 
    case 2: 
     LED1_ON();     
     break;
    case 3: 
     LED1_OFF();     
     break;		
    case 4: 
     _uart_PlayVoc(Sensor.VOC); 
     break; 
    case 5: 
     _uart_PlayCh20(Sensor.CH2O); 
     break;   
    case 6: 
     _uart_PlayCo2(Sensor.CO2);      
     break;  
    case 7: 
     _uart_PlayTime(Now_Time->tm_hour,Now_Time->tm_min); 
     break; 
    case 8:
     REV=66;			
     Machine_forward();     
     break;
    case 9:
     REV=0;			
     Machine_Stop(); 
		break;
    case 10:		
    _uart_PlayWendu(Dht.Tem);    
     break;
    case 11:
    _uart_PlayShidu(Dht.Hum);    
     break;
		case 12: 
     RELAY_ON();     
     break;
    case 13: 
     RELAY_OFF();     
     break;
    case 0x0E:
    //光照强度
		_uart_PlayLightInten(LightInten);
     break;
		case 0x0F: 
		 REV=33;
     TIM_SetCompare3(TIM4, REV);     
     break;
    case 0x10:
     REV=66;			
     TIM_SetCompare3(TIM4, REV);		
     break;
		case 0x11:
		 REV=100;
     TIM_SetCompare3(TIM4, REV);    
     break;
		case 0x12: 
     //当前风扇几档 
     _uart_PlayMachine(REV/33);		
     break;
    case 0x13: 
     //风扇快一点
     switch(REV)
		 {
			 case 0:
				 //还未开启风扇呢
			 break;
			 case 33:
				 REV=66;			
         TIM_SetCompare3(TIM4, REV);
			   _uart_PlayMachine1();
			   break;
			 case 66:
				 REV=100;			
         TIM_SetCompare3(TIM4, REV);
			   _uart_PlayMachine1();
			   break;
			 case 100:
         //播报最高档
			   _uart_PlayMachine2();
         break;			 
		 }			 
     break;	
    case 0x14: 
     //风扇慢一点 
    switch(REV)
		 {
			 case 33:
				 //播报最低档
			   _uart_PlayMachine3();
         break;
			 case 66:
				 REV=33;			
         TIM_SetCompare3(TIM4, REV);
			   _uart_PlayMachine1();
			   break;
			 case 100:
         REV=66;			
         TIM_SetCompare3(TIM4, REV);
			   _uart_PlayMachine1();
			   break;			 
		 }	    
     break;
    default:     
     break;      
   } 
  } 
  else 
  { 
   printf("SU03T的帧头帧尾错误\r\n"); 
  } 
  SU03T_Clear_R_Buff(); //清空接收缓冲区 
  
// SU03T_Clear_R_Buff();  //千万不能在这里调用 
} 
 
