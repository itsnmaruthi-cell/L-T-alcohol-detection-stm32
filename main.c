/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include <stdio.h>
#include <stdint.h>

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
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
volatile uint32_t *const RCC_AHB1ENR = (volatile uint32_t*) 0x40023830;
volatile uint32_t *const RCC_APB2ENR = (volatile uint32_t*) 0x40023844;
volatile uint32_t *const RCC_APB1ENR = (volatile uint32_t*) 0x40023840;

volatile uint32_t *const GPIOA_MODER = (volatile uint32_t*) 0x40020000;
volatile uint32_t *const GPIOA_ODR   = (volatile uint32_t*) 0x40020014;
volatile uint32_t *const GPIOA_AFRL  = (volatile uint32_t*) 0x40020020;

volatile uint32_t *const GPIOB_MODER = (volatile uint32_t*) 0x40020400;
volatile uint32_t *const GPIOB_ODR   = (volatile uint32_t*) 0x40020414;

volatile uint32_t *const ADC1_SR     = (volatile uint32_t*) 0x40012000;
volatile uint32_t *const ADC1_CR1    = (volatile uint32_t*) 0x40012004;
volatile uint32_t *const ADC1_CR2    = (volatile uint32_t*) 0x40012008;
volatile uint32_t *const ADC1_SQR3   = (volatile uint32_t*) 0x4001203C;
volatile uint32_t *const ADC1_DR     = (volatile uint32_t*) 0x4001204C;

volatile uint32_t *const USART2_SR   = (volatile uint32_t*) 0x40004400;
volatile uint32_t *const USART2_DR   = (volatile uint32_t*) 0x40004404;
volatile uint32_t *const USART2_BRR  = (volatile uint32_t*) 0x40004408;
volatile uint32_t *const USART2_CR1  = (volatile uint32_t*) 0x4000440C;
void delay(volatile int count) {
    while(count--);
}

void uart2_init(void) {
    *RCC_APB1ENR |= (1U << 17);          // USART2 clock enable

    *GPIOA_MODER &= ~(3U << 4);
    *GPIOA_MODER |=  (2U << 4);          // PA2 alternate function mode

    *GPIOA_AFRL  &= ~(0xF << 8);
    *GPIOA_AFRL  |=  (7U << 8);          // AF7 = USART2 TX on PA2

    *USART2_BRR = 0x0683;                // 9600 baud @ 16MHz
    *USART2_CR1 |= (1U << 3);            // TE — transmitter enable
    *USART2_CR1 |= (1U << 13);           // UE — USART enable
}

void uart2_send_char(char c) {
    while(!(*USART2_SR & (1U << 7)));    // wait for TXE flag
    *USART2_DR = c;
}

void uart2_send_string(char *str) {
    while(*str) {
        uart2_send_char(*str);
        str++;
    }
}

void uart2_send_number(uint16_t num) {
    char buffer[6];
    sprintf(buffer, "%d", num);
    uart2_send_string(buffer);
}


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
  /* USER CODE BEGIN 2 */
  *RCC_AHB1ENR |= (1U << 0);      // GPIOA clock enable
      *RCC_AHB1ENR |= (1U << 1);      // GPIOB clock enable
      *RCC_APB2ENR |= (1U << 8);      // ADC1 clock enable

      *GPIOA_MODER |= (3U << 0);      // PA0 analog mode — MQ-3 AOUT

      *GPIOA_MODER &= ~((3U<<10)|(3U<<12)|(3U<<14));
      *GPIOA_MODER |=  (1U<<10)|(1U<<12)|(1U<<14);   // PA5,PA6,PA7 output

      *GPIOB_MODER &= ~(3U<<0);
      *GPIOB_MODER |=  (1U<<0);       // PB0 output — buzzer

      *ADC1_SQR3 = 0;                 // channel 0 selected
      *ADC1_CR2 |= (1U<<0);           // ADC ON

      uart2_init();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  *ADC1_CR2 |= (1U<<30);                // start conversion
	          while(!(*ADC1_SR & (1U<<1)));         // wait for EOC flag
	          uint16_t adc_val = *ADC1_DR;         // read converted value

	          uart2_send_string("ADC: ");
	          uart2_send_number(adc_val);
	          uart2_send_string("  Status: ");

	          if (adc_val < 1700) {
	              *GPIOA_ODR |= (1U<<5);
	              *GPIOA_ODR &= ~(1U<<6);
	              *GPIOA_ODR &= ~(1U<<7);
	              *GPIOB_ODR |= (1U<<0);
	              uart2_send_string("SAFE\r\n");
	          }
	          else if (adc_val < 2200) {
	              *GPIOA_ODR &= ~(1U<<5);
	              *GPIOA_ODR |=  (1U<<6);
	              *GPIOA_ODR &= ~(1U<<7);
	              *GPIOB_ODR |=  (1U<<0);
	              uart2_send_string("WARNING\r\n");
	          }
	          else {
	              *GPIOA_ODR &= ~(1U<<5);
	              *GPIOA_ODR &= ~(1U<<6);
	              *GPIOA_ODR |=  (1U<<7);
	              *GPIOB_ODR &=  ~(1U<<0);
	              uart2_send_string("DANGER\r\n");
	          }
	          uart2_send_string("PB0=");
	                  uart2_send_number((*GPIOB_ODR >> 0) & 1);
	                  uart2_send_string("\r\n");


	          delay(500000);

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
#ifdef USE_FULL_ASSERT
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
