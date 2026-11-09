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


/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "main.h" //defines pins from CubeMX
#include "i2c.h" // defines hi2c1 and MX_I2C1_Init()
#include "spi.h" // defines hspi1 and MX_SPI1_Init()
#include "usart.h" // defines huart1 for dubugging
#include "gpio.h" // GPIO
#include "st7735.h"
#include "fonts.h"
#include <stdio.h>
#include "header_image.h"

/* USER CODE END Includes */


/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void ST7735_DrawImageTransparent(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t *data, uint16_t transparent)
{
  for (uint16_t row = 0; row < h; row++)
  {
    for (uint16_t col = 0; col < w; col++)
    {
      uint16_t color = data[row * w + col];
     // color = (color >> 8) | (color << 8);   // keep the byte swap

      if (color != transparent)              // skip transparent pixels
      {
        ST7735_DrawPixel(x + col, y + row, color);
      }
    }
  }
}

int _write(int file, char *ptr, int len)
{
  HAL_UART_Transmit(&huart1, (uint8_t*)ptr, len, HAL_MAX_DELAY);
  return len;
}

float LM75_ReadTemperature(void)
{
  uint8_t data[2];
  int16_t raw;

  if (HAL_I2C_Mem_Read(&hi2c1, 0x48 <<1, 0x00, I2C_MEMADD_SIZE_8BIT, data, 2, 1000) != HAL_OK)
    return -999.0f;

  raw = (data[0] << 8) | data[1];
  raw >>= 5;

  if (raw & 0x400)
    raw |= 0xF800;

  return raw * 0.125f;
}

/* USER CODE END 0 */


int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();
  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_SET);
  ST7735_Init();
  ST7735_InvertColors(true);
  ST7735_FillScreen(ST7735_BLACK);
  ST7735_DrawImageTransparent(0, 0, HEADER_W, HEADER_H, header_logo, ST7735_BLACK);
  ST7735_WriteString(30,37, "Temperature", Font_7x10, ST7735_WHITE, ST7735_BLACK);


char lastC[16] = "        "; // spaces
char lastF[16] = "        ";
char lastK[16] = "        ";

// Used for I2C error
//  char buf[20];
//  uint8_t y = 30;
//  uint8_t found = 0;
//
//  for (uint8_t addr = 1; addr < 128; addr++)
//  {
//    if (HAL_I2C_IsDeviceReady(&hi2c1, addr << 1, 2, 10) == HAL_OK)
//    {
//      sprintf(buf, "0x%02X", addr);
//      ST7735_WriteString(5, y, buf, Font_11x18, ST7735_GREEN, ST7735_BLACK);
//      y += 20;
//      found++;
//      if (y > 140) break;   // don't run off the screen
//    }
//  }

//  if (found == 0)
//  {
//    ST7735_WriteString(5, 30, "None found", Font_11x18, ST7735_RED, ST7735_BLACK);
//  }

    /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  float tempC = LM75_ReadTemperature();

	    if (tempC > -100.0f)
	    {
	      float tempF = tempC * 9.0f / 5.0f + 32.0f;
	      float tempK = tempC + 273.15f;

	      char bufC[16], bufF[16], bufK[16];
	      sprintf(bufC, "%5.1f C", tempC);
	      sprintf(bufF, "%5.1f F", tempF);
	      sprintf(bufK, "%5.1f K", tempK);

	      // Update only changed digits for Celsius
	      for (int i = 0; i < 8; i++)
	      {
	        if (bufC[i] != lastC[i])
	        {
	          ST7735_FillRectangle(10 + i * 12, 50, 12, 18, ST7735_BLACK);
	          char s[2] = { bufC[i], '\0' };
	          ST7735_WriteString(10 + i * 12, 50, s, Font_11x18, ST7735_WHITE, ST7735_BLACK);
	          lastC[i] = bufC[i];
	        }
	      }

	      // Fahrenheit
	      for (int i = 0; i < 8; i++)
	      {
	        if (bufF[i] != lastF[i])
	        {
	          ST7735_FillRectangle(10 + i * 12, 90, 12, 18, ST7735_BLACK);
	          char s[2] = { bufF[i], '\0' };
	          ST7735_WriteString(10 + i * 12, 90, s, Font_11x18, ST7735_WHITE, ST7735_BLACK);
	          lastF[i] = bufF[i];
	        }
	      }

	      // Kelvin
	      for (int i = 0; i < 8; i++)
	      {
	        if (bufK[i] != lastK[i])
	        {
	          ST7735_FillRectangle(10 + i * 12, 130, 12, 18, ST7735_BLACK);
	          char s[2] = { bufK[i], '\0' };
	          ST7735_WriteString(10 + i * 12, 130, s, Font_11x18, ST7735_WHITE, ST7735_BLACK);
	          lastK[i] = bufK[i];
	        }
	      }
	    }

	    HAL_Delay(500);


  }
    /* USER CODE END WHILE */
}


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
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
  /* USER CODE END Error_Handler_Debug */

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
