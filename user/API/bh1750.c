#include "bh1750.h" 
#include "soft_iic.h" 
#include "hard_iic.h" 
#include "stdio.h" 
#include "uart1.h" 
double LightInten=0; 
//BH1750的初始化 
void BH1750_Config(void) 
{ 
#if (BH1750_IIC_TYPE==0) 
 SOFT_IIC_Config(); 
#elif (BH1750_IIC_TYPE==1) 
 HARD_IIC_Config(); 
#endif 
 BH1750_SendCmd(0x01);  //通电指令 
 BH1750_SendCmd(0x10);  //连续高分辨率模式测量指令  
 
} 
//发送命令  按照BH1750的手册进行拼接 
void BH1750_SendCmd(uint8_t Cmd) 
{ 
#if (BH1750_IIC_TYPE==0)  
 SOFT_IIC_START(); 
 SOFT_IIC_SendByte(BH1750_W); 
 SOFT_IIC_RecACK(); 
 SOFT_IIC_SendByte(Cmd); 
 SOFT_IIC_RecACK();  
 SOFT_IIC_STOP(); 
#elif (BH1750_IIC_TYPE==1)  
 //按照参考手册  24.3.3的主发送器的图245编程 
 //1.发送起始信号 
//void I2C_GenerateSTART(I2C_TypeDef* I2Cx, FunctionalState NewState); 
 I2C_GenerateSTART(I2C1,ENABLE); 
 //2.检测EV5标志位 
//ErrorStatus I2C_CheckEvent(I2C_TypeDef* I2Cx, uint32_t I2C_EVENT); 
 while(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT)!=SUCCESS); 
 //3.发送7位的器件地址加读写位  第二参数是7位器件地址左移1位
//void I2C_Send7bitAddress(I2C_TypeDef* I2Cx, uint8_t Address, uint8_t I2C_Direction); 
 I2C_Send7bitAddress(I2C1,BH1750_ADRR<<1,I2C_Direction_Transmitter); 
 //4.检测EV6标志位  第二个参数是发送模式 
//ErrorStatus I2C_CheckEvent(I2C_TypeDef* I2Cx, uint32_t I2C_EVENT); 
  while(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)!=SUCCESS); 
 //5.发送数据 
//void I2C_SendData(I2C_TypeDef* I2Cx, uint8_t Data);  
  I2C_SendData(I2C1,Cmd); 
 //6.检测EV8_2标志位  
 while(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED)!=SUCCESS); 
 //7.发送停止信号 
//void I2C_GenerateSTOP(I2C_TypeDef* I2Cx, FunctionalState NewState);  
 I2C_GenerateSTOP(I2C1,ENABLE); 
#endif  
} 
 
//获取测量结果  按照BH1750的手册进行拼接 
float BH1750_Handle(void) 
{ 
 uint8_t Buff[2]; 
#if (BH1750_IIC_TYPE==0)   
 SOFT_IIC_START();  
 SOFT_IIC_SendByte(BH1750_R); 
 SOFT_IIC_RecACK();  
 Buff[0]=SOFT_IIC_RecByte(); 
 SOFT_IIC_SendACK(1); 
 Buff[1]=SOFT_IIC_RecByte(); 
 SOFT_IIC_SendACK(0);  
 SOFT_IIC_STOP();  
#elif (BH1750_IIC_TYPE==1)  
 //按照参考手册  24.3.4的主发送器的图246编程  
 //1.发送起始信号 
//void I2C_GenerateSTART(I2C_TypeDef* I2Cx, FunctionalState NewState); 
 I2C_GenerateSTART(I2C1,ENABLE); 
 //2.检测EV5标志位 
//ErrorStatus I2C_CheckEvent(I2C_TypeDef* I2Cx, uint32_t I2C_EVENT); 
 while(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT)!=SUCCESS); 
 //3.发送7位的器件地址加读写位  第二参数是7位器件地址左移1位 
//void I2C_Send7bitAddress(I2C_TypeDef* I2Cx, uint8_t Address, uint8_t I2C_Direction); 
 I2C_Send7bitAddress(I2C1,BH1750_ADRR<<1,I2C_Direction_Receiver); 
 //4.检测EV6标志位  第二个参数是接收模式 
//ErrorStatus I2C_CheckEvent(I2C_TypeDef* I2Cx, uint32_t I2C_EVENT); 
  while(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)!=SUCCESS); 
 //5.检测EV7标志位，确定数据是否到来 
  while(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_RECEIVED)!=SUCCESS); 
 //6.已经接收到数据，将数据读取出来
//uint8_t I2C_ReceiveData(I2C_TypeDef* I2Cx);  
 Buff[0] = I2C_ReceiveData(I2C1); 
//void I2C_AcknowledgeConfig(I2C_TypeDef* I2Cx, FunctionalState NewState);  
  //7.收到倒数第二个字节之后，关闭自动应答   图245上面的关闭通信的语音描述中 
 I2C_AcknowledgeConfig(I2C1,DISABLE); 
 //8.收到倒数第二个字节之后，使能停止信号   图245上面的关闭通信的语音描述中 
//void I2C_GenerateSTOP(I2C_TypeDef* I2Cx, FunctionalState NewState);  
 I2C_GenerateSTOP(I2C1,ENABLE); 
 //9.再次检测EV7标志位是否收到新的数据，接收最后1个 
  while(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_RECEIVED)!=SUCCESS);  
 Buff[1] = I2C_ReceiveData(I2C1);  
 //10.为了方便后续的通信，开启自动应答 
 I2C_AcknowledgeConfig(I2C1,ENABLE);  
#endif 
 LightInten=((Buff[0]<<8)+Buff[1])/1.2;
// safe_printf("采集的光照:%.1f\r\n",((Buff[0]<<8)+Buff[1])/1.2); 
}