/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "fsmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
   
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_TIM6_Init();
  MX_SPI2_Init();
  MX_FSMC_Init();
  MX_USART2_UART_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
#if (UART1_TYPE==0) 
		//HAL_StatusTypeDef HAL_UART_Receive_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size); 
		//参数1   串口的句柄，也就是串口几 
	  //参数2   接收数据存放的地址  
	  //参数3   待接收的长度    接收后该参数长度，就会触发回调函数  HAL_UART_RxCpltCallback  
    HAL_UART_Receive_IT(&huart1,&U1_Rec_Single,1); 
#elif (UART1_TYPE==1) 
	  //参数1   串口的句柄，也就是串口几 
	  //参数2   接收数据存放的地址  
	  //参数3   待接收的长度    接收后该参数长度，就会触发回调函数  HAL_UART_RxCpltCallback  
    HAL_UART_Receive_IT(&huart1,U1_Rec_Buff,8); 
    #elif (UART1_TYPE==2) 
    //HAL_StatusTypeDef HAL_UARTEx_ReceiveToIdle_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size); 
	 //参数1   串口的句柄，也就是串口几 
	 //参数2   接收数据存放的地址  
	 //参数3   缓冲区可以接收的最大长度    触发回调函数  HAL_UARTEx_RxEventCallback 
	 //触发的条件  1.总线空闲了，触发空闲回调   2.接收够了缓冲区的最大长度，触发空闲回调 
   HAL_UARTEx_ReceiveToIdle_IT(&huart1,U1_Rec_Buff,sizeof(U1_Rec_Buff)); 
#endif 	
#if (UART2_TYPE==0) 
		//HAL_StatusTypeDef HAL_UART_Receive_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size); 
		//参数1   串口的句柄，也就是串口几 
	  //参数2   接收数据存放的地址  
	  //参数3   待接收的长度    接收后该参数长度，就会触发回调函数  HAL_UART_RxCpltCallback  
    HAL_UART_Receive_IT(&huart2,&U2_Rec_Single,1); 
#elif (UART2_TYPE==1) 
	  //参数1   串口的句柄，也就是串口几 
	  //参数2   接收数据存放的地址  
	  //参数3   待接收的长度    接收后该参数长度，就会触发回调函数  HAL_UART_RxCpltCallback  
    HAL_UART_Receive_IT(&huart2,kqm.R_BUFF,8); 
    #elif (UART2_TYPE==2) 
    //HAL_StatusTypeDef HAL_UARTEx_ReceiveToIdle_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size); 
	 //参数1   串口的句柄，也就是串口几 
	 //参数2   接收数据存放的地址  
	 //参数3   缓冲区可以接收的最大长度    触发回调函数  HAL_UARTEx_RxEventCallback 
	 //触发的条件  1.总线空闲了，触发空闲回调   2.接收够了缓冲区的最大长度，触发空闲回调 
   HAL_UARTEx_ReceiveToIdle_IT(&huart2,kqm.R_BUFF,sizeof(kqm.R_BUFF)); 
 #elif (UART2_TYPE==3)
 HAL_UART_Receive_IT(&huart2,&U2_Rec_Single,1);
#endif 	
//   W25Q64_ReadID_0x90();
//	 W25Q64_ReadID_0x9F();
		LCD_Init(); 
		LCD_Clear(RED); 
  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();  /* Call init function for freertos objects (in freertos.c) */
  MX_FREERTOS_Init();

  /* Start scheduler */
  osKernelStart();
  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
//		HAL_GPIO_TogglePin(GPIOE, LED1_Pin|LED2_Pin|LED3_Pin|LED4_Pin); 
//    HAL_Delay(1000);
//		if(KEY_Period[0]>=KEY_Period[1]) 
//		{ 
//			KEY_Period[0]=0; 
//			Key_Handle(); 
//		}
    KQM_Handle();
		HAL_Delay(1000);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */


/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM7 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */
	if(htim->Instance == TIM6) 
	{ 
		kqm.R_Idle=1; //空闲中断标志位置 1 
		HAL_TIM_Base_Stop_IT(&htim6);     
	} 
  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM7) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
