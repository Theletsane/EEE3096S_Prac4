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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "stm32f4xx.h"
#include "lcd_stm32f4.h"


/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
// TODO: Add values for below variables
#define NS 128      // Number of samples in LUT
#define TIM2CLK 16000000  // STM Clock frequency: Hint You might want to check the ioc file
#define F_SIGNAL 440	// Frequency of output analog signal
#define TIM2_Ticks (uint32_t)((TIM2CLK) / ((NS) * (F_SIGNAL)))
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* USER CODE BEGIN PV */
// Task 1: Global variables including LUTs
uint32_t Saw_LUT[NS] = {
	    0,    32,    64,    97,    129,    161,    193,    226,    258,    290,
	    322,    355,    387,    419,    451,    484,    516,    548,    580,    613,
	    645,    677,    709,    742,    774,    806,    838,    871,    903,    935,
	    967,    1000,    1032,    1064,    1096,    1129,    1161,    1193,    1225,    1258,
	    1290,    1322,    1354,    1386,    1419,    1451,    1483,    1515,    1548,    1580,
	    1612,    1644,    1677,    1709,    1741,    1773,    1806,    1838,    1870,    1902,
	    1935,    1967,    1999,    2031,    2064,    2096,    2128,    2160,    2193,    2225,
	    2257,    2289,    2322,    2354,    2386,    2418,    2451,    2483,    2515,    2547,
	    2580,    2612,    2644,    2676,    2709,    2741,    2773,    2805,    2837,    2870,
	    2902,    2934,    2966,    2999,    3031,    3063,    3095,    3128,    3160,    3192,
	    3224,    3257,    3289,    3321,    3353,    3386,    3418,    3450,    3482,    3515,
	    3547,    3579,    3611,    3644,    3676,    3708,    3740,    3773,    3805,    3837,
	    3869,    3902,    3934,    3966,    3998,    4031,    4063,    4095,
	};
uint32_t Triangle_LUT[NS] = {
    0,    64,    128,    192,    256,    320,    384,    448,    512,    576,
    640,    704,    768,    832,    896,    960,    1024,    1088,    1152,    1216,
    1280,    1344,    1408,    1472,    1536,    1600,    1664,    1728,    1792,    1856,
    1920,    1984,    2048,    2111,    2175,    2239,    2303,    2367,    2431,    2495,
    2559,    2623,    2687,    2751,    2815,    2879,    2943,    3007,    3071,    3135,
    3199,    3263,    3327,    3391,    3455,    3519,    3583,    3647,    3711,    3775,
    3839,    3903,    3967,    4031,    4095,    4031,    3967,    3903,    3839,    3775,
    3711,    3647,    3583,    3519,    3455,    3391,    3327,    3263,    3199,    3135,
    3071,    3007,    2943,    2879,    2815,    2751,    2687,    2623,    2559,    2495,
    2431,    2367,    2303,    2239,    2175,    2111,    2048,    1984,    1920,    1856,
    1792,    1728,    1664,    1600,    1536,    1472,    1408,    1344,    1280,    1216,
    1152,    1088,    1024,    960,    896,    832,    768,    704,    640,    576,
    512,    448,    384,    320,    256,    192,    128,    64,
};
uint32_t Piano_LUT[NS] = {
	     2047,  2164,  1980,  2100,  2058,  2078,  2044,  2139,  2062,  2077,
	     2067,  2051,  2008,  2404,  2073,  1768,  2042,  2045,  2012,  2058,
	     2003,  2100,  1521,  2044,  2303,  2050,  3034,  1587,  1747,  1834,
	     2294,  2259,  1515,  2207,  1942,  2070,  2024,  2079,  1780,  1899,
	     1947,  2061,  2232,  1784,  2126,  1901,  2875,  2042,  2090,  2065,
	     2056,  2035,  1969,  2037,  2770,  1940,  2405,  1772,  1937,  1992,
	     1135,  1909,  1636,  2051,  1674,  2025,  2167,  2058,  2032,  2014,
	     1855,  2130,  1970,  2080,  1739,  2514,  2224,  2025,  2352,  2241,
	     2054,  2107,  2013,  2057,  2052,  2033,  2237,  2064,  2077,  1918,
	     1860,  2030,  1996,  2247,  2049,  2127,  2414,  2031,  2109,  2065,
	     2084,  2040,  1667,  1861,  1925,  2032,  2178,  2128,  2009,  2020,
	     1838,  2188,  2037,  2078,  1985,  2058,  2080,  2044,  1926,  1862,
	     1624,  1615,  2097,  2263,  2043,  2104,  1956,  2047
	};

uint32_t Guitar_LUT[NS] = {
	     2047,  2249,  2365,  2161,  2264,  1943,  1855,  2243,  1994,  2114,
	     2067,  2029,  2067,  2204,  1756,  2062,  2042,  2076,  1786,  2470,
	     1928,  1929,  2512,  1849,  1767,  2041,  2020,  2053,  2037,  2026,
	     1958,  2059,  2052,  1991,  2007,  2136,  1880,  2176,  1984,  2034,
	     1892,  2063,  2061,  2061,  2055,  1932,  2137,  2041,  1901,  1906,
	     2283,  1866,  2311,  1964,  1887,  2319,  1993,  2159,  2086,  1782,
	     1980,  2141,  2845,  2014,  1876,  2121,  2142,  2256,  2221,  2379,
	     1682,  2340,  2089,  2020,  2078,  2062,  2374,  1909,  2061,  2054,
	     2157,  2166,  1792,  2154,  2084,  1670,  1982,  1729,  2095,  2083,
	     2038,  2034,  1960,  1918,  2090,  2046,  2379,  1833,  2352,  1851,
	     2154,  1861,  2378,  2469,  1987,  2049,  2017,  2053,  2138,  1907,
	     2055,  2046,  1677,  2183,  2051,  2007,  2241,  1999,  2003,  1736,
	     2088,  1786,  2368,  1935,  1695,  2595,  2065,  2045
	};

uint32_t Drum_LUT[NS] = {
	     2047,  3346,  1883,  2120,  2069,  3380,  1914,  1944,  2087,  2496,
	     2028,  2028,  2035,  2013,  1461,  1852,  2090,  2024,  2047,  2060,
	     2048,  2084,  2043,  2027,  2135,  1849,  2066,  2052,  2181,  2034,
	     3000,  1260,  1485,  2104,  2430,  2106,  1784,  2054,  2029,  2039,
	     4090,  1893,  2038,  2041,  2305,  2272,  1811,  1997,  2173,  2061,
	     2013,  2048,   764,  2414,  2084,  2074,    33,  2225,  2033,  2026,
	     2165,  2050,  2911,  2392,  4081,  1985,  2277,  2033,  2626,  2693,
	     2061,  2100,  1798,  2013,  2088,  2046,  2686,  1901,  2049,  2074,
	     2005,  2057,  2039,  2047,  2183,  2056,  2067,  2047,  2898,  2063,
	     2020,  2077,  2004,  2054,  1776,  2037,   791,  2279,  1969,  2035,
	     2041,  2041,  2088,  2040,  1706,  2058,  2062,  2030,  2050,  2409,
	     2147,  2232,  2126,  2044,  2039,  1727,  2146,  2409,  2132,  4094,
	     2149,  2011,  2042,  1633,  1970,   513,  2520,  2047
	};
uint32_t Sin_LUT[NS] = {
	    2048,    2148,    2248,    2348,    2447,    2545,    2642,    2737,    2831,    2923,
	    3013,    3100,    3185,    3267,    3346,    3423,    3495,    3565,    3630,    3692,
	    3750,    3804,    3853,    3898,    3939,    3975,    4007,    4034,    4056,    4073,
	    4085,    4093,    4095,    4093,    4085,    4073,    4056,    4034,    4007,    3975,
	    3939,    3898,    3853,    3804,    3750,    3692,    3630,    3565,    3495,    3423,
	    3346,    3267,    3185,    3100,    3013,    2923,    2831,    2737,    2642,    2545,
	    2447,    2348,    2248,    2148,    2048,    1947,    1847,    1747,    1648,    1550,
	    1453,    1358,    1264,    1172,    1082,    995,    910,    828,    749,    672,
	    600,    530,    465,    403,    345,    291,    242,    197,    156,    120,
	    88,    61,    39,    22,    10,    2,    0,    2,    10,    22,
	    39,    61,    88,    120,    156,    197,    242,    291,    345,    403,
	    465,    530,    600,    672,    749,    828,    910,    995,    1082,    1172,
	    1264,    1358,    1453,    1550,    1648,    1747,    1847,    1947,
	};
// Remove this line: uint32_t TIM2_Ticks = 0;  // DELETE THIS
/* USER CODE END PV */

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
DMA_HandleTypeDef hdma_tim2_ch1;

// TODO: Equation to calculate TIM2_Ticks
 // How often to write new LUT value

uint32_t DestAddress = (uint32_t) &(TIM3->CCR3); // Write LUT TO TIM3->CCR3 to modify PWM duty cycle
volatile uint8_t current_waveform = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
/* USER CODE BEGIN PFP */
void EXTI0_IRQHandler(void);
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
  MX_DMA_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  // TODO: Start TIM3 in PWM mode on channel 3
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
  // TODO: Start TIM2 in Output Compare (OC) mode on channel 1
  HAL_TIM_OC_Start(&htim2, TIM_CHANNEL_1);
  // TODO: Start DMA in IT mode on TIM2->CH1. Source is LUT and Dest is TIM3->CCR3; start with Sine LUT
  HAL_DMA_Start_IT(&hdma_tim2_ch1, (uint32_t)Sin_LUT, DestAddress, NS);
  // TODO: Write current waveform to LCD(Sine is the first waveform)
  init_LCD();
  lcd_command(CLEAR);
  lcd_putstring("Sine");
  // TODO: Enable DMA (start transfer from LUT to CCR)
  __HAL_TIM_ENABLE_DMA(&htim2, TIM_DMA_CC1);
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
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = TIM2_Ticks - 1;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_OC_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_TIMING;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_OC_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */
  /* TIM2_CH1 DMA Init */
  __HAL_RCC_DMA1_CLK_ENABLE();

  hdma_tim2_ch1.Instance = DMA1_Stream5;
  hdma_tim2_ch1.Init.Channel = DMA_CHANNEL_3;         // TIM2_CH1 is on channel 3
  hdma_tim2_ch1.Init.Direction = DMA_MEMORY_TO_PERIPH; // Memory -> TIM3->CCR3
  hdma_tim2_ch1.Init.PeriphInc = DMA_PINC_DISABLE;    // Peripheral address fixed
  hdma_tim2_ch1.Init.MemInc = DMA_MINC_ENABLE;        // Memory address increments
  hdma_tim2_ch1.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
  hdma_tim2_ch1.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
  hdma_tim2_ch1.Init.Mode = DMA_CIRCULAR;            // Repeat LUT automatically
  hdma_tim2_ch1.Init.Priority = DMA_PRIORITY_HIGH;
  hdma_tim2_ch1.Init.FIFOMode = DMA_FIFOMODE_DISABLE;

  if (HAL_DMA_Init(&hdma_tim2_ch1) != HAL_OK)
  {
      Error_Handler();
  }

  /* Link DMA handle to TIM2 handle */
  __HAL_LINKDMA(&htim2, hdma[TIM_DMA_ID_CC1], hdma_tim2_ch1);
  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 65535;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);

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
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  // -------------------------------
  // LCD pins configuration
  // -------------------------------
  // Configure PC14 (RS) and PC15 (E) as output push-pull
  GPIO_InitStruct.Pin = GPIO_PIN_14 | GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  // Configure PB8 (D4) and PB9 (D5) as output push-pull
  GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  // Configure PA12 (D6) and PA15 (D7) as output push-pull
  GPIO_InitStruct.Pin = GPIO_PIN_12 | GPIO_PIN_15;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  // Set all LCD pins LOW initially
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14 | GPIO_PIN_15, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8 | GPIO_PIN_9, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12 | GPIO_PIN_15, GPIO_PIN_RESET);


  // -------------------------------
  // Button0 configuration (PA0)
  // -------------------------------
  GPIO_InitStruct.Pin = Button0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING; // Interrupt on rising edge
  GPIO_InitStruct.Pull = GPIO_PULLUP;         // Use pull-up resistor
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  // Enable and set EXTI line 0 interrupt priority
  HAL_NVIC_SetPriority(EXTI0_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void EXTI0_IRQHandler(void){
    // Debounce using HAL_GetTick()
    static uint32_t last_interrupt_time = 0;
    uint32_t current_time = HAL_GetTick();
    
    // Only process if 200ms has passed since last interrupt (debouncing)
    if (current_time - last_interrupt_time > 200) {
        last_interrupt_time = current_time;
        
        // Disable DMA transfer and abort IT
        __HAL_TIM_DISABLE_DMA(&htim2, TIM_DMA_CC1);
        HAL_DMA_Abort_IT(&hdma_tim2_ch1);
        
        // Move to next waveform (cycle through 0-5)
        current_waveform = (current_waveform + 1) % 6;
        
        // Start DMA in IT mode with new LUT and update LCD
        switch(current_waveform) {
            case 0:  // Sine wave
                HAL_DMA_Start_IT(&hdma_tim2_ch1, (uint32_t)Sin_LUT, DestAddress, NS);
                lcd_command(CLEAR);
                lcd_putstring("Sine");
                break;
                
            case 1:  // Sawtooth wave
                HAL_DMA_Start_IT(&hdma_tim2_ch1, (uint32_t)Saw_LUT, DestAddress, NS);
                lcd_command(CLEAR);
                lcd_putstring("Sawtooth");
                break;
                
            case 2:  // Triangle wave
                HAL_DMA_Start_IT(&hdma_tim2_ch1, (uint32_t)Triangle_LUT, DestAddress, NS);
                lcd_command(CLEAR);
                lcd_putstring("Triangle");
                break;
                
            case 3:  // Piano sound
                HAL_DMA_Start_IT(&hdma_tim2_ch1, (uint32_t)Piano_LUT, DestAddress, NS);
                lcd_command(CLEAR);
                lcd_putstring("Piano");
                break;
                
            case 4:  // Guitar sound
                HAL_DMA_Start_IT(&hdma_tim2_ch1, (uint32_t)Guitar_LUT, DestAddress, NS);
                lcd_command(CLEAR);
                lcd_putstring("Guitar");
                break;
                
            case 5:  // Drum sound
                HAL_DMA_Start_IT(&hdma_tim2_ch1, (uint32_t)Drum_LUT, DestAddress, NS);
                lcd_command(CLEAR);
                lcd_putstring("Drum");
                break;
        }
        
        // Re-enable DMA transfer
        __HAL_TIM_ENABLE_DMA(&htim2, TIM_DMA_CC1);
    }
    
    // Clear interrupt flags (must be at the end)
    HAL_GPIO_EXTI_IRQHandler(Button0_Pin);
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
