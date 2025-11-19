#include "relay.h"
#include "main.h"
//继电器初始化
void RELAY_Config()
{
	#if (USE_STD_LIB==0)
  RCC->APB2ENR |=(0x01<<2);    //开启时钟A；
	GPIOA->CRH &= ~(0x0F<<12);   //清空
	GPIOA->CRH |= (0x03<<12);    //通用推挽输出，50Mhz
	#elif (USE_STD_LIB==1)
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);  //开启时钟A
	GPIO_InitTypeDef GPIO_InitStruct={0};  //定义结构体
	//给结构体赋初值
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_Out_PP;  //通用推挽输出
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_11;        //PA11
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_2MHz;  //Hz
	//调用XXX_Init函数，将函数写入到寄存器中
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	#endif
}
//开启继电器
void RELAY_ON()
{ 
  #if (USE_STD_LIB==0)
	GPIOA->ODR |= (0x01<<11); 
  #elif (USE_STD_LIB==1)
  GPIO_SetBits(GPIOA,GPIO_Pin_11);
  #endif  
}
//关闭继电器
void RELAY_OFF()
{
	#if (USE_STD_LIB==0)
  GPIOA->ODR &= ~(0x01<<11);
	#elif (USE_STD_LIB==1)
	GPIO_ResetBits(GPIOA,GPIO_Pin_11);
	#endif
}
void RELAY_TOGGLE()
{
	GPIOA->ODR ^= (0x01<<11);
	//GPIO_WriteBit(GPIOA,GPIO_Pin_11,(BitAction)(1-(GPIO_ReadOutputDataBit(GPIOA,GPIO_Pin_11))));
}
