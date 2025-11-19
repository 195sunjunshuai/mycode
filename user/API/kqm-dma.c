#include "kqm-dma.h"
#include "stdio.h"
/*
  串口2 PA2 PA3
	尝试通过 UART2_RX+DMA 解析一下 KQM6600 的数据
	配置：1.GPIO 2.DMA 3.USART2
	GPIO PA2 复用推挽输出
	     PA3 浮空输入
*/
#define PerAddr ((uint32_t)0x40004404)
typedef struct 
{
	uint8_t KqmDma_BUFF[8];
	double KqmDma_VOC;
	double KqmDma_CH2O;
	uint32_t KqmDma_CO2;
	uint8_t data_ready;
}KQM_DMA;
KQM_DMA kqm_dma;
void USART2_RX_DMA_Config()
{
	//1.GPIO
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct={0};
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_3;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_2;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	//2.DMA
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	DMA_InitTypeDef DMA_InitStruct={0};
	DMA_InitStruct.DMA_DIR=DMA_DIR_PeripheralSRC;
	DMA_InitStruct.DMA_PeripheralBaseAddr=PerAddr;
	DMA_InitStruct.DMA_MemoryBaseAddr=(uint32_t)kqm_dma.KqmDma_BUFF;
	DMA_InitStruct.DMA_BufferSize=8;
	DMA_InitStruct.DMA_PeripheralInc=DMA_PeripheralInc_Disable;
	DMA_InitStruct.DMA_MemoryInc=DMA_MemoryInc_Enable;
	DMA_InitStruct.DMA_PeripheralDataSize=DMA_PeripheralDataSize_Byte;
	DMA_InitStruct.DMA_MemoryDataSize=DMA_MemoryDataSize_Byte;
	DMA_InitStruct.DMA_Mode=DMA_Mode_Circular;
	DMA_InitStruct.DMA_Priority=DMA_Priority_VeryHigh;
	DMA_InitStruct.DMA_M2M=DMA_M2M_Disable;
	DMA_Init(DMA1_Channel6,&DMA_InitStruct);

	DMA_ITConfig(DMA1_Channel6,DMA_IT_TC,ENABLE);
	//3.USART2
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
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
	 
	/* 5) NVIC 配置 DMA1_Channel6 中断 */
    NVIC_InitTypeDef nvic = {0};
    nvic.NVIC_IRQChannel = DMA1_Channel6_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority = 1;
    nvic.NVIC_IRQChannelSubPriority = 1;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);
	
  USART_Cmd(USART2,ENABLE); 
	 
  USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE);
	kqm_dma.data_ready=0;
	DMA_SetCurrDataCounter(DMA1_Channel6,8);
	DMA_Cmd(DMA1_Channel6,ENABLE);
}
void DMA1_Channel6_IRQHandler(void)
{
    if (DMA_GetITStatus(DMA1_IT_TC6) != RESET)
    {
        DMA_ClearITPendingBit(DMA1_IT_TC6);
        DMA_Cmd(DMA1_Channel6, DISABLE);   // 先关，防止写入

        /* 标记一帧就绪，让主循环里解析 */
        kqm_dma.data_ready = 1;

        /* 重新装载，准备接下一帧 */
        DMA_SetCurrDataCounter(DMA1_Channel6, 8);
        DMA_Cmd(DMA1_Channel6, ENABLE);
    }
}
void KQM_DMA_Handle()
{
	if(kqm_dma.data_ready==1)
	{
		kqm_dma.data_ready=0;
		uint8_t Data_CS=0; 
		//自己计算校验，检测校验是否通过 
		Data_CS=kqm_dma.KqmDma_BUFF[0]+kqm_dma.KqmDma_BUFF[1]+kqm_dma.KqmDma_BUFF[2]+
		kqm_dma.KqmDma_BUFF[3]+kqm_dma.KqmDma_BUFF[4]+kqm_dma.KqmDma_BUFF[5]+kqm_dma.KqmDma_BUFF[6]; 
		if(Data_CS==kqm_dma.KqmDma_BUFF[7]) 
		{//校验通过 
		 if(kqm_dma.KqmDma_BUFF[0]==0x5F) 
		 { 
			kqm_dma.KqmDma_VOC= ((kqm_dma.KqmDma_BUFF[1]<<8)+kqm_dma.KqmDma_BUFF[2])*0.1;  
			kqm_dma.KqmDma_CH2O = ((kqm_dma.KqmDma_BUFF[3]<<8)+kqm_dma.KqmDma_BUFF[4])*0.01; 
			kqm_dma.KqmDma_CO2 = (kqm_dma.KqmDma_BUFF[5]<<8)+kqm_dma.KqmDma_BUFF[6]; 
			printf("VOC=%.1f,CH20=%.2f,CO2=%d\r\n",kqm_dma.KqmDma_VOC,kqm_dma.KqmDma_CH2O ,kqm_dma.KqmDma_CO2); 
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
  }		
}