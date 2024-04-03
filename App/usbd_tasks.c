#include "usbd_tasks.h"
#include "fdcan.h"
#include "main.h"
#include "stm32h7xx_hal_fdcan.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_cdc.h"
#include "usbd_cdc_if.h"

#include "usbd_gs_can.h"
#include "usbd_composite.h"

USBD_HandleTypeDef hUSB;
extern USBD_GS_CAN_HandleTypeDef hGS_CAN;

void usbd_gs_can_queue_to_host_task(void *argument);
void usbd_gs_can_queue_from_host_task(void *argument);

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

void usbd_tasks_init(void)
{
  if (USBD_Init(&hUSB, &HS_Desc, DEVICE_HS) != USBD_OK)
  {
    Error_Handler();
  }
  if (USBD_RegisterClass(&hUSB, &USBD_CMPSIT) != USBD_OK)
  {
    Error_Handler();
  }
  if (USBD_Start(&hUSB) != USBD_OK)
  {
    Error_Handler();
  }

  HAL_PWREx_EnableUSBVoltageDetector();
}

