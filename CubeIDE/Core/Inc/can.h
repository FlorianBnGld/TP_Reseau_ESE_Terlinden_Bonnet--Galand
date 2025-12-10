/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    can.h
  * @brief   This file contains all the function prototypes for
  *          the can.c file
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CAN_H__
#define __CAN_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern CAN_HandleTypeDef hcan1;

/* USER CODE BEGIN Private defines */
CAN_TxHeaderTypeDef TxHeader;
uint8_t TxData[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
uint32_t TxMailbox;

TxHeader.StdId = 0x123;                 // Identifiant standard (11 bits)
TxHeader.ExtId = 0x01;                  // Ignoré car IDE = CAN_ID_STD
TxHeader.IDE = CAN_ID_STD;              // Trame standard
TxHeader.RTR = CAN_RTR_DATA;            // Trame de type DATA
TxHeader.DLC = 8;                       // Nombre d’octets
TxHeader.TransmitGlobalTime = DISABLE;  // Toujours DISABLE si non utilisé/* USER CODE END Private defines */

void MX_CAN1_Init(void);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __CAN_H__ */

