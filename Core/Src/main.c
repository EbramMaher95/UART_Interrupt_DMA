/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
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
UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_rx;

/* USER CODE BEGIN PV */

//define the uart used
UART_HandleTypeDef huart1;

//function declaration for the UART module to be used in interrupt mode
void on_data_received(UART_HandleTypeDef *config_it);

//function declaration
void RxEvent(UART_HandleTypeDef *huart, uint16_t length);

//string is set as volatile because it will be used in interrupt mode
//variable for the string to store the name
volatile char str[100];

//variable for the index in the string
volatile uint8_t str_index = 0;

//variable to store the received byte to determine if it is Enter button or not
volatile char ByteReceived;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {

	/* USER CODE BEGIN 1 */

	//enable the clock for Port C
	__HAL_RCC_GPIOC_CLK_ENABLE();
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
	MX_USART1_UART_Init();
	/* USER CODE BEGIN 2 */

	//register callback for the UART module
	//HAL_UART_RegisterCallback(&huart1, HAL_UART_RX_COMPLETE_CB_ID,on_data_received);

	//the RxEvent pointer is pointing to the RxEvnet (the function (function callback) i created)
	//so once it is triggered, it points to the function -> execute this function -> linked together
	huart1.RxEventCallback = RxEvent;

	//asking the user to enter his name
	HAL_UART_Transmit(&huart1, (uint8_t*) "Enter your name\r\n", 18, 100);

	//enable the Receive mode in interrupt mode to initialize the interrupt once data is received
	//data will be received in the string str and it will receive only one byte
	HAL_UARTEx_ReceiveToIdle_DMA(&huart1, (uint8_t*) &str, 1);

	//set pin 13 on port C as output (connected to LED on the board)
	GPIO_InitTypeDef PC13 = { .Pin = GPIO_PIN_13, .Mode = GPIO_MODE_OUTPUT_PP,
			.Speed = GPIO_SPEED_FREQ_HIGH,

	};

	//initialize PIN 13 on PORT C
	HAL_GPIO_Init(GPIOC, &PC13);

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) //on the while loop pin is toggled every 500 ms and when data is received interrupt is enabled
	{

		//toggle the pin
		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);

		//delay for 500 ms
		HAL_Delay(500);

		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

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
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
		Error_Handler();
	}
}

/**
 * @brief USART1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART1_UART_Init(void) {

	/* USER CODE BEGIN USART1_Init 0 */

	/* USER CODE END USART1_Init 0 */

	/* USER CODE BEGIN USART1_Init 1 */

	/* USER CODE END USART1_Init 1 */
	huart1.Instance = USART1;
	huart1.Init.BaudRate = 115200;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart1) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN USART1_Init 2 */

	/* USER CODE END USART1_Init 2 */

}

/**
 * Enable DMA controller clock
 */
static void MX_DMA_Init(void) {

	/* DMA controller clock enable */
	__HAL_RCC_DMA1_CLK_ENABLE();

	/* DMA interrupt init */
	/* DMA1_Channel5_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);

}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void) {
	/* USER CODE BEGIN MX_GPIO_Init_1 */
	/* USER CODE END MX_GPIO_Init_1 */

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();

	/* USER CODE BEGIN MX_GPIO_Init_2 */
	/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

//write the interrupt function handler
void on_data_received(UART_HandleTypeDef *config_it) {

	//when the enter button is pressed -> display the entered name
	if (ByteReceived == '\r' || ByteReceived == '\n') {

		//display the contents of the str string
		HAL_UART_Transmit(&huart1, (uint8_t*) "\r\nYour name is: ", 16, 100);
		HAL_UART_Transmit(&huart1, (uint8_t*) str, str_index, 100);
		HAL_UART_Transmit(&huart1, (uint8_t*) "\r\n", 2, 100);

		// Reset the index for the next input
		str_index = 0;

		// ask the user to enter the name again
		HAL_UART_Transmit(&huart1, (uint8_t*) "Enter your name: ", 17, 100);
	}

	//another button is pressed -> store the character into the string of the name
	else {
		//store received byte in the string
		str[str_index] = ByteReceived;

		//increment the string index by 1
		str_index++;
	}

	//wait for another data to be received again
	HAL_UART_Receive_IT(&huart1, (uint8_t*) &ByteReceived, 1);

}
;

//function definition
//this function is executed once the stream of data is sent and nothing is received (IDLE)
//the data stream is saved into the string
void RxEvent(UART_HandleTypeDef *huart, uint16_t length) {

	//display the name
	HAL_UART_Transmit(&huart1, (uint8_t*) "\r\nYour name is: ", 16, 100);

	//the length variable is used in this function
	//because it is returned from the string length
	HAL_UART_Transmit(&huart1, (uint8_t*) str, length, 100);

}

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
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
