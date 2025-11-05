/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "semphr.h"
#include <stdio.h>
#include "usart.h"
//#include "adc.h"
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
/* USER CODE BEGIN Variables */
uint32_t adc_value; 				//global Variables
/* USER CODE END Variables */
/* Definitions for Task_Show */
osThreadId_t Task_ShowHandle;
const osThreadAttr_t Task_Show_attributes = {
  .name = "Task_Show",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for BinSem_DataReady */
osSemaphoreId_t BinSem_DataReadyHandle;
const osSemaphoreAttr_t BinSem_DataReady_attributes = {
  .name = "BinSem_DataReady"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void AppTask_Show(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* creation of BinSem_DataReady */
  BinSem_DataReadyHandle = osSemaphoreNew(1, 1, &BinSem_DataReady_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of Task_Show */
  Task_ShowHandle = osThreadNew(AppTask_Show, NULL, &Task_Show_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_AppTask_Show */
/**
  * @brief  Function implementing the Task_Show thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_AppTask_Show */
void AppTask_Show(void *argument)
{
  /* USER CODE BEGIN AppTask_Show */
  /* Infinite loop */
  for(;;)
  {
		if (xSemaphoreTake(BinSem_DataReadyHandle, portMAX_DELAY)==pdTRUE)
		{
			uint32_t  tmpValue=adc_value;
			printf("ADC  Value = %ld.\r\n",tmpValue);		//show ADC initiate value

			uint32_t  Volt=3300 * tmpValue;						//mV
			Volt=Volt>>12;												//divided by 2^12
			printf("Voltage(mV)= %ld.\r\n",Volt);				//show ADC initiate value
		}
  }
  /* USER CODE END AppTask_Show */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	if (hadc->Instance == ADC3)
	{
		adc_value=HAL_ADC_GetValue(hadc); 				//ADC convert initiate value
		BaseType_t  highTaskWoken=pdFALSE;
		if (BinSem_DataReadyHandle != NULL)
		{
			xSemaphoreGiveFromISR(BinSem_DataReadyHandle, &highTaskWoken);
			portYIELD_FROM_ISR(highTaskWoken); 		//Task Scheduling
		}
	}
}

int __io_putchar(int ch)
{
	HAL_UART_Transmit(&huart3,(uint8_t*)&ch,1,0xFFFF);
	return ch;
}
/* USER CODE END Application */

