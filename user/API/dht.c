#include "dht.h" 
#include "delay.h" 
#include "stdio.h" 
DHT Dht;
/* 
DHT11  PG11 
GPIO 口的模式，两种配置模式都可以 
1.开漏输出     
2.推挽输出和浮空输入两种模式切换 
开漏输出，可以通过读IDR寄存器获取线的电平，所以选择开漏更方便 
*/ 
void DHT_Config(void) 
{ 
//中文固件库翻译手册路径：授课大纲资料\1.项目分析与环境搭建\资料\芯片相关资料\STM32固件库使用手册的中文翻译版 
//1.开 GPIOG 的时钟      STM32 固件库使用手册的中文翻译版的15.2.22 
RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG,ENABLE); 
//2.定义结构体    参考STM32固件库使用手册的中文翻译版的10.2.3 
//目的：给XXX_Init函数准备参数，XXX_Init函数需要传递这个类型的变量地址 
GPIO_InitTypeDef GPIO_InitStructure={0};  
//3.给结构体赋值  参考STM32固件库使用手册的中文翻译版的10.2.3  
GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; //待设置的引脚 
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;  //引脚模式 通用开漏输出 
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;  //引脚输出速率 
//4.调用 XXX_Init 将结构体的参数写入到寄存器中 
GPIO_Init(GPIOG, &GPIO_InitStructure);  
//5.释放一下总线 
GPIO_SetBits(GPIOG, GPIO_Pin_11); 
} 
//DHT11 的处理函数
uint8_t Buff[5]={0};
uint8_t DHT_Handle(void) 
{ 
 uint8_t time_out=0;  //超时变量  
 uint8_t Data_CS=0; 
 uint8_t i,j; 
 //1.STM32先发起起始信号   位置1 
 GPIO_ResetBits(GPIOG, GPIO_Pin_11);  
 Delay_ms(18); 
 //2.起始信号结束，释放总线  位置2 
 GPIO_SetBits(GPIOG, GPIO_Pin_11);   
 //3.检测响应信号的低电平，在低电平没有到来之前，一直是高电平 
 while(GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_11)==Bit_SET) 
 {//未检测到响应信号的低电平 
  time_out++; 
  Delay_us(1); 
  if(time_out>=100) 
  {//超过100us还未等到响应信号的低电平，直接返回失败 
   return 0; 
  } 
 } 
 //4.刚检测到响应信号低电平，等待检测响应信号的高电平  位置3 
 time_out=0; 
 while(GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_11)==Bit_RESET) 
 {//未检测到响应信号的高电平 
  time_out++; 
  Delay_us(1); 
  if(time_out>=100) 
  {//超过100us还未等到响应信号的高电平 
   //其中83us是正常的响应信号低电平，剩余17us等待响应信号的高电平，超时未等到直接返回失败 
   return 0; 
  } 
 } 
 for(i=0;i<5;i++) 
 { 
  for(j=0;j<8;j++) 
  { 
   //5.刚检测到响应的高电平，等待检测数据位的低电平   位置4 
   time_out=0;  
   while(GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_11)==Bit_SET) 
   {//未检测到数据位的低电平 
    time_out++; 
    Delay_us(1); 
    if(time_out>=100) 
    {//超过100us还未等到数据位的低电平，直接返回失败
     //其中87us是正常的响应信号的高电平，剩余13us等待数据位的低电平，超时未等到直接返回失败 
     return 0; 
    } 
   }  
   //6.刚检测到数据位的低电平，等待数据位的高电平    位置5 
   time_out=0; 
   while(GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_11)==Bit_RESET) 
   {//未检测到数据位的高电平 
    time_out++; 
    Delay_us(1); 
    if(time_out>=100) 
    {//超过100us还未等到数据位的高电平 
     //其中54us是正常的数据位低电平，剩余46us等待数据位的高电平，超时未等到直接返回失败 
     return 0; 
    } 
   }  
   //7.区分数据0和数据1   位置6 
   Delay_us(30);   //对着手册的数据0和数据1的图查看，延时30us区分数据0和数据1 
   if(GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_11)==Bit_RESET) 
   {//收到数据0 
    Buff[i] &= ~(0x01<<(7-j)); 
   } 
   else 
   {//收到数据1 
    Buff[i] |= (0x01<<(7-j)); 
   } 
  } 
 } 
 Data_CS=Buff[0]+Buff[1]+Buff[2]+Buff[3];  //字节计算校验和 

 if(Data_CS==Buff[4]) 
 { 
	double num=Buff[3];
	if((Buff[3]>>7)==0)
	{
	  Dht.Tem=Buff[2]+Buff[3]*0.1;
	}
	else
	{
		Dht.Tem=(Buff[2]+(Buff[3]<<1)*0.1/2)*-1;
	}
	
	Dht.Hum=Buff[0];
//  printf("温度=%.2f,湿度=%d\r\n",Dht.Tem,Dht.Hum);   
 } 
 else 
 { 
  printf("DHT11校验失败\r\n"); 
 } 
} 