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
#include "can.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdlib.h>
#include "shell.h"
#include "BMP280_simple.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define SLAVE_ADDRESS (uint16_t)(0x77 << 1)
#define ID 0xD0
#define SIZE_OF_1 1
#define SIZE_OF_2 2
#define CTRL_MEAS 0xF4
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
float K = 1.0f;


CAN_TxHeaderTypeDef TxHeader=
		{
		.StdId = 0x60,                  // Identifiant standard (11 bits)
		.ExtId = 0x00,                  // Ignoré car IDE = CAN_ID_STD
		.IDE = CAN_ID_STD,             // Trame standard
		.RTR = CAN_RTR_DATA,            // Trame de type DATA
		.DLC = 3,	                      // Nombre d’octets
		.TransmitGlobalTime = DISABLE  // Toujours DISABLE si non utilisé
		};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */




/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int __io_putchar(int ch)
{
	HAL_UART_Transmit(&huart2, (uint8_t*)&ch, 1, HAL_MAX_DELAY);
	HAL_UART_Transmit(&huart1, (uint8_t*)&ch, 1, HAL_MAX_DELAY);
	return ch;
}


int addition(int argc, char ** argv,h_shell_t *h_shell){
	int sum=0;
	for(int i=1;i<argc;i++){
		sum+=atoi(argv[i]);
	}
	printf("\r\nsum=%d\r\n",sum);
	return sum;
}
uint8_t drv_uart_receive(char * pData, uint16_t size){
	HAL_UART_Receive(&huart2,(uint8_t*)pData,size,HAL_MAX_DELAY);
	return 0;
}
uint8_t drv_uart_transmit(char * pData, uint16_t size){
	HAL_UART_Transmit(&huart2,(uint8_t*)pData,size,HAL_MAX_DELAY);
	return 0;
}
h_shell_t h_shell={
		.huart=&huart2,
		.drv_shell={
				.drv_shell_receive =drv_uart_receive,
				.drv_shell_transmit= drv_uart_transmit
		}

};

int GET_T(int argc, char ** argv,h_shell_t *h_shell){
	BMP280_S32_t temperature;
	temperature =BMP280_get_temperature();

	return 0;

}


int GET_P(int argc, char ** argv,h_shell_t *h_shell){
	BMP280_S32_t pression;
	pression =BMP280_get_pressure();

	return 0;
}


int SET_K(int argc, char ** argv, h_shell_t *h_shell){
    if(argc < 2){
        printf("Usage : SET_K <valeur>\r\n");
        return 1;
    }

    K = atof(argv[1]);  // conversion ASCII → float
    printf("Coefficient K mis à jour : %.3f\r\n", K);
    return 0;
}
int GET_K(int argc, char ** argv, h_shell_t *h_shell){
    printf("Coefficient K : %.3f\r\n", K);
    return 0;
}

int GET_A(int argc, char ** argv, h_shell_t *h_shell){

	return 0;
}

int ANGLE(int argc, char **argv, h_shell_t *h_shell)
{
    if (argc < 3) {
        printf("Usage : A <angle_decimal>\r\n");
        return 1;
    }

    int angle = atoi(argv[1]);
    int sens = atoi(argv[2]);

    if (angle < 0 || angle > 180) {
        printf("Angle must be between 0 and 180\r\n");
        return 1;
    }

    uint8_t TxData[3];
    TxData[0] = (uint8_t)sens;             // Adresse ou commande fixe
    TxData[1] = (uint8_t)angle;   // L’angle demandé
    TxData[2] = 0x04;             // Commande moteur

    uint32_t TxMailbox;

    /* Envoi */
    HAL_StatusTypeDef ret =
        HAL_CAN_AddTxMessage(&hcan1, &TxHeader, TxData, &TxMailbox);

    if (ret != HAL_OK) {
        printf("HAL_CAN_AddTxMessage FAILED: %d\r\n", (int)ret);
        return 1;
    }

    printf("Message queued in mailbox %lu\r\n", (unsigned long)TxMailbox);

    /* Attendre la fin (polling) */
    uint32_t timeout = HAL_GetTick() + 100;

    while ((__HAL_CAN_GET_FLAG(&hcan1, CAN_FLAG_RQCP0) == RESET) &&
           (__HAL_CAN_GET_FLAG(&hcan1, CAN_FLAG_RQCP1) == RESET) &&
           (__HAL_CAN_GET_FLAG(&hcan1, CAN_FLAG_RQCP2) == RESET))
    {
        if (HAL_GetTick() > timeout) {
            printf("TX timeout waiting mailbox complete\r\n");
            break;
        }
    }

    printf("TX send attempt complete\r\n");
    return 0;
}


void taskShell(void *unused){
	shell_init(&h_shell);
    shell_add('a', addition, "Ma super addition", &h_shell);
    shell_add('t', GET_T, "Température compensée", &h_shell);
    shell_add('p', GET_P, "Pression compensée", &h_shell);
    shell_add('k', SET_K, "Fixe le coefficient K", &h_shell);
    shell_add('K', GET_K, "Affiche K", &h_shell);
    shell_add('A', GET_A, "Affiche l'angle", &h_shell);
    shell_add('m', ANGLE, "set motor angle", &h_shell);
	shell_run(&h_shell);//shell_run contient une boucle infinie donc on ne retournera jamais de cette fonction
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

	if (huart->Instance==USART1){
		shell_uart_rx_callback(&h_shell);
	}
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
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_USART1_UART_Init();
  MX_CAN1_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
  printf("============== TP_Réseaux/Bus ==============\r\n         Terlinden & Bonnet--Galand\r\n");
  BMP280_init();
  int32_t raw_t;
  int32_t raw_p;
  BMP280_S32_t t_compensate;
  BMP280_S32_t p_compensate;

  BMP280_Read_Raw(&hi2c1, &raw_t, &raw_p);
  t_compensate = bmp280_compensate_T_int32((BMP280_S32_t) raw_t);
  p_compensate = bmp280_compensate_P_int64((BMP280_S32_t) raw_p);
  printf("RAW : T=%ld P=%ld | FINAL : T=%.2f C, P=%.2f hPa\r\n", raw_t, raw_p, raw_t/20000.0f, p_compensate/25600.0f);


  /* Démarrer le CAN */
  if (HAL_CAN_Start(&hcan1) != HAL_OK) {
      printf("HAL_CAN_Start FAILED\r\n");
      Error_Handler();
  } else {
      printf("CAN started OK\r\n");
  }

  /* Optionnel : activer notifications pour debug (Tx mailbox empty + bus off + error) */
  if (HAL_CAN_ActivateNotification(&hcan1,
          CAN_IT_TX_MAILBOX_EMPTY | CAN_IT_BUSOFF | CAN_IT_ERROR) != HAL_OK) {
      printf("HAL_CAN_ActivateNotification FAILED\r\n");
  }

  /* Préparer et envoyer le message */
  uint8_t TxData[3] = {0x00, 0x64, 0x04};
  uint32_t TxMailbox;

  /* envoyer et loguer le code de retour */
  HAL_StatusTypeDef ret = HAL_CAN_AddTxMessage(&hcan1, &TxHeader, TxData, &TxMailbox);
  if (ret != HAL_OK) {
      printf("HAL_CAN_AddTxMessage FAILED: %d\r\n", (int)ret);
      Error_Handler();
  } else {
      printf("Message queued in mailbox %lu\r\n", (unsigned long)TxMailbox);
  }

  /* Attend (poll) la sortie du mailbox (optionnel simple) */
  uint32_t timeout = HAL_GetTick() + 100; // 100 ms timeout
  while ((__HAL_CAN_GET_FLAG(&hcan1, CAN_FLAG_RQCP0) == RESET) &&
         (__HAL_CAN_GET_FLAG(&hcan1, CAN_FLAG_RQCP1) == RESET) &&
         (__HAL_CAN_GET_FLAG(&hcan1, CAN_FLAG_RQCP2) == RESET)) {
      if (HAL_GetTick() > timeout) {
          printf("TX timeout waiting mailbox complete\r\n");
          break;
      }
  }
  printf("TX send attempt complete (check bus)\r\n");



  /*
  HAL_StatusTypeDef status;
  uint8_t reg_address = ID; // 0xD0
  uint8_t i2c_rx_buffer = 0;
  status = HAL_I2C_Master_Transmit(&hi2c1, SLAVE_ADDRESS, &reg_address, SIZE, HAL_MAX_DELAY);

  if (status == HAL_OK)
  {
      status = HAL_I2C_Master_Receive(&hi2c1, SLAVE_ADDRESS, &i2c_rx_buffer, SIZE, HAL_MAX_DELAY);

      if (status == HAL_OK)
      {
          // Reading succeed.
          printf("ID read (0x%02X) : 0x%02X\r\n", ID, i2c_rx_buffer);
      }
      else
      {
          // Reading failed.
          printf("CKC - Reading failed after pointing the register.\r\n");
      }
  }
  else
  {
      printf("CKC - Transmission of the address register failed.\r\n");
  }
  */

  	/*
  	HAL_StatusTypeDef status;

  	uint8_t buffer[2];
  	buffer[0] = CTRL_MEAS;
  	buffer[1] = 0b01010111;

	uint8_t i2c_rx_buffer = 0;
	status = HAL_I2C_Master_Transmit(&hi2c1, SLAVE_ADDRESS, buffer, SIZE_OF_2, HAL_MAX_DELAY);

	if (status == HAL_OK)
	{
		status = HAL_I2C_Master_Receive(&hi2c1, SLAVE_ADDRESS, &i2c_rx_buffer, SIZE_OF_2, HAL_MAX_DELAY);

		if (status == HAL_OK)
		{
			// Reading succeed.
			printf("ID read (0x%02X) : 0x%02X\r\n", CTRL_MEAS, i2c_rx_buffer);
		}
		else
		{
			// Reading failed.
			printf("CKC - Reading failed after pointing the register.\r\n");
		}
	}
	else
	{
		printf("CKC - Transmission of the address register failed.\r\n");
	}

	*/

	if (xTaskCreate(
				taskShell,             // fonction
				"SHELL",                // nom
				512,                  // stack (en mots, pas en octets)
				NULL,                 // paramètre
				1, // priorité
				NULL                  // handle (optionnel)
	)!=pdPASS){
		printf("Error creating task shell\r\n");
		Error_Handler();
	}






	vTaskStartScheduler();


  /* USER CODE END 2 */

  /* Call init function for freertos objects (in cmsis_os2.c) */
  MX_FREERTOS_Init();

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

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
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
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
