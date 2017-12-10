/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2017 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_hal.h"
#include "adc.h"
#include "dma.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "fmc.h"

/* USER CODE BEGIN Includes */
#include "tm_stm32_hd44780.h"
#include "ads1256_test.h"
#include "tm_stm32f4_keypad.h"
#include "tm_stm32f4_ili9341.h"
#include "stm32f429i_discovery_sdram.h"
#include "main.h"
#include "midi.h"
#include "string.h"
#include "math.h"
#include "drums.h"
#include "stdlib.h"
#include "audio.h"
#include "SF3.h"
#define pi 3.14159
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

__IO BOOL Recording = FALSE;
__IO BOOL Playback = FALSE;
__IO BOOL Overdubbing = FALSE;
__IO FUNCTION ToggleFunction = NONE;
__IO BOOL StartApp = FALSE;
__IO BOOL clipping = FALSE;
uint8_t footswitch = 0;


extern __IO uint8_t midiMetronome;
extern __IO uint8_t midiRecording;
extern __IO uint8_t midiPlayback;
extern __IO uint32_t midiDrumClock;
extern __IO uint32_t midiDrumPointer;
extern __IO uint32_t midiMetronomeClock;

extern uint8_t key_to_drum[];
extern uint8_t midiEvents[];
extern uint32_t midiTimes[];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/


/* USER CODE END PFP */

/* USER CODE BEGIN 0 */


extern char SD_Path[];
TM_KEYPAD_Button_t Keypad_Button;

/* USER CODE END 0 */

int main(void)
{

  /* USER CODE BEGIN 1 */
	uint32_t data;
	char keystr[4];
	uint8_t sf3_ID[20];
	HAL_StatusTypeDef status;

	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
	DWT->CYCCNT = 0;
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;


  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

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
  MX_TIM3_Init();
  MX_TIM2_Init();
  MX_FMC_Init();
  MX_TIM4_Init();
  MX_TIM8_Init();
  MX_SPI3_Init();
  MX_USART1_UART_Init();
  MX_ADC1_Init();
  MX_ADC3_Init();
  MX_SPI5_Init();

  /* USER CODE BEGIN 2 */

  TM_ILI9341_Init();
     //Rotate LCD for 90 degrees
     TM_ILI9341_Rotate(TM_ILI9341_Orientation_Landscape_1);
     //FIll lcd with color
     TM_ILI9341_Fill(ILI9341_COLOR_MAGENTA);
     //Draw white circle
     TM_ILI9341_DrawCircle(60, 60, 40, ILI9341_COLOR_GREEN);
     //Draw red filled circle
     TM_ILI9341_DrawFilledCircle(60, 60, 35, ILI9341_COLOR_RED);
     //Draw blue rectangle
     TM_ILI9341_DrawRectangle(120, 20, 220, 100, ILI9341_COLOR_BLUE);
     //Draw black filled rectangle
     TM_ILI9341_DrawFilledRectangle(130, 30, 210, 90, ILI9341_COLOR_BLACK);
     //Draw line with custom color 0x0005
     TM_ILI9341_DrawLine(10, 120, 310, 120, 0x0005);

     //Put string with black foreground color and blue background with 11x18px font
     TM_ILI9341_Puts(65, 130, "STM32F4 Discovery", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
     //Put string with black foreground color and blue background with 11x18px font
     TM_ILI9341_Puts(60, 150, "ILI9341 LCD Module", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
     //Put string with black foreground color and red background with 11x18px font
     TM_ILI9341_Puts(245, 225, "majerle.eu", &TM_Font_7x10, ILI9341_COLOR_BLACK, ILI9341_COLOR_ORANGE);


  //SF3_CS1();

  //ST7735_init();
  //ST7735_pushColor(&color,1);
 // hspi2.Init.DataSize = SPI_DATASIZE_16BIT;
//  while(1){
//	  LCD_CS_LOW();
//	  HAL_SPI_Transmit(&hspi2,(uint8_t *)&dat,2,100);
//	  LCD_CS_HIGH();
//	  HAL_Delay(100);
//
//  }

  HAL_NVIC_DisableIRQ(EXTI2_IRQn);
  BSP_SDRAM_Init();
  status = HAL_TIM_Base_Start_IT(&htim2);

  BSP_LED_Init(LED_GREEN);
  BSP_LED_Init(LED_RED);


  BSP_PB_Init(BUTTON_KEY, BUTTON_MODE_EXTI);

  //status = HAL_ADC_Start_DMA(&hadc3,(uint32_t *)readADC,2);
  //status = HAL_TIM_Base_Start_IT(&htim8);
  status = HAL_TIM_Base_Start_IT(&htim4);
  //status = HAL_DAC_Start(&hdac,DAC_CHANNEL_1);
  //status = HAL_DAC_Start(&hdac,DAC_CHANNEL_2);
  //status = HAL_ADC_Start_IT(&hadc1);
 // status = HAL_ADC_Start_IT(&hadc3);
  //status = HAL_ADC_Start_DMA(&hadc3,(uint32_t *)readADC,2);
  //ReadDrumSamples();

  ADS1256_WriteCmd(CMD_RESET);
  ADS1256_WriteCmd(CMD_SDATAC);

  data = ADS1256_ReadChipID();
  ADS1256_CfgADC(ADS1256_GAIN_16, ADS1256_15000SPS);
  ADS1256_WriteCmd(CMD_SELFCAL);
  ADS1256_SetChannel(0);
  HAL_NVIC_EnableIRQ(EXTI2_IRQn);
  ADS1256_WriteCmd(CMD_RDATAC);
  //FATFS_UnLinkDriver(SD_Path);
  TM_KEYPAD_Init();
  setupMidi();
  talkMIDI(0xB0, 0, 0x01); //Default bank GM1

  TM_HD44780_Init(20, 4);
  TM_HD44780_Clear();
  TM_HD44780_Puts(0,0,"Looper");
  utoa(data,keystr,10);
  TM_HD44780_Puts(0,1,keystr);

  //getDeviceID(sf3_ID);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  while (1)
  {
	if(Overdubbing == 1 && clipping == TRUE)
		BSP_LED_On(LED_RED);

	 Keypad_Button = TM_KEYPAD_Read();
	          /* Keypad was pressed */
	          if (Keypad_Button != TM_KEYPAD_Button_NOPRESSED) {/* Keypad is pressed */
	        	  switch (Keypad_Button) {
	                  case TM_KEYPAD_Button_0:        /* Button 0 pressed */
	                  case TM_KEYPAD_Button_1:        /* Button 1 pressed */
	                  case TM_KEYPAD_Button_2:        /* Button 2 pressed */
	                  case TM_KEYPAD_Button_3:        /* Button 3 pressed */
	                  case TM_KEYPAD_Button_4:        /* Button 4 pressed */
	                  case TM_KEYPAD_Button_5:        /* Button 5 pressed */
	                  case TM_KEYPAD_Button_6:        /* Button 6 pressed */
	                  case TM_KEYPAD_Button_7:        /* Button 7 pressed */
	                  case TM_KEYPAD_Button_8:        /* Button 8 pressed */
	                  case TM_KEYPAD_Button_9:        /* Button 9 pressed */
	                	  	  	utoa(Keypad_Button,keystr,10);
	                	  		TM_HD44780_Clear();
	                	  		TM_HD44780_Puts(0,0,keystr);
//	                	 if(midiRecording == 0){
//	                		  midiDrumPointer = 0;
//	                		  midiDrumClock = 0;
//	                		  midiRecording = 1;
//	                		  midiPlayback = 0;
//	                	  }
	                	  //recordPercussionEvent(Keypad_Button);
	                	  playPercussion(NOTEON,key_to_drum[Keypad_Button]);
//	                	  midiDrumPointer++;
//	                	  if(midiDrumPointer == MIDI_QUEUE){
//	                		  midiEvents[MIDI_QUEUE - 1] = No_Event;
//	                		  midiTimes[MIDI_QUEUE - 1] = midiDrumClock;
//	                		  midiDrumClock = 0;
//	                		  midiDrumPointer = 0;
//	                	  }
	                	  break;
	                  case TM_KEYPAD_Button_STAR:        /* Button STAR pressed */
//	                	  if(midiRecording == 1){
//	                		  midiEvents[midiDrumPointer] = No_Event;
//	                		  midiTimes[midiDrumPointer] = midiDrumClock;
//	                		  midiRecording = 0;
//	                		  midiPlayback = 1;
//	                		  midiDrumClock = 0;
//	                		  midiDrumPointer = 0;
//	                	  }
	                      break;
	                  case TM_KEYPAD_Button_HASH:        /* Button HASH pressed */
	                	  if(midiMetronome == 0){
	                		  //playPercussion(NOTEON,Metronome_Bell);
	                		  midiMetronome = 1;
	                	  }
	                	  else
	                		  midiMetronome = 0;
	                	  midiMetronomeClock = 0;
	                      break;
	                  case TM_KEYPAD_Button_A:        /* Button A pressed, only on large keyboard */
	                      /* Do your stuff here */
	                	 break;
	                  case TM_KEYPAD_Button_B:        /* Button B pressed, only on large keyboard */
	                      /* Do your stuff here */
	                      break;
	                  case TM_KEYPAD_Button_C:        /* Button C pressed, only on large keyboard */
	                      /* Do your stuff here */
	                      break;
	                  case TM_KEYPAD_Button_D:        /* Button D pressed, only on large keyboard */
	                      /* Do your stuff here */
	                      break;
	                  default:
	                      break;
	              } // end of switch

	        	  }// end of key pressed

//	          if(midiMetronome == 1 && midiMetronomeClock >= 10000){
//	        	  midiMetronomeClock = 0;
//	        	  playPercussion(NOTEON,Metronome_Bell);
//	          }
//
//	          if(midiPlayback == 1 && midiDrumClock >= midiTimes[midiDrumPointer]){
//	          	playPercussionEvent();
//	          	if(midiEvents[midiDrumPointer] == No_Event){
//	          		midiDrumPointer = 0;
//	          		midiDrumClock = 0;
//	          	}
//	          	else
//	          		midiDrumPointer++;

//	          }


  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */

	}
  /* USER CODE END 3 */

}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Configure the main internal regulator output voltage 
    */
  __HAL_RCC_PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 180;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 8;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Activate the Over-Drive mode 
    */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USER CODE BEGIN 4 */

/**
 * @brief  Perform the SDRAM exernal memory inialization sequence
 * @param  hsdram: SDRAM handle
 * @param  Command: Pointer to SDRAM command structure
 * @retval None
 */


void buttonHandler() {
	if(IS_BUT_DOWN(BUT_REC) && HAL_GPIO_ReadPin(Recording_GPIO_Port,Recording_Pin) == GPIO_PIN_SET )
		BUT_UP(BUT_REC);

	if(IS_BUT_DOWN(BUT_PLAY) && HAL_GPIO_ReadPin(Playback_GPIO_Port,Playback_Pin) == GPIO_PIN_SET )
		BUT_UP(BUT_PLAY);

	if(IS_BUT_DOWN(BUT_OVERDUB) && HAL_GPIO_ReadPin(Overdubbing_GPIO_Port,Overdubbing_Pin) == GPIO_PIN_SET )
		BUT_UP(BUT_OVERDUB);

	if(IS_BUT_DOWN(BUT_TOGFUN) && HAL_GPIO_ReadPin(ToggleFunction_GPIO_Port,ToggleFunction_Pin) == GPIO_PIN_SET )
		BUT_UP(BUT_TOGFUN);
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void _Error_Handler(char * file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1) 
  {
  }
  /* USER CODE END Error_Handler_Debug */ 
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
			/* User can add his own implementation to report the file name and line number,
			 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
