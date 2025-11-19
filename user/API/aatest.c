#include "aatest.h"
#include "main.h"
/*
  PB5  TX
*/
void TEST_Config()
{
	//1.GPIO
	//1.1时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	//1.2.结构体
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_5;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_2MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
  GPIO_SetBits(GPIOB,GPIO_Pin_5|GPIO_Pin_6);
}
void Send_Byte_Self(uint8_t data)
{
	uint8_t i;
	uint8_t byte;
	//起始位
	GPIO_ResetBits(GPIOB,GPIO_Pin_5);
	Delay_1us(104);
	//数据位
	for(i=0;i<8;i++)
	{
		byte=data & (0x01);
		GPIO_WriteBit(GPIOB,GPIO_Pin_5,byte);
		Delay_1us(104);
		data=data>>1;
	}
	//停止位
	GPIO_SetBits(GPIOB,GPIO_Pin_5);
	Delay_1us(104);
}
void Send_Str_Self(char *str)
{
	while(*str)
	{
		Send_Byte_Self(*str++);
	}
}



