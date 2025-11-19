#include "adcfire.h"
#include "stdio.h"
/*
	 PC2/ADC123_IN12
*/
uint16_t ADC_FIRE_VALUE;
void ADCFIRE_Config()
{
	//1.开时钟      
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE); //中文固件库翻译手册  Table 373  
 //2.定义结构体,目的是给XXX_Init函数准备参数 
 GPIO_InitTypeDef GPIO_InitStruct={0};   //变量名无所谓 
  
 //3.给结构体赋值 
 GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN; //通用模拟输入   
 GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2; //待设置的引脚 
// GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;  //GPIO口的速率  

 GPIO_Init(GPIOC,&GPIO_InitStruct); 
 //5.ADC1时钟
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
 //6.对ADC分频
 RCC_ADCCLKConfig(RCC_PCLK2_Div6);
 //7.定义结构体,目的是给XXX_Init函数准备参数  
 ADC_InitTypeDef ADC_InitStruct={0};   //变量名无所谓
 //8.给结构体赋值 
 ADC_InitStruct.ADC_ContinuousConvMode=DISABLE;         //是否开启连续
 ADC_InitStruct.ADC_DataAlign=ADC_DataAlign_Right;      //右对齐
 ADC_InitStruct.ADC_ExternalTrigConv=ADC_ExternalTrigConv_None;  //软件触发
 ADC_InitStruct.ADC_Mode=ADC_Mode_Independent;          //独立工作
 ADC_InitStruct.ADC_NbrOfChannel=1;        //待转换的通道数量 ADC_SQR位23-20
 ADC_InitStruct.ADC_ScanConvMode=DISABLE;               //是否需要扫描
//9.调用XXX_Init函数，将参数写入到寄存器中 
 ADC_Init(ADC1,&ADC_InitStruct); 
//10.ADC使能 
 ADC_Cmd(ADC1,ENABLE);
//11.校准
/* Enable ADC1 reset calibration register */   
  ADC_ResetCalibration(ADC1);                           //初始化校准寄存器
  /* Check the end of ADC1 reset calibration register */
  while(ADC_GetResetCalibrationStatus(ADC1));           //等待初始化完成

  /* Start ADC1 calibration */
  ADC_StartCalibration(ADC1);              //开始校准指定的 ADC 的校准寄存器
  /* Check the end of ADC1 calibration */
  while(ADC_GetCalibrationStatus(ADC1));   //等待校准完成
     
  /* Start ADC1 Software Conversion */ 
//  ADC_SoftwareStartConvCmd(ADC1, ENABLE); //使能指定的 ADC 的软件转换启动功能
}
void ADCFIRE_Handle()
{
	//1.配置要转换的通道
	ADC_RegularChannelConfig(ADC1,ADC_Channel_12,1,ADC_SampleTime_28Cycles5);	
	//参数2：光敏接PC2，PC2是ADC12_IN12
	//参数3: ADC_SRQx的顺序
	//2.手动启动转换
	ADC_SoftwareStartConvCmd(ADC1, ENABLE); //使能指定的 ADC 的软件转换启动功能
	//3.等待转换完成
	while(ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC)==RESET) ;
	//4.获取转换结果
	ADC_FIRE_VALUE=ADC_GetConversionValue(ADC1);
	printf("火焰采样值=%d\r\n",ADC_FIRE_VALUE);
	printf("火焰采样口电压=%.2fv\r\n",3.3*ADC_FIRE_VALUE/4096.0);
}