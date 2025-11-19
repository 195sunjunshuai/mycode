#include "dma.h"
#include "stdio.h"
#define ADC_SET_NUM 10
#define DMA_Buf_Size 2
volatile uint16_t ADC_Buf_Value[ADC_SET_NUM*DMA_Buf_Size] = {0}; 


void ADC_DMA_Config(void)
{
    /* GPIO: PA5 (CH5), PC1 (CH11) 为模拟输入 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC, ENABLE);
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_5;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_1;
    GPIO_Init(GPIOC, &GPIO_InitStruct);

    /* DMA1 时钟 */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    /* DMA: ADC1->DR -> ADCConvertedValue1[2] 循环 */
    DMA_InitTypeDef DMA_InitStructure = {0};
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;         // 外设地址
    DMA_InitStructure.DMA_MemoryBaseAddr     = (uint32_t)ADC_Buf_Value;     // 数组首地址
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralSRC;       // 外设到内存
    DMA_InitStructure.DMA_BufferSize         = ADC_SET_NUM*DMA_Buf_Size;    // 数组长度
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;   // 不自增
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;        // 自增
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Circular;            // 循环
    DMA_InitStructure.DMA_Priority           = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);
    DMA_Cmd(DMA1_Channel1, ENABLE);

    /* ADC1 时钟与分频（<=14MHz） */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_ADCCLKConfig(RCC_PCLK2_Div6); // 72MHz/6=12MHz

    /* ADC 基本配置：扫描+连续模式，2 通道，软触发，右对齐 */
    ADC_InitTypeDef ADC_InitStruct = {0};
    ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStruct.ADC_ScanConvMode = ENABLE;           // 扫描
    ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;     // 连续
    ADC_InitStruct.ADC_NbrOfChannel = 2;                // 2通道
    ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;  // 软触发
    ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right; // 右对齐

    ADC_Init(ADC1, &ADC_InitStruct);

    /* 配置规则序列：rank1=CH5(PA5)，rank2=CH11(PC1) */
    ADC_RegularChannelConfig(ADC1, ADC_Channel_5,  1, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 2, ADC_SampleTime_55Cycles5);

    /* 使能 ADC 的 DMA 请求 */
    ADC_DMACmd(ADC1, ENABLE);

    /* 使能 + 校准 */
    ADC_Cmd(ADC1, ENABLE);
    ADC_ResetCalibration(ADC1); while (ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1); while (ADC_GetCalibrationStatus(ADC1));
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

void ADC_DMA_Handle(void)
{
	uint8_t i;
	uint16_t ADC_DMA_LIGHT_VALUE = 0;
  uint16_t ADC_DMA_SMOKE_VALUE = 0;
	for(i=0;i<ADC_SET_NUM;i++)
	{
    ADC_DMA_LIGHT_VALUE += ADC_Buf_Value[i*2+0];
    ADC_DMA_SMOKE_VALUE += ADC_Buf_Value[i*2+1];
	}
	ADC_DMA_LIGHT_VALUE/=10;
	ADC_DMA_SMOKE_VALUE/=10;
	printf("光敏采样=%u, 电压=%.3fV\r\n",
           ADC_DMA_LIGHT_VALUE, 3.3f * ADC_DMA_LIGHT_VALUE / 4095.0f);
  printf("烟雾采样=%u, 电压=%.3fV\r\n",
           ADC_DMA_SMOKE_VALUE, 3.3f * ADC_DMA_SMOKE_VALUE / 4095.0f);
}