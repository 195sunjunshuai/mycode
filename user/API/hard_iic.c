#include "hard_iic.h" 
//硬件IIC 的初始化 
/* 
主要分为两大部分   
GPIO  通用开漏模式   1.参考手册8.1.11   2.官方例程 
IIC 
*/ 
void HARD_IIC_Config(void) 
{ 
 //1.开时钟      
/*中文固件库翻译手册  RCC_APB2PeriphClockCmd  15.2.22 
 stm32f10x_rcc.h  693行*/  
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE); //中文固件库翻译手册  Table 373 
  
 //2.定义结构体,目的是给XXX_Init函数准备参数 
//void GPIO_Init(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef* GPIO_InitStruct)  
/*中文固件库翻译手册  GPIO_Init  10.2.3 
 stm32f10x_gpio.h  351行*/ 
 GPIO_InitTypeDef GPIO_InitStruct={0};   //变量名无所谓 
  
 //3.给结构体赋值 
 GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_OD; //复用开漏输出   中文固件库翻译手册  Table 185 
 GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7; //待设置的引脚 中文固件库翻译手册  Table 183 
 GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;  //GPIO口的速率  中文固件库翻译手册  Table 184 
  
 //4.调用XXX_Init函数，将参数写入到寄存器中 
 GPIO_Init(GPIOB,&GPIO_InitStruct);   
  
 //5.开启IIC的时钟 
 RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE); //中文固件库翻译手册  Table 373 
  
 //6.定义IIC的结构体 
//void I2C_Init(I2C_TypeDef* I2Cx, I2C_InitTypeDef* I2C_InitStruct);  stm32f10x_iic.h 535行 
 I2C_InitTypeDef I2C_InitStruct={0}; 
  
 //7.给结构体赋值 
 I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;//是否开启自动应答 
 I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit; //7位地址 
 I2C_InitStruct.I2C_ClockSpeed = 100000;   //时钟频率100KHZ  BH1750手册描述，最大400K 
 I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_16_9; //参数快速模式才有用 
 I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;  //IIC模式 
 I2C_InitStruct.I2C_OwnAddress1 = 0;  //自身地址   作为从机才有用 
  
 //8.将参数写入到寄存器中 
 I2C_Init(I2C1,&I2C_InitStruct); 
  
 //9.把外设使能 
//void I2C_Cmd(I2C_TypeDef* I2Cx, FunctionalState NewState);   stm32f10x_iic.h 537行 
 I2C_Cmd(I2C1,ENABLE); 
} 
