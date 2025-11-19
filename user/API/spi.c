#include "spi.h" 
 
//SPI的初始化 
/*分为  GPIO   SPI   NVIC(可有可无) 
  STM32的引脚   FLASH的引脚        GPIO口模式 
PB12             FLASH_CS         通用推挽输出 
  看官方例程,SPI1_NSS根本没用,而是随便一个IO作为片选引脚并且配置通用推挽输出 
PB13   SPI_CLK   FLASH_CLK        复用推挽输出 
PB14   SPI_MISO  FLASH_MISO(DO)   浮空输入 
PB15   SPI_MOSI  FLASH_MOSI(DI)   复用推挽输出 
*/ 
void SPI_Config(void) 
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
 GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP; //通用推挽输出   中文固件库翻译手册  Table 185 
 GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12; //待设置的引脚 中文固件库翻译手册  Table 183 
 GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;  //GPIO口的速率  中文固件库翻译手册  Table 184 
  
 //4.调用XXX_Init函数，将参数写入到寄存器中 
 GPIO_Init(GPIOB,&GPIO_InitStruct);  
  
 GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出   中文固件库翻译手册  Table 185 
 GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_15; //待设置的引脚 中文固件库翻译手册  Table 183 
 
 GPIO_Init(GPIOB,&GPIO_InitStruct);   
  
 GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING; //复用推挽输出   中文固件库翻译手册  Table 185 
 GPIO_InitStruct.GPIO_Pin = GPIO_Pin_14; //待设置的引脚 中文固件库翻译手册  Table 183 
 
 GPIO_Init(GPIOB,&GPIO_InitStruct); 
 //5.开启SPI的时钟 
//void RCC_APB2PeriphClockCmd(uint32_t RCC_APB2Periph, FunctionalState NewState); stm32f10x_rcc.h 693行 
//void RCC_APB1PeriphClockCmd(uint32_t RCC_APB1Periph, FunctionalState NewState);  
 RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE); 
  
 //6.定义结构体 
//void SPI_Init(SPI_TypeDef* SPIx, SPI_InitTypeDef* SPI_InitStruct);  stm32f0x_spi.h 447行 
  SPI_InitTypeDef SPI_InitStruct={0}; 
  
 //7.给结构体赋值 
 SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;//SPI的速率  根据从设备确定  W25Q64手册中文 标准SPI支持80M 
  //SPI2挂载APB1(36M),36M/2=18M<80M 
 SPI_InitStruct.SPI_CPHA = SPI_CPHA_2Edge;//时钟相位  根据从设备确定 W25Q64手册中文描述支持SPI0和SPI3 
 SPI_InitStruct.SPI_CPOL = SPI_CPOL_High;//时钟极性  根据从设备确定 W25Q64手册中文描述支持SPI0和SPI3 
// SPI_InitStruct.SPI_CRCPolynomial //CRC校验,我们不使用CRC校验 
 SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;//数据位宽度  根据从设备确定  W25Q64手册中文描述支持SPI0和SPI3 
 SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//数据传输的方向   双线双向全双工  根据从设备确定 W25Q64手册中文描述支持SPI0和SPI3 
 SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;//高/低位先发   根据从设备确定 W25Q64手册中文描述支持SPI0和SPI3 
 SPI_InitStruct.SPI_Mode = SPI_Mode_Master; //主机模式  
 SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;//软件模式   NSS引脚作为普通IO口,结合官方例程确定 
  
 //8.调用XXX_Init函数，将参数写入到寄存器中 
 SPI_Init(SPI2,&SPI_InitStruct); 
  
 //9.将SPI使能 
//void SPI_Cmd(SPI_TypeDef* SPIx, FunctionalState NewState);  
 SPI_Cmd(SPI2,ENABLE); 
  
 //10.把片选拉高，避免W25Q64处于被选中状态 
 GPIO_SetBits(GPIOB,GPIO_Pin_12); 
} 
 
//单字节的发送和接收函数 
//参数   要发送的数据 
uint8_t SPI_Send_Rec_Byte(uint8_t Byte) 
{ 
 uint8_t data; 
 //1.检测STM32上一次是否发完 
//FlagStatus SPI_I2S_GetFlagStatus(SPI_TypeDef* SPIx, uint16_t SPI_I2S_FLAG); 
//void SPI_I2S_ClearFlag(SPI_TypeDef* SPIx, uint16_t SPI_I2S_FLAG); 
 while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_TXE)==RESET) 
 {//STM32上一次还未发送完成，等待 
 } 
 //2.STM32上一次发送完成，STM32发送新的数据 
//void SPI_I2S_SendData(SPI_TypeDef* SPIx, uint16_t Data); 
 SPI_I2S_SendData(SPI2,Byte); 
 //3.检测STM32(STM32给从机发送数据的同时，从机也在给主机发送数据)接收是否完成 
 while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_RXNE)==RESET) 
 {//STM32还未接收完成，等待 
 }  
 //4.STM32读取接收到的数据，并返回 
//uint16_t SPI_I2S_ReceiveData((SPI_TypeDef* SPIx);  
 data=SPI_I2S_ReceiveData(SPI2); 
 //5.清除标志位  
 //参数手册 23.3.3描述，读SPI_DR寄存器将清除RXNE位 
 return data; 
} 