/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
//#define BPS 400
//#define TEMPO (1000 * 60 / BPS)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t rx_buffer;

uint8_t notes[100];
uint8_t duration[100];
uint8_t idx_music;
uint8_t music_length;
uint16_t tempo = 120;
uint16_t tempo_delay;

uint8_t flag_note;
uint8_t flag_dur;
uint8_t flag_tempo;

int note_table[14] = {262, 293, 329, 349, 392, 440, 494,
                      523, 587, 659, 698, 784, 880, 988};
uint8_t playMusic = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_NVIC_Init(void);
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
  MX_USART3_UART_Init();
  MX_TIM2_Init();

  /* Initialize interrupts */
  MX_NVIC_Init();
  /* USER CODE BEGIN 2 */
  HAL_UART_Receive_IT(&huart3, &rx_buffer, 1);
  tempo_delay = 500 * 60 / tempo;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    while (playMusic == 1)  // use while to utilize 'break'
    {
      if (music_length == 0)  // if no music stored
      {
        playMusic = 0;
        break;
      }

      for (idx_music = 0; idx_music < music_length; idx_music++)
      {
        // stop on signal
        if (playMusic == 0)
        {
          break;
        }

        if (notes[idx_music] > -1)
        {
          TIM2->ARR = 1000000 / note_table[notes[idx_music]];
          TIM2->CNT = 0;
          HAL_TIM_OC_Start(&htim2, TIM_CHANNEL_1);
          HAL_Delay(tempo_delay * duration[idx_music]);
          HAL_TIM_OC_Stop(&htim2, TIM_CHANNEL_1);
        }
        else
        {
          HAL_Delay(tempo_delay * duration[idx_music]);
        }
        HAL_Delay(10);
      }

      playMusic = 0;  // done playing
    }
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief NVIC Configuration.
  * @retval None
  */
static void MX_NVIC_Init(void)
{
  /* EXTI15_10_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
  /* EXTI3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(EXTI3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI3_IRQn);
  /* USART3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(USART3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(USART3_IRQn);
}

/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if (huart->Instance == USART3)
  {
    if (rx_buffer == 0x4E)       // 'N' : notes
    {
      // clear array (optional)
      flag_note = 1;
      flag_dur = 0;
      idx_music = 0;
    }
    else if (rx_buffer == 0x44)  // 'D' : duration
    {
      // clear array (optional)
      flag_note = 0;
      flag_dur = 1;
      idx_music = 0;
    }
    else if (rx_buffer == 0x7F)  // end code
    {
      flag_note = 0;
      flag_dur = 0;
      music_length = idx_music;
      idx_music = 0;
    }
    else if (rx_buffer == 0x50) // 'P' : play
    {
      playMusic = 1;
    }
    else if (rx_buffer == 0x53) // 'S' : stop
    {
      playMusic = 0;
    }
    else if (rx_buffer == 0x54) // 'T' : tempo
    {
      flag_tempo = 1;
    }
    else
    {
      if (flag_note == 1)
      {
        notes[idx_music] = rx_buffer;
        idx_music++;
      }
      if (flag_dur == 1)
      {
        duration[idx_music] = rx_buffer;
        idx_music++;
      }
      if (flag_tempo == 1)
      {
        tempo = rx_buffer;

        if (tempo == 0)
          tempo_delay = 500 * 60 / 60;
        else if (tempo == 1)
          tempo_delay = 500 * 60 / 80;
        else if (tempo == 2)
          tempo_delay = 500 * 60 / 100;
        else if (tempo == 3)
          tempo_delay = 500 * 60 / 120;
        else if (tempo == 4)
          tempo_delay = 500 * 60 / 200;

        flag_tempo = 0;
      }
    }
  }
  HAL_UART_Receive_IT(&huart3, &rx_buffer, 1);
}
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if (GPIO_Pin == GPIO_PIN_3)
  {
    if (playMusic)
      playMusic = 0;
    else if (!playMusic)
      playMusic = 1;
  }
  if (GPIO_Pin == GPIO_PIN_13)
  {
    if (playMusic)
      playMusic = 0;
    else if (!playMusic)
      playMusic = 1;
  }
}
/* USER CODE END 4 */

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
