#include "rtc.h"
#include "time.h"
struct tm Set_Time={0,33,11,10,9-1,2025-1900};
uint32_t  Set_Time_Cnt;  //待设置的时间戳秒数
uint32_t  Now_Time_Cnt;  //当前时间戳秒数
struct tm *Now_Time=&Set_Time;  //当前时间结构体指针

//RTC初始化
void RTC_Config()
{
	// 使能 PWR 与 BKP 外设时钟（位于 APB1 总线）
	// RCC->APB1ENR：PWREN[28]=1、BKPEN[27]=1   7.3.8
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

	/* 允许访问备份域（BKP/RTC 寄存器可写）     4.4.1
	 * PWR->CR：DBP[8]=1（Disable Backup write protection=0 → 1 表示关闭保护，允许写）
	 */
	PWR_BackupAccessCmd(ENABLE);

	/* 复位备份域（清 RTC/BKP 寄存器，时钟源选择等也会被复位）
	 * RCC->BDCR：BDRST[16] 置 1 再清 0         6.3.9
	 */
	BKP_DeInit();

	/* 使能 LSE（外部低速晶振 32.768 kHz）      6.3.9
	 * RCC->BDCR：LSEON[0]=1（必要时清除 LSEBYP[2]）
	 */
	RCC_LSEConfig(RCC_LSE_ON);

	/* 等待 LSE 就绪
	 * 读取 RCC->BDCR：LSERDY[1] == 1
	 */
	while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET) {}

	/* 选择 LSE 作为 RTC 时钟源
	 * RCC->BDCR：RTCSEL[9:8] = 0b01 （00=无，01=LSE，10=LSI，11=HSE/128）
	 */
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

	/* 使能 RTC 时钟
	 * RCC->BDCR：RTCEN[15]=1
	 */
	RCC_RTCCLKCmd(ENABLE);

	/* 等待 RTC 寄存器与 APB 总线同步
	 * RTC->CRL：RSF[3] 同步标志。库函数流程：先清 RSF，再等待硬件置位 RSF=1
	 */
	RTC_WaitForSynchro();

	/* 等待上一次对 RTC 的写操作结束
	 * RTC->CRL：RTOFF[5] = 1 表示“RTC 寄存器空闲，可写”
	 */
	RTC_WaitForLastTask();

	/* 使能秒中断
	 * RTC->CRH：SECIE[0]=1 使能“秒”溢出中断（1Hz）
	 */
	RTC_ITConfig(RTC_IT_SEC, ENABLE);

	/* 再次等待上一次写操作结束（良好习惯，确保前一步写入完成）
	 * RTC->CRL：RTOFF[5] = 1
	 */
	RTC_WaitForLastTask();

	/* 设置 RTC 预分频，使 RTC 的计数周期为 1 秒
	 * 预分频寄存器：RTC->PRLH / RTC->PRLL 写入 32767（0x7FFF）
	 * 实际分频：RTCCLK / (PRL+1) = 32768 / (32767+1) = 1 Hz
	 * 设置流程（库内部完成）：先置 CRL.CNF[4]=1 进入配置态，写 PRL，高低 16 位，清 CNF，
	 * 再等待 RTOFF=1
	 */
	RTC_SetPrescaler(32767);

	/* 再次等待写完成
	 * RTC->CRL：RTOFF[5] = 1
	 */
	RTC_WaitForLastTask();
 // 4. 配置 NVIC
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // 抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;        // 次优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           // 使能
	NVIC_Init(&NVIC_InitStructure);
	Time_Adjust();
}
//更新时间
void Time_Adjust(void)
{
  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();
  /* Change the current time */
	Set_Time_Cnt=mktime(&Set_Time);
  RTC_SetCounter(Set_Time_Cnt);
  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();
}
void Time_Adjust2(void)
{
  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();
  /* Change the current time */
//	Set_Time_Cnt=mktime(&Set_Time);
  RTC_SetCounter(Set_Time_Cnt);
  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();
}
uint8_t RTC_Second_FLAG = 0;

//RTC中断服务函数 
void RTC_IRQHandler(void) 
{ 
 //1.检测中断标志位 
  if (RTC_GetITStatus(RTC_IT_SEC) != RESET) 
  { 
  RTC_Second_FLAG=1; 
    //2.清除中断标志位 
    RTC_ClearITPendingBit(RTC_IT_SEC); 
   
  //3.等待上一次操作完成 
    RTC_WaitForLastTask(); 
  }  
} 