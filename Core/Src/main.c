/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include "ili9341.h"
#include "stm32f429i_discovery_lcd.h"
#include "l3gd20.h"
#include "stm32f429i_discovery_gyroscope.h"
#include "FirstOrderIIR.h"
#include "Obstacle.h"
#include "punkty.h"
#include "flash_f429zi.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */


/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
//#define ALPHA 0.043213918263772265
#define ALPHA 0.01229f
#define BETA_0 (1+ALPHA)/2.0
#define BETA_1 -(1+ALPHA)/2.0
#define DPS_SCALE_250 0.00875f
#define DPS_SCALE_500 0.01750f
#define DPS_SCALE_2000 0.070f
#define DPS_SCALE_USER 0.00095f
#define OBSTACLES_NUMBER 5
#define POINTS_NUMBER 2


#define BALL_Y 80
#define BALL_RAY 15

#define BEST_SCORE_ADDRESS 0x08010000
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
DMA2D_HandleTypeDef hdma2d;

I2C_HandleTypeDef hi2c3;

LTDC_HandleTypeDef hltdc;

SPI_HandleTypeDef hspi5;

TIM_HandleTypeDef htim6;

SDRAM_HandleTypeDef hsdram1;

/* USER CODE BEGIN PV */

GYRO_DrvTypeDef gyroscope;
LTDC_HandleTypeDef LtdcHandle;

__IO uint32_t ReloadFlag = 0;
__IO float X = 0;
__IO int gyro_flag = 0;
__IO int tryb = 0;
uint32_t punkty = 0;
uint32_t bestScore = 0;
FirstOrderIIR_t filter;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA2D_Init(void);
static void MX_FMC_Init(void);
static void MX_I2C3_Init(void);
static void MX_LTDC_Init(void);
static void MX_SPI5_Init(void);
static void MX_TIM6_Init(void);
/* USER CODE BEGIN PFP */
//static void LCD_Config(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
static void zmien_na_char(uint32_t pom,int x, int y);
static void Generate_Obstacles(ObstacleDef *obstacles, uint8_t NumberOfObjects, uint16_t width_limit, uint16_t height_limit , uint16_t gap);
static void Generate_Item(Item *point, uint8_t NumberOfPoints, uint16_t width, uint16_t height , uint16_t gap);
static void scoreFlashWrite(uint16_t Score);
static uint16_t scoreFlashRead(uint16_t *returnScore);
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	ObstacleDef obstacles[OBSTACLES_NUMBER];
	Item point[POINTS_NUMBER];
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  FirstOrderIIR_Init(&filter, ALPHA, BETA_0, BETA_1);
  srand(time(NULL));
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA2D_Init();
  MX_FMC_Init();
  MX_I2C3_Init();
  MX_LTDC_Init();
  MX_SPI5_Init();
  MX_TIM6_Init();
  /* USER CODE BEGIN 2 */
  BSP_LCD_Init();
  BSP_LCD_LayerDefaultInit(LCD_FOREGROUND_LAYER, LCD_FRAME_BUFFER);
  BSP_LCD_SelectLayer(LCD_FOREGROUND_LAYER);
  BSP_LCD_Clear(LCD_COLOR_BLACK);

  /* Configure 2 layers w/ Blending */
   /* Gyroscope init */
  if(BSP_GYRO_Init() != GYRO_OK)
  {
	  BSP_LCD_DisplayStringAtLine(1, (uint8_t*)"[FAIL]");
	  return -1;
  }
  BSP_GYRO_Reset();

  float Xpos = BSP_LCD_GetXSize()/2;
  uint16_t width_limit = 80;
  uint16_t height_limit = 10;

  uint16_t width = 10;
  uint16_t height= 10;

  uint16_t gap = 60;
  Generate_Obstacles(obstacles, OBSTACLES_NUMBER, width_limit, height_limit , gap);
  Generate_Item(point, POINTS_NUMBER, width, height, gap);

//  uint32_t tmp = 0;
//  Flash_Write_Data(BEST_SCORE_ADDRESS, &tmp, 1);


  HAL_TIM_Base_Start_IT(&htim6);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0))
	  {
		 tryb=1;
	  }
	  else
	  {
	  switch(tryb)
	  {
	  case 0:
		  BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		  BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
		  BSP_LCD_DisplayStringAt(0,BSP_LCD_GetYSize()/2-30, (uint8_t*)"NACISNIJ",CENTER_MODE);
		  BSP_LCD_DisplayStringAt(0,BSP_LCD_GetYSize()/2, (uint8_t*)"NIEBIESKI",CENTER_MODE);
		  BSP_LCD_DisplayStringAt(0,BSP_LCD_GetYSize()/2+30, (uint8_t*)"PRZYCISK",CENTER_MODE);

		  ReloadFlag = 0;
		  BSP_LCD_Relaod(LCD_RELOAD_VERTICAL_BLANKING);
		  while(ReloadFlag == 0) {} /* wait till reload takes effect */
		  HAL_Delay(10);
		  BSP_LCD_Clear(LCD_COLOR_BLACK);
		  punkty=0;
		  break;
	  case 1:
		BSP_LCD_Clear(LCD_COLOR_BLACK);
		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
		BSP_LCD_Clear(LCD_COLOR_BLACK);
		BSP_LCD_DisplayStringAt(0,BSP_LCD_GetYSize()/2-10, (uint8_t*)"3",CENTER_MODE);
		HAL_Delay(1000);
		BSP_LCD_Clear(LCD_COLOR_BLACK);
		BSP_LCD_DisplayStringAt(0,BSP_LCD_GetYSize()/2-10, (uint8_t*)"2",CENTER_MODE);
		HAL_Delay(1000);
		BSP_LCD_Clear(LCD_COLOR_BLACK);
		BSP_LCD_DisplayStringAt(0,BSP_LCD_GetYSize()/2-10, (uint8_t*)"1",CENTER_MODE);
		HAL_Delay(1000);
		BSP_LCD_Clear(LCD_COLOR_BLACK);
		BSP_LCD_DisplayStringAt(0,BSP_LCD_GetYSize()/2-10, (uint8_t*)"START",CENTER_MODE);
		HAL_Delay(1000);

		Generate_Obstacles(obstacles, OBSTACLES_NUMBER, width_limit, height_limit , gap);
		Generate_Item(point, POINTS_NUMBER, width, height, gap);

		tryb=2;
		  break;
	  case 2:

		  BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		  BSP_LCD_SetBackColor(LCD_COLOR_BLACK);

		  zmien_na_char(punkty,200,50);

		  if(gyro_flag == 1)
		  	  {
		  		  gyro_flag = 0;
		  		  Xpos += X;

		  		  if(Xpos > BSP_LCD_GetXSize() - 20)
		  			  Xpos = BSP_LCD_GetXSize() - 20;
		  		  else if(Xpos <20)
		  			  Xpos = 20;
		  	  }

	  //	  Obstacle_Overflow(obstacles, OBSTACLES_NUMBER);
	  //	  Obstacle_OverflowRandom(obstacles, OBSTACLES_NUMBER, width_limit);

		  	  ParityItem_OverflowRandom(point, POINTS_NUMBER, width_limit);
		  	  MultiItem_Move(point, POINTS_NUMBER, 0, -1);
		   	  MultiItem_Draw(point, POINTS_NUMBER);


		  	  ParityObstacle_OverflowRandom(obstacles, OBSTACLES_NUMBER, width_limit);
		  	  MultiObstacle_Move(obstacles, OBSTACLES_NUMBER, 0, -1);
		  	  MultiObstacle_Draw(obstacles, OBSTACLES_NUMBER);


		  	  BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
		  	  BSP_LCD_FillCircle(Xpos, BALL_Y, BALL_RAY);

		  	  if(IfCollisionDetect(obstacles, OBSTACLES_NUMBER, Xpos, BALL_Y, BALL_RAY))
		  	  {
		  		  tryb=3;
		  		  HAL_Delay(2000);
		  	  }
		  	  int score;
		  	  score=IfScore(point, POINTS_NUMBER, Xpos, BALL_Y, BALL_RAY);
		  	  punkty=punkty+score;

		  	 if(punkty>=50)
		  	 {
		  		tryb=4;
		  		HAL_Delay(500);
		  	  }

		  	  ReloadFlag = 0;
		  	  BSP_LCD_Relaod(LCD_RELOAD_VERTICAL_BLANKING);
		  	  while(ReloadFlag == 0) {} /* wait till reload takes effect */
		  	  BSP_LCD_Clear(LCD_COLOR_BLACK);

		  break;
	  case 3:
		  BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		  BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
		  BSP_LCD_DisplayStringAt(0,BSP_LCD_GetYSize()/2-30, (uint8_t*)"MALA",CENTER_MODE);
		  BSP_LCD_DisplayStringAt(0,BSP_LCD_GetYSize()/2, (uint8_t*)"KOLIZJA",CENTER_MODE);

		  ReloadFlag = 0;
 		  BSP_LCD_Relaod(LCD_RELOAD_VERTICAL_BLANKING);
		  while(ReloadFlag == 0) {} /* wait till reload takes effect */
		  HAL_Delay(1500);
		  BSP_LCD_Clear(LCD_COLOR_BLACK);
		  tryb=5;
		  break;
	  case 4:
		  BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		  BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
		  BSP_LCD_DisplayStringAt(0,BSP_LCD_GetYSize()/2-30, (uint8_t*)"GRATULACJE",CENTER_MODE);
		  BSP_LCD_DisplayStringAt(0,BSP_LCD_GetYSize()/2, (uint8_t*)"POKONALES",CENTER_MODE);
		  BSP_LCD_DisplayStringAt(0,BSP_LCD_GetYSize()/2+30, (uint8_t*)"MNIE",CENTER_MODE);

		  ReloadFlag = 0;
		  BSP_LCD_Relaod(LCD_RELOAD_VERTICAL_BLANKING);
		  while(ReloadFlag == 0) {} /* wait till reload takes effect */
		  HAL_Delay(1500);
		  BSP_LCD_Clear(LCD_COLOR_BLACK);
		  tryb=5;
		  break;
	  case 5:
		  HAL_TIM_Base_Stop_IT(&htim6);

		  Flash_Read_Data(BEST_SCORE_ADDRESS, &bestScore, 1);
		  BSP_LCD_DisplayStringAt(0,BSP_LCD_GetYSize()/2-120, (uint8_t*)"---------------",LEFT_MODE);
		  BSP_LCD_DisplayStringAt(0,BSP_LCD_GetYSize()/2-100, (uint8_t*)"Twoj Wynik",CENTER_MODE);
		  BSP_LCD_DisplayStringAt(0,BSP_LCD_GetYSize()/2-79, (uint8_t*)"---------------",LEFT_MODE);

		  zmien_na_char(punkty,BSP_LCD_GetXSize()/2-20,BSP_LCD_GetYSize()/2-45);

		  BSP_LCD_DisplayStringAt(0,BSP_LCD_GetYSize()/2+20, (uint8_t*)"---------------",LEFT_MODE);
		  BSP_LCD_DisplayStringAt(0,BSP_LCD_GetYSize()/2+41, (uint8_t*)"Best score",CENTER_MODE);
		  BSP_LCD_DisplayStringAt(0,BSP_LCD_GetYSize()/2+61, (uint8_t*)"---------------",LEFT_MODE);

		  zmien_na_char(bestScore, BSP_LCD_GetXSize()/2-20, BSP_LCD_GetYSize()/2+95);

		  if(punkty > bestScore)
		  {
			  Flash_Write_Data(BEST_SCORE_ADDRESS, &punkty, 1);
//			  bestScore=punkty;
		  }

		  ReloadFlag = 0;
		  BSP_LCD_Relaod(LCD_RELOAD_VERTICAL_BLANKING);
		  while(ReloadFlag == 0) {} /* wait till reload takes effect */
		  HAL_Delay(4000);
		  BSP_LCD_Clear(LCD_COLOR_BLACK);

		  FirstOrderIIR_Init(&filter, ALPHA, BETA_0, BETA_1);
		  HAL_TIM_Base_Start_IT(&htim6);

		  tryb=0;
		  break;
	  }
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 180;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief DMA2D Initialization Function
  * @param None
  * @retval None
  */
static void MX_DMA2D_Init(void)
{

  /* USER CODE BEGIN DMA2D_Init 0 */

  /* USER CODE END DMA2D_Init 0 */

  /* USER CODE BEGIN DMA2D_Init 1 */

  /* USER CODE END DMA2D_Init 1 */
  hdma2d.Instance = DMA2D;
  hdma2d.Init.Mode = DMA2D_M2M;
  hdma2d.Init.ColorMode = DMA2D_OUTPUT_RGB565;
  hdma2d.Init.OutputOffset = 0;
  hdma2d.LayerCfg[1].InputOffset = 0;
  hdma2d.LayerCfg[1].InputColorMode = DMA2D_INPUT_RGB565;
  hdma2d.LayerCfg[1].AlphaMode = DMA2D_NO_MODIF_ALPHA;
  hdma2d.LayerCfg[1].InputAlpha = 0;
  if (HAL_DMA2D_Init(&hdma2d) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_DMA2D_ConfigLayer(&hdma2d, 1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN DMA2D_Init 2 */

  /* USER CODE END DMA2D_Init 2 */

}

/**
  * @brief I2C3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C3_Init(void)
{

  /* USER CODE BEGIN I2C3_Init 0 */

  /* USER CODE END I2C3_Init 0 */

  /* USER CODE BEGIN I2C3_Init 1 */

  /* USER CODE END I2C3_Init 1 */
  hi2c3.Instance = I2C3;
  hi2c3.Init.ClockSpeed = 100000;
  hi2c3.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c3.Init.OwnAddress1 = 0;
  hi2c3.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c3.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c3.Init.OwnAddress2 = 0;
  hi2c3.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c3.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c3) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c3, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c3, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C3_Init 2 */

  /* USER CODE END I2C3_Init 2 */

}

/**
  * @brief LTDC Initialization Function
  * @param None
  * @retval None
  */
static void MX_LTDC_Init(void)
{

  /* USER CODE BEGIN LTDC_Init 0 */
	ili9341_Init();
  /* USER CODE END LTDC_Init 0 */

  LTDC_LayerCfgTypeDef pLayerCfg = {0};
  LTDC_LayerCfgTypeDef pLayerCfg1 = {0};

  /* USER CODE BEGIN LTDC_Init 1 */

  /* USER CODE END LTDC_Init 1 */
  hltdc.Instance = LTDC;
  hltdc.Init.HSPolarity = LTDC_HSPOLARITY_AL;
  hltdc.Init.VSPolarity = LTDC_VSPOLARITY_AL;
  hltdc.Init.DEPolarity = LTDC_DEPOLARITY_AL;
  hltdc.Init.PCPolarity = LTDC_PCPOLARITY_IPC;
  hltdc.Init.HorizontalSync = 9;
  hltdc.Init.VerticalSync = 1;
  hltdc.Init.AccumulatedHBP = 29;
  hltdc.Init.AccumulatedVBP = 3;
  hltdc.Init.AccumulatedActiveW = 269;
  hltdc.Init.AccumulatedActiveH = 323;
  hltdc.Init.TotalWidth = 279;
  hltdc.Init.TotalHeigh = 327;
  hltdc.Init.Backcolor.Blue = 0;
  hltdc.Init.Backcolor.Green = 0;
  hltdc.Init.Backcolor.Red = 0;
  if (HAL_LTDC_Init(&hltdc) != HAL_OK)
  {
    Error_Handler();
  }
  pLayerCfg.WindowX0 = 0;
  pLayerCfg.WindowX1 = 240;
  pLayerCfg.WindowY0 = 0;
  pLayerCfg.WindowY1 = 320;
  pLayerCfg.PixelFormat = LTDC_PIXEL_FORMAT_RGB565;
  pLayerCfg.Alpha = 0;
  pLayerCfg.Alpha0 = 0;
  pLayerCfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_CA;
  pLayerCfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_CA;
  pLayerCfg.FBStartAdress = FRAME_BUFFER_DEAFULT;
  pLayerCfg.ImageWidth = 0;
  pLayerCfg.ImageHeight = 0;
  pLayerCfg.Backcolor.Blue = 0;
  pLayerCfg.Backcolor.Green = 0;
  pLayerCfg.Backcolor.Red = 0;
  if (HAL_LTDC_ConfigLayer(&hltdc, &pLayerCfg, 0) != HAL_OK)
  {
    Error_Handler();
  }
  pLayerCfg1.WindowX0 = 0;
  pLayerCfg1.WindowX1 = 240;
  pLayerCfg1.WindowY0 = 0;
  pLayerCfg1.WindowY1 = 320;
  pLayerCfg1.PixelFormat = LTDC_PIXEL_FORMAT_RGB565;
  pLayerCfg1.Alpha = 0;
  pLayerCfg1.Alpha0 = 0;
  pLayerCfg1.BlendingFactor1 = LTDC_BLENDING_FACTOR1_CA;
  pLayerCfg1.BlendingFactor2 = LTDC_BLENDING_FACTOR2_CA;
  pLayerCfg1.FBStartAdress = 0;
  pLayerCfg1.ImageWidth = 0;
  pLayerCfg1.ImageHeight = 0;
  pLayerCfg1.Backcolor.Blue = 0;
  pLayerCfg1.Backcolor.Green = 0;
  pLayerCfg1.Backcolor.Red = 0;
  if (HAL_LTDC_ConfigLayer(&hltdc, &pLayerCfg1, 1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN LTDC_Init 2 */

  /* USER CODE END LTDC_Init 2 */

}

/**
  * @brief SPI5 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI5_Init(void)
{

  /* USER CODE BEGIN SPI5_Init 0 */

  /* USER CODE END SPI5_Init 0 */

  /* USER CODE BEGIN SPI5_Init 1 */

  /* USER CODE END SPI5_Init 1 */
  /* SPI5 parameter configuration*/
  hspi5.Instance = SPI5;
  hspi5.Init.Mode = SPI_MODE_MASTER;
  hspi5.Init.Direction = SPI_DIRECTION_2LINES;
  hspi5.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi5.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi5.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi5.Init.NSS = SPI_NSS_SOFT;
  hspi5.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  hspi5.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi5.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi5.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi5.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi5) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI5_Init 2 */

  /* USER CODE END SPI5_Init 2 */

}

/**
  * @brief TIM6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM6_Init(void)
{

  /* USER CODE BEGIN TIM6_Init 0 */

  /* USER CODE END TIM6_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM6_Init 1 */

  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 18000-1;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 100-1;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM6_Init 2 */

  /* USER CODE END TIM6_Init 2 */

}

/* FMC initialization function */
static void MX_FMC_Init(void)
{

  /* USER CODE BEGIN FMC_Init 0 */

  /* USER CODE END FMC_Init 0 */

  FMC_SDRAM_TimingTypeDef SdramTiming = {0};

  /* USER CODE BEGIN FMC_Init 1 */

  /* USER CODE END FMC_Init 1 */

  /** Perform the SDRAM1 memory initialization sequence
  */
  hsdram1.Instance = FMC_SDRAM_DEVICE;
  /* hsdram1.Init */
  hsdram1.Init.SDBank = FMC_SDRAM_BANK2;
  hsdram1.Init.ColumnBitsNumber = FMC_SDRAM_COLUMN_BITS_NUM_8;
  hsdram1.Init.RowBitsNumber = FMC_SDRAM_ROW_BITS_NUM_12;
  hsdram1.Init.MemoryDataWidth = FMC_SDRAM_MEM_BUS_WIDTH_16;
  hsdram1.Init.InternalBankNumber = FMC_SDRAM_INTERN_BANKS_NUM_4;
  hsdram1.Init.CASLatency = FMC_SDRAM_CAS_LATENCY_1;
  hsdram1.Init.WriteProtection = FMC_SDRAM_WRITE_PROTECTION_DISABLE;
  hsdram1.Init.SDClockPeriod = FMC_SDRAM_CLOCK_DISABLE;
  hsdram1.Init.ReadBurst = FMC_SDRAM_RBURST_DISABLE;
  hsdram1.Init.ReadPipeDelay = FMC_SDRAM_RPIPE_DELAY_0;
  /* SdramTiming */
  SdramTiming.LoadToActiveDelay = 16;
  SdramTiming.ExitSelfRefreshDelay = 16;
  SdramTiming.SelfRefreshTime = 16;
  SdramTiming.RowCycleDelay = 16;
  SdramTiming.WriteRecoveryTime = 16;
  SdramTiming.RPDelay = 16;
  SdramTiming.RCDDelay = 16;

  if (HAL_SDRAM_Init(&hsdram1, &SdramTiming) != HAL_OK)
  {
    Error_Handler( );
  }

  /* USER CODE BEGIN FMC_Init 2 */

  /* USER CODE END FMC_Init 2 */
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, NCS_MEMS_SPI_Pin|CSX_Pin|OTG_FS_PSO_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(ACP_RST_GPIO_Port, ACP_RST_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, RDX_Pin|WRX_DCX_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOG, LD3_Pin|LD4_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : NCS_MEMS_SPI_Pin CSX_Pin OTG_FS_PSO_Pin */
  GPIO_InitStruct.Pin = NCS_MEMS_SPI_Pin|CSX_Pin|OTG_FS_PSO_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PA0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : MEMS_INT1_Pin MEMS_INT2_Pin TP_INT1_Pin */
  GPIO_InitStruct.Pin = MEMS_INT1_Pin|MEMS_INT2_Pin|TP_INT1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : ACP_RST_Pin */
  GPIO_InitStruct.Pin = ACP_RST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(ACP_RST_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : OTG_FS_OC_Pin */
  GPIO_InitStruct.Pin = OTG_FS_OC_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(OTG_FS_OC_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : BOOT1_Pin */
  GPIO_InitStruct.Pin = BOOT1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BOOT1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : OTG_HS_ID_Pin OTG_HS_DM_Pin OTG_HS_DP_Pin */
  GPIO_InitStruct.Pin = OTG_HS_ID_Pin|OTG_HS_DM_Pin|OTG_HS_DP_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF12_OTG_HS_FS;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : VBUS_HS_Pin */
  GPIO_InitStruct.Pin = VBUS_HS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(VBUS_HS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : TE_Pin */
  GPIO_InitStruct.Pin = TE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(TE_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : RDX_Pin WRX_DCX_Pin */
  GPIO_InitStruct.Pin = RDX_Pin|WRX_DCX_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : STLINK_RX_Pin STLINK_TX_Pin */
  GPIO_InitStruct.Pin = STLINK_RX_Pin|STLINK_TX_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : LD3_Pin LD4_Pin */
  GPIO_InitStruct.Pin = LD3_Pin|LD4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

void zmien_na_char(uint32_t pom,int x,int y)
{
	if(pom<10)
	{
		char punkty=pom+'0';
		BSP_LCD_DisplayChar(x,y,punkty);
	}
	else
	{
		if(pom<20)
		{
			pom=pom%10;
			char punkty=pom+'0';
			char jeden=1+'0';
			BSP_LCD_DisplayChar(x,y,jeden);
			BSP_LCD_DisplayChar(x+15,y,punkty);
		}
		else
		{
			if(pom<30)
			{
				pom=pom%10;
				char punkty=pom+'0';
				char dwa=2 +'0';
				BSP_LCD_DisplayChar(x,y,dwa);
				BSP_LCD_DisplayChar(x+15,y,punkty);
			}
			else
			{
				if(pom<40)
				{
					pom=pom%10;
					char punkty=pom+'0';
					char trzy=3 +'0';
					BSP_LCD_DisplayChar(x,y,trzy);
					BSP_LCD_DisplayChar(x+15,y,punkty);
				}
				else
				{
					if(pom<50)
					{
						pom=pom%10;
						char punkty=pom+'0';
						char cztery=4 +'0';
						BSP_LCD_DisplayChar(x,y,cztery);
						BSP_LCD_DisplayChar(x+15,y,punkty);
					}
					else
					{
						if(pom<60)
						{
							pom=pom%10;
							char punkty=pom+'0';
							char piec=5 +'0';
							BSP_LCD_DisplayChar(x,y,piec);
							BSP_LCD_DisplayChar(x+15,y,punkty);
										}
									}
				}
			}
		}
	}

}

void Generate_Obstacles(ObstacleDef *obstacles, uint8_t NumberOfObjects, uint16_t width_limit, uint16_t height_limit, uint16_t gap)
{
	int16_t X;
	int16_t Y = BSP_LCD_GetYSize();
	uint16_t width;

	for(uint8_t i = 0; i < NumberOfObjects; ++i)
	{
		X = rand() % BSP_LCD_GetXSize() + 40;
		Y += gap;

		if(BSP_LCD_GetXSize() - X < 50)
			width = BSP_LCD_GetXSize() - X;
		else if(BSP_LCD_GetXSize() - X > BSP_LCD_GetXSize() - 50)
		{
			X = 0;
			width = rand() % width_limit;
		}
		else
			width = rand() % width_limit;
		Obstacle_Init(&obstacles[i], X, Y, width, height_limit);
	}
}


void Generate_Item(Item *point, uint8_t NumberOfPointss, uint16_t width_limit, uint16_t height_limit, uint16_t gap)
{
	int16_t X;
	int16_t Y = BSP_LCD_GetYSize();
	uint16_t width;

	for(uint8_t i = 0; i < NumberOfPointss; ++i)
	{
		int random;
		random=rand()%2;
		random=random+1;
		X = rand() % BSP_LCD_GetXSize() + 40;
		Y += (random*gap)+15;

		if(BSP_LCD_GetXSize() - X < 50)
			width = 10;
		else if(BSP_LCD_GetXSize() - X > BSP_LCD_GetXSize() - 50)
		{
			X = 0;
			width = 10;
		}
		else
			width = 10;
		Item_Init(&point[i], X, Y, width, height_limit);
	}
}

void scoreFlashWrite(uint16_t Score)
{
	uint32_t SECTORError;
	FLASH_EraseInitTypeDef eraseStruct;
	eraseStruct.TypeErase = FLASH_TYPEERASE_SECTORS;   /*!< Mass erase or sector Erase.*/
	eraseStruct.Sector = 	FLASH_SECTOR_20;      /*!< Initial FLASH sector to erase when Mass erase is disabled*/
	eraseStruct.NbSectors = 1;   /*!< Number of sectors to be erased.*/
	eraseStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;

	HAL_FLASH_Unlock();
	if(HAL_FLASHEx_Erase(&eraseStruct, &SECTORError) != HAL_OK)
	{
		BSP_LCD_DisplayStringAt(0,BSP_LCD_GetYSize()/2-30, (uint8_t*)"ERROR REMOVING PREVIOUS SCORE",CENTER_MODE);
		return;
	}

	if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, FLASH_SECTOR_20, (uint64_t)Score) != HAL_OK)
	{
		BSP_LCD_DisplayStringAt(0,BSP_LCD_GetYSize()/2-30, (uint8_t*)"ERROR SAVING NEW SCORE",CENTER_MODE);
		return;
	}
	HAL_FLASH_Lock();
}

uint16_t scoreFlashRead(uint16_t *returnScore)
{
	*returnScore = *(__IO uint16_t*)FLASH_SECTOR_20;
	return *returnScore;
}

/**
  * @brief  Reload Event callback.
  * @param  hltdc: pointer to a LTDC_HandleTypeDef structure that contains
  *                the configuration information for the LTDC.
  * @retval None
  */
void HAL_LTDC_ReloadEventCallback(LTDC_HandleTypeDef *hltdc)
{
	ReloadFlag = 1;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim == &htim6)
	{
		gyro_flag = 1;
		float XYZ[3];

		BSP_GYRO_GetXYZ(XYZ);

		X = FirstOrderIIR_Update(&filter, XYZ[1]) * DPS_SCALE_2000 * DPS_SCALE_USER;
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

