/*

The MIT License (MIT)

Copyright (c) 2016 Hubert Denkmair
Copyright (c) 2022 Ryan Edwards (changes for STM32G0/G4 and CAN-FD)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CAN_H
#define __CAN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include <stdint.h>
#include <stdbool.h>
#include "gs_usb.h"

#if defined (FDCAN1)
#define CAN_HANDLE_TYPEDEF FDCAN_HandleTypeDef
#define CAN_TYPEDEF		   FDCAN_GlobalTypeDef
#else
#define CAN_HANDLE_TYPEDEF CAN_HandleTypeDef
#define CAN_TYPEDEF		   CAN_TypeDef
#endif

/* USER CODE BEGIN */
#define BOARD_SYSMEM_RESET_VECTOR	 0x1FFF0000
#define DFU_INTERFACE_STRING		 (uint8_t*) "budgetcan_h7 DFU interface"

#define FDCAN_SJW_INIT				 2
#define FDCAN_BRP_INIT				 6
#define FDCAN_TS1_INIT				 14
#define FDCAN_TS2_INIT				 5

#define FDCAN_DATA_SJW_INIT			 1
#define FDCAN_DATA_BRP_INIT			 1
#define FDCAN_DATA_TS1_INIT			 1
#define FDCAN_DATA_TS2_INIT			 1

#define FDCAN_FRAME_FMT_INIT		 FDCAN_FRAME_CLASSIC
#define FDCAN_MODE_INIT				 FDCAN_MODE_NORMAL
#define FDCAN_AUTO_RETX_INIT		 DISABLE
#define FDCAN_AUTO_TX_PAUSE_INIT	 DISABLE
#define FDCAN_PROT_EXCPTN_INIT		 DISABLE
#define FDCAN_STD_FLTR_NUM_INIT		 1
#define FDCAN_EXT_FLTR_NUM_INIT		 0

#define FDCAN_TX_FIFO_OPERATION_INIT FDCAN_TX_FIFO_OPERATION
#define FDCAN_MSG_RAM_OFST_INIT		 0
#define FDCAN_RX_FIFO0_ELEM_NUM_INIT 32
#define FDCAN_RX_FIFO0_ELEM_SZ_INIT	 FDCAN_DATA_BYTES_8
#define FDCAN_RX_FIFO1_ELEM_NUM_INIT 0
#define FDCAN_RX_FIFO1_ELEM_SZ_INIT	 FDCAN_DATA_BYTES_8
#define FDCAN_RX_BUFF_NUM_INIT		 0
#define FDCAN_RX_BUFF_SZ_INIT		 FDCAN_DATA_BYTES_8
#define FDCAN_TX_EVNT_NUM_INIT		 0
#define FDCAN_TX_BUFF_NUM_INIT		 0
#define FDCAN_TX_FIFO_ELEM_NUM_INIT	 32
#define FDCAN_TX_FIFO_ELEM_SZ_INIT	 FDCAN_DATA_BYTES_8

#define CAN_NUM_CHANNELS			 3
#define CAN_CLOCK_SPEED				 120000000
// #define CANFD_FEATURE_ENABLED

#define BOARD_TIM2_PRESCALER   64-1

#define QUEUE_SIZE_HOST_TO_DEV 32
#define QUEUE_SIZE_DEV_TO_HOST 32

/* USER CODE END */
    
/* Exported functions --------------------------------------------------------*/
void can_init(CAN_HANDLE_TYPEDEF *hcan, CAN_TYPEDEF *instance);
void can_set_bittiming(CAN_HANDLE_TYPEDEF *hcan, uint16_t brp, uint8_t phase_seg1, uint8_t phase_seg2, uint8_t sjw);
void can_set_data_bittiming(CAN_HANDLE_TYPEDEF *hcan, uint16_t brp, uint8_t phase_seg1, uint8_t phase_seg2, uint8_t sjw);
void can_enable(CAN_HANDLE_TYPEDEF *hcan, bool loop_back, bool listen_only, bool one_shot, bool can_mode_fd);
void can_disable(CAN_HANDLE_TYPEDEF *hcan);
bool can_is_enabled(CAN_HANDLE_TYPEDEF *hcan);
bool can_send(CAN_HANDLE_TYPEDEF *hcan, struct gs_host_frame *frame);
void can_set_termination(uint8_t channel, uint8_t value);
uint8_t can_get_termination(uint8_t channel);

void can_on_enable_cb(uint8_t channel);
void can_on_disable_cb(uint8_t channel);
void can_on_tx_cb(uint8_t channel, struct gs_host_frame *frame);
void can_on_rx_cb(uint8_t channel, struct gs_host_frame *frame);
void can_identify_cb(uint32_t do_identify);
void can_set_term_cb(uint8_t channel, GPIO_PinState state);
GPIO_PinState can_get_term_cb(uint8_t channel);

#ifdef __cplusplus
}
#endif

#endif /* __CAN_H */
