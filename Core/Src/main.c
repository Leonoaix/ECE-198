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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "liquidcrystal_i2c.h"
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
I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_I2C1_Init(void);
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
  MX_USART2_UART_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
    char printout[16];

    long refreshRate = 2;
    uint32_t lastRefreshTick = HAL_GetTick();
    uint32_t now;
    HD44780_Init(2);

    uint16_t joystick_val[3];
    char cursor_x = 0;
    _Bool cursor_y = 1;
    _Bool went_right = 0;
    _Bool went_left = 0;
    _Bool went_y = 0;
    _Bool pressed = 0;

    char time[3] = {0};
    char time_limit[3] = {24, 60,60};
    char* time_print[] = {"hr ", "min", "sec"};
    char time_state = 0;
    unsigned short adding[] = {1, 5, 10, 20};
    unsigned int time_remaining;
    uint32_t start_time;
    unsigned int total_time;

    int remaining_pause;

    _Bool servo_state = 0;

    enum Page screen = SELECTING;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
      now = HAL_GetTick();

      if(screen == SELECTING) {
          if (!went_right && joystick_val[0] > 4000) {
              went_right = 1;
          } else if (went_right && joystick_val[0] < 4000) {
              if (cursor_y)
                  cursor_x = (cursor_x + 4) % 16;
              else
                  cursor_x = 12;
              went_right = 0;
          }
          if (!went_left && joystick_val[0] < 20) {
              went_left = 1;
          } else if (went_left && joystick_val[0] > 20) {
              if (cursor_y)
                  cursor_x = (cursor_x - 4) % 16;
              else
                  cursor_x = 8;
              went_left = 0;
          }


          if (pressed == 0 && joystick_val[2] < 20) {
              pressed = 1;
              if (cursor_y) {
                  time[time_state] = (time[time_state] + adding[cursor_x / 4]) % time_limit[time_state];
              }
              else {
                  if (cursor_x == 8) {
                      time_state++;
                      if(time_state > 2){
                          time_state = 2;
                          screen = CONFIRMING;
                          cursor_x = 1;
                      }
                  } else {
                      time_state--;
                      time_state = time_state < 0 ? 0 : time_state;
                  }
              }
          } else if (pressed && joystick_val[2] > 20) {
              pressed = 0;
          }


          if (!went_y && joystick_val[1] > 4000) {
              went_y = 1;
          } else if (went_y && joystick_val[1] < 4000) {
              cursor_y = 0;
              cursor_x = 8;
              went_y = 0;
          }
          if (!went_y && joystick_val[1] < 20) {
              went_y = 1;
          } else if (went_y && joystick_val[1] > 20) {
              cursor_y = 1;
              cursor_x = 0;
              went_y = 0;
          }
      }
      else if(screen == CONFIRMING){
          if (!went_right && joystick_val[0] > 4000) {
              went_right = 1;
          } else if (went_right && joystick_val[0] < 4000) {
              cursor_x = 9;
              went_right = 0;
          }
          if (!went_left && joystick_val[0] < 20) {
              went_left = 1;
          } else if (went_left && joystick_val[0] > 20) {
              cursor_x = 1;
              went_left = 0;
          }

          if (pressed == 0 && joystick_val[2] < 20) {
              pressed = 1;
              if(cursor_x == 9){
                  screen = SELECTING;
                  cursor_x = 0;
                  cursor_y = 1;
              }
              else if (cursor_x == 1){
                  screen = COUNTING;
                  start_time = now;
                  total_time = (time[0]*3600 + time[1]*60 + time[2])*1000;
                  servo_state = 1;
                  remaining_pause = 2;
                  cursor_x = 6;
              }
          } else if (pressed && joystick_val[2] > 20) {
              pressed = 0;
          }
      }
      else if(screen == COUNTING){
          if(now-start_time > total_time){
              screen = REMINDING;
              servo_state = 0;
          }
          time_remaining = total_time - (now - start_time);

          if (!went_right && joystick_val[0] > 4000) {
              went_right = 1;
          } else if (went_right && joystick_val[0] < 4000) {
              cursor_x = 7;
              went_right = 0;
          }
          if (!went_left && joystick_val[0] < 20) {
              went_left = 1;
          } else if (went_left && joystick_val[0] > 20) {
              cursor_x = 6;
              went_left = 0;
          }

          if (pressed == 0 && joystick_val[2] < 20) {
              pressed = 1;
              if(cursor_x == 7 && remaining_pause > 0){
                  screen = PAUSING;
                  servo_state = 0;
                  remaining_pause--;
              }
          } else if (pressed && joystick_val[2] > 20) {
              pressed = 0;
          }
      }
      else if(screen == PAUSING){
          if (pressed == 0 && joystick_val[2] < 20) {
              pressed = 1;
              screen = COUNTING;
              servo_state = 1;
              total_time = time_remaining;
              start_time = now;
              cursor_x = 6;
          } else if (pressed && joystick_val[2] > 20) {
              pressed = 0;
          }
      }
      else{
          if (pressed == 0 && joystick_val[2] < 20) {
              pressed = 1;
              screen = SELECTING;
              cursor_x = 0;
              cursor_y = 1;
              time[0] = time[1] = time[2] = 0;
              time_state = 0;
          } else if (pressed && joystick_val[2] > 20) {
              pressed = 0;
          }
      }
//
      if(now - lastRefreshTick >= 1000 / refreshRate){
          HD44780_Clear();
          if(screen == SELECTING) {
              HD44780_SetCursor(0, 1);
              HD44780_PrintStr(" +01 +05 +10 +20");
              HD44780_SetCursor(0, 0);
              sprintf(printout, "%s:%d", time_print[time_state], time[time_state]);
              HD44780_PrintStr(printout);
              HD44780_SetCursor(9, 0);
              HD44780_PrintStr("->  <-");
              HD44780_SetCursor(cursor_x, cursor_y);
              HD44780_Blink();
          }
          else if(screen == CONFIRMING){
              HD44780_SetCursor(0,0);
              sprintf(printout, "%d:%d:%d", time[0], time[1], time[2]);
              HD44780_PrintStr(printout);
              HD44780_SetCursor(0,1);
              HD44780_PrintStr("  Start   Back");
              HD44780_SetCursor(cursor_x, 1);
              HD44780_Blink();
          }
          else if(screen == COUNTING){
              HD44780_SetCursor(0,0);
              sprintf(printout, "Left:   Pauses:%d", remaining_pause);
              HD44780_PrintStr(printout);
              HD44780_SetCursor(0,1);
              sprintf(printout, "%d:%d:%d", time_remaining/1000/3600,
                      time_remaining/1000%3600/60, time_remaining/1000%3600%60);
              HD44780_PrintStr(printout);
              HD44780_SetCursor(cursor_x, 0);
              HD44780_Blink();
          }
          else if(screen == PAUSING){
              HD44780_SetCursor(0,0);
              HD44780_PrintStr(" Restart");
              HD44780_SetCursor(0,1);
              sprintf(printout, "%d:%d:%d", time_remaining/1000/3600,
                      time_remaining/1000%3600/60, time_remaining/1000%3600%60);
              HD44780_PrintStr(printout);
              HD44780_SetCursor(0,0);
              HD44780_Blink();
          }
          else{
              HD44780_SetCursor(0,0);
              HD44780_PrintStr("Over");
              HD44780_SetCursor(0,1);
              HD44780_PrintStr(" Restart");
              HD44780_SetCursor(0,1);
              HD44780_Blink();
          }
          lastRefreshTick = now;
      }
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 7;
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

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

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
