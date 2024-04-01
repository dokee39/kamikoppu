/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : usb_device.c
  * @version        : v1.0_Cube
  * @brief          : This file implements the USB Device
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

#include "usb_device.h"
#include "fdcan.h"
#include "main.h"
#include "stm32h7xx_hal_fdcan.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_cdc.h"
#include "usbd_cdc_if.h"

/* USER CODE BEGIN Includes */
#include "usbd_gs_can.h"

/* USER CODE END Includes */

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USB Device Core handle declaration. */
USBD_HandleTypeDef hUSB;
USBD_GS_CAN_HandleTypeDef hGS_CAN;

/*
 * -- Insert your variables declaration here --
 */
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*
 * -- Insert your external function declaration here --
 */
/* USER CODE BEGIN 1 */
extern FDCAN_HandleTypeDef hfdcan1;
extern FDCAN_HandleTypeDef hfdcan2;
extern FDCAN_HandleTypeDef hfdcan3;

void usbd_gs_can_queue_to_host_task(void *argument);
void usbd_gs_can_queue_from_host_task(void *argument);
static void usbd_class_gs_can_init(void);

void usbd_gs_can_queue_to_host_task(void *argument)
{
    UNUSED(argument);
    struct gs_host_frame_object frame_object;

    /* Infinite loop */
    for (;;) {
        /* Check the queue to see if we have data TO the host to handle */
        if (xQueueReceive(hGS_CAN.queue_to_hostHandle, &frame_object.frame, portMAX_DELAY) == pdPASS) {
            if (USBD_GS_CAN_SendFrame(&hUSB, &frame_object.frame) == USBD_OK) {
                can_on_rx_cb(frame_object.frame.channel, &frame_object.frame);
            }
            else {
                /* throw the message back onto the queue */
                if (uxQueueSpacesAvailable(hGS_CAN.queue_to_hostHandle) == 0) {
                    /* the pipe to the host is full - delay longer to allow for
                     * catchup if needed */
                    vTaskDelay(pdMS_TO_TICKS(10));
                }
                else {
                    xQueueSendToFront(hGS_CAN.queue_to_hostHandle, &frame_object.frame, 0);
                    vTaskDelay(pdMS_TO_TICKS(0));
                }
            }
        }
    }
}

void usbd_gs_can_queue_from_host_task(void *argument)
{
    UNUSED(argument);
    struct gs_host_frame_object frame_object;

    /* Infinite loop */
    for (;;) {
        /* Check the queue to see if we have data FROM the host to handle */
        if (xQueueReceive(hGS_CAN.queue_from_hostHandle, &frame_object.frame, portMAX_DELAY) == pdPASS) {
#if defined(LIN_FEATURE_ENABLED)
            if (IS_LIN_FRAME(frame_object.frame.can_id)) {
                lin_process_frame(&frame_object.frame);
                continue; /* just loop again so this frame isn't sent on CAN bus
                           */
            }
#endif /* LIN_FEATURE_ENABLED */
            if (can_send(hGS_CAN.channels[frame_object.frame.channel], &frame_object.frame)) {
                /* Echo sent frame back to host */
                frame_object.frame.reserved = 0x0;
                xQueueSendToBack(hGS_CAN.queue_to_hostHandle, &frame_object.frame, 0);
                can_on_tx_cb(frame_object.frame.channel, &frame_object.frame);
            }
            else {
                /* throw the message back onto the queue */
                if (uxQueueSpacesAvailable(hGS_CAN.queue_from_hostHandle) == 0) {
                    /* the pipe to the host is full - delay longer to allow for
                     * catchup if needed */
                    vTaskDelay(pdMS_TO_TICKS(10));
                }
                else {
                    xQueueSendToFront(hGS_CAN.queue_from_hostHandle, &frame_object.frame, 0);
                    vTaskDelay(pdMS_TO_TICKS(0));
                }
            }
        }
    }
}

void usbd_class_gs_can_init(void)
{
	hGS_CAN.channels[0] = &hfdcan1;
	hGS_CAN.channels[1] = &hfdcan2;
	hGS_CAN.channels[2] = &hfdcan3;
    // MX_FDCAN1_Init();
    // MX_FDCAN2_Init();
    // MX_FDCAN3_Init();
	// can_init(hGS_CAN.channels[0], FDCAN1);
	// can_init(hGS_CAN.channels[1], FDCAN2);
	// can_init(hGS_CAN.channels[2], FDCAN3);
	// led_init(&hled1, LED1_GPIO_Port, LED1_Pin, LED_MODE_INACTIVE, LED_ACTIVE_LOW);
	// host_channel_is_active = false;
}
/* USER CODE END 1 */

/**
  * Init USB device Library, add supported class and start the library
  * @retval None
  */
void MX_USB_DEVICE_Init(void)
{
  /* USER CODE BEGIN USB_DEVICE_Init_PreTreatment */
    usbd_class_gs_can_init();

  /* USER CODE END USB_DEVICE_Init_PreTreatment */

  /* Init Device Library, add supported class and start the library. */
  if (USBD_Init(&hUSB, &HS_Desc, DEVICE_HS) != USBD_OK)
  {
    Error_Handler();
  }
  if (USBD_RegisterClass(&hUSB, &USBD_GS_CAN) != USBD_OK)
  {
    Error_Handler();
  }
  if (USBD_Start(&hUSB) != USBD_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN USB_DEVICE_Init_PostTreatment */
  HAL_PWREx_EnableUSBVoltageDetector();

  /* USER CODE END USB_DEVICE_Init_PostTreatment */
}

/**
  * @}
  */

/**
  * @}
  */

