#include "soft_iic.h" 
#include "delay.h" 
//模拟IIC(软件 IIC)  -- 手搓  -- IO口配置成通用开漏模式 
/*PB6  SCL 
  PB7  SDA 
*/ 
void SOFT_IIC_Config(void) 
{ 
//中文固件库翻译手册路径：授课大纲资料\1.项目分析与环境搭建\资料\芯片相关资料\STM32固件库使用手册的中文翻译版 
//1.开 GPIOB 的时钟      STM32 固件库使用手册的中文翻译版的15.2.22 
RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE); 
//2.定义结构体    参考STM32固件库使用手册的中文翻译版的10.2.3 
//目的：给XXX_Init函数准备参数，XXX_Init函数需要传递这个类型的变量地址 
GPIO_InitTypeDef GPIO_InitStructure={0}; 
//3.给结构体赋值  参考STM32固件库使用手册的中文翻译版的10.2.3  
GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7; //待设置的引脚 
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;  //引脚模式 通用开漏输出 
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;  //引脚输出速率 
  
 //4.调用XXX_Init将结构体的参数写入到寄存器中 
 GPIO_Init(GPIOB, &GPIO_InitStructure);  
  
 //5.释放一下总线 
 GPIO_SetBits(GPIOB, GPIO_Pin_6|GPIO_Pin_7);     
} 
 
//控制SCL的高低电平 
void SOFT_IIC_W_SCL(uint8_t BitValue) 
{ 
 GPIO_WriteBit(GPIOB, GPIO_Pin_6,(BitAction)BitValue);  
 Delay_us(10); 
} 
 
//控制SDA的高低电平 
void SOFT_IIC_W_SDA(uint8_t BitValue) 
{ 
 GPIO_WriteBit(GPIOB, GPIO_Pin_7,(BitAction)BitValue);  
 Delay_us(10); 
} 
 
//读取SDA的高低电平 
uint8_t SOFT_IIC_R_SDA(void) 
{ 
 uint8_t BitValue=0; 
 BitValue=GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7); 
 Delay_us(10); 
 return BitValue; 
} 
 
//起始信号 
void SOFT_IIC_START(void) 
{ 
 //进来的时候SCL是高电平 
// SOFT_IIC_W_SDA(1);  
// SOFT_IIC_W_SCL(1); 
 SOFT_IIC_W_SDA(0);  
 SOFT_IIC_W_SCL(0);  
} 
 
//停止信号 
void SOFT_IIC_STOP(void) 
{ 
 //进来的时候SCL都是低电平  
 SOFT_IIC_W_SDA(0);  //先让SDA拉低，方便产生上升沿 (避免SDA是高电平，无法产生上升沿)
 SOFT_IIC_W_SCL(1);  
 SOFT_IIC_W_SDA(1);  
} 
 
//发送1个字节  高位先发 
void SOFT_IIC_SendByte(uint8_t Byte) 
{ 
 //进来的时候SCL都是低电平 
 uint8_t BitValue=0; 
 for(uint8_t i=0;i<8;i++) 
 { 
  //Byte>>(7-i)  目的：把要发送的位移到最低位 
  BitValue=Byte>>(7-i); 
  BitValue=BitValue&0x01;  //目的：只保留最低位，其他位清0 
  SOFT_IIC_W_SDA(BitValue);  //根据要发送的数据，控制SDA的电平 
  SOFT_IIC_W_SCL(1); //产生高电平，让从机读取 
  SOFT_IIC_W_SCL(0); //产生低电平，准备下次要发送的数据 
 } 
} 
 
/*发送数据0xAA  1010 1010 
                          最终BitValue 
第一次发送   Byte>>(7-0)   0000 0001 
             BitValue&0x01 0000 0001 
第二次发送   Byte>>(7-1)   0000 0010 
             BitValue&0x01 0000 0000 
第三次发送   Byte>>(7-2)   0000 0101 
             BitValue&0x01 0000 0001   
第四次发送   Byte>>(7-3)   0000 1010 
             BitValue&0x01 0000 0000 
*/ 
 
//接收1个字节  高位先接收 
uint8_t SOFT_IIC_RecByte(void) 
{ 
 SOFT_IIC_W_SDA(1); //主机释放SDA，让从机操作SDA的高低电平 
 //进来的时候SCL都是低电平  
 uint8_t Byte=0; 
 uint8_t BitValue=0; 
 for(uint8_t i=0;i<8;i++) 
 {  
  SOFT_IIC_W_SCL(1); //产生高电平，从机不能变换数据，主机读取SDA线的电平 
  BitValue= SOFT_IIC_R_SDA();  //读取SDA的电平
  Byte |= BitValue<<(7-i);     //将收到的数据设置到对应位 
  SOFT_IIC_W_SCL(0); //产生低电平，让从机准备下一位要发送的数据   
 } 
 return Byte; 
} 
 
 
/*Byte最开始的结果是0000 0000 
第一次接收  接收数据1   Byte |= BitValue<<(7-0)执行的结果   Byte=1000 0000 
第二次接收  接收数据0   Byte |= BitValue<<(7-1)执行的结果   Byte=1000 0000 
第三次接收  接收数据1   Byte |= BitValue<<(7-2)执行的结果   Byte=1010 0000 
第四次接收  接收数据0   Byte |= BitValue<<(7-3)执行的结果   Byte=1010 0000 
*/ 
 
//发送应答   
//0  非应答   1  应答 
//应答信号  SDA  低   非应答信号  SDA  高 
void SOFT_IIC_SendACK(uint8_t ack) 
{ 
 //进来的时候SCL都是低电平 
 if(ack==0) 
 {//非应答 
  SOFT_IIC_W_SDA(1); //产生高电平，非应答 
 } 
 else  
 { 
  SOFT_IIC_W_SDA(0); //产生低电平，应答   
 } 
 SOFT_IIC_W_SCL(1); //产生高电平，从机读取应答信号 
  SOFT_IIC_W_SCL(0); //产生低电平，除了停止信号，每段都以低电平结束，方便拼接 
} 
 
//接收应答 
//应答信号  SDA  低   非应答信号  SDA  高 
//返回值  1   收到应答信号   0  未收到应答信号 
uint8_t SOFT_IIC_RecACK(void) 
{ 
 uint8_t Ack=0; 
 SOFT_IIC_W_SDA(1); //主机释放SDA，让从机操作SDA的高低电平 
 //进来的时候SCL都是低电平  
 SOFT_IIC_W_SCL(1); //产生高电平，主机读取应答信号 
 Ack=SOFT_IIC_R_SDA(); //读取应答信号 
 SOFT_IIC_W_SCL(0);  //产生低电平 
 if(Ack==0) 
 {//收到应答信号 
  return 1; 
 } 
 else 
 {//未收到应答信号 
  return 0;   
 } 
} 