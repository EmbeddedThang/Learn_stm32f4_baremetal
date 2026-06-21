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
#include "string.h"
#include <stdio.h>
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
uint8_t rev_data=0;
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
void spi_GPIO_init(void);
void spi_conf_init(void);
void CS_gyro(char state);
uint8_t SPI_recive();
void SPI_transmit(uint8_t data);
uint8_t gyro_regRead8(uint8_t reg);
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
  spi_GPIO_init();
  spi_conf_init();
  rev_data=gyro_regRead8(0x0f);
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

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
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}
void spi_GPIO_init()
{
	// SP1_SCK: PA5 SP1_MISO: PA6 SP1_MOSI: PA7(AF05) CS: GPIO_Output: PE3
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();
	GPIOA->MODER &=~(0x3f<<10);
	GPIOA->MODER |= (GPIO_MODER_MODE5_1)|GPIO_MODER_MODE6_1|GPIO_MODER_MODE7_1;
	GPIOE->MODER &=~(GPIO_MODER_MODE3);
	GPIOE->MODER |= GPIO_MODER_MODE3_0;
	GPIOA->AFR[0] &=~(0xfff<<20);
	GPIOA->AFR[0] |= (0b0101<<20)|(0b0101<<24)|(0b0101<<28);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, GPIO_PIN_SET);
}
void spi_conf_init()
{
	__HAL_RCC_SPI1_CLK_ENABLE();
	SPI1->CR1 |= SPI_CR1_SSM | SPI_CR1_SSI;  //Disable Software slave management and Internal slave select
	SPI1->CR1 &=~(SPI_CR1_LSBFIRST); //MSB
	//Gyro maxspeed 10Mhz
	SPI1->CR1 &=~SPI_CR1_BR; //16/2Mhz
	SPI1->CR1 &=~SPI_CR1_MSTR;
	SPI1->CR1 |= SPI_CR1_MSTR;
	SPI1->CR1 &= ~SPI_CR1_RXONLY;
	SPI1->CR1 &=~(SPI_CR1_CPHA|SPI_CR1_CPOL);// clock polarity and clock phase
	SPI1->CR1 |= SPI_CR1_CPHA|SPI_CR1_CPOL;
	SPI1->CR1 &=~(SPI_CR1_DFF);// data frame format 8bit
	SPI1->CR1 |= SPI_CR1_SPE; // enable spi

}
void CS_gyro(char state)
{
	if(state)
	{
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, SET);
	}
	else{
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, RESET);
	}
}
void SPI_transmit(uint8_t data)
{
	while(!(SPI1->SR&(SPI_SR_TXE))){}
	SPI1->DR = data;
	while(!(SPI1->SR&(SPI_SR_RXNE))){}
	(void)SPI1->DR;
	while(SPI1->SR&(SPI_SR_BSY)){}

}
uint8_t SPI_recive()
{
	uint8_t data;
	while(!(SPI1->SR&(SPI_SR_TXE))){}
	SPI1->DR = 0xff;
	while(!(SPI1->SR&(SPI_SR_RXNE))){}
	data = SPI1->DR;
	while(SPI1->SR&(SPI_SR_BSY)){}
	return data;
}
uint8_t gyro_regRead8(uint8_t reg)
{
	CS_gyro(0);
	reg = (reg & 0x3F)|0x80;
	SPI_transmit(reg);
	uint8_t data;
	data = SPI_recive();
	CS_gyro(1);
	return data;
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
