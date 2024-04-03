#pragma once

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32h7xx.h"
#include "stm32h7xx_hal.h"
#include "usbd_def.h"

void usbd_tasks_init(void);
void usbd_gs_can_queue_to_host_task(void *argument);
void usbd_gs_can_queue_from_host_task(void *argument);

#ifdef __cplusplus
}
#endif

