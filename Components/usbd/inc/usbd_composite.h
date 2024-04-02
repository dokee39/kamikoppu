#ifndef USBD_COMPOSITE_H
#define USBD_COMPOSITE_H
#include "usbd_def.h"
#include "usbd_cdc.h"
#include "usbd_cdc_if.h"
#include "usbd_gs_can.h"

#define WBVAL(x) (x & 0xFF),((x >> 8) & 0xFF)

#define USBD_IAD_DESC_SIZE           0x08
#define USBD_IAD_DESCRIPTOR_TYPE     0x0B

#define USBD_COMPOSITE_DESC_SIZ (9 + 8 + 41 + 8 + 58) // cchere

#define USBD_INTERFACE_NUM USBD_MAX_NUM_INTERFACES

#define USBD_GS_CAN_CLASSID 0
#define USBD_GS_CAN_DATA_INTERFACE 0x00
#define USBD_GS_CAN_DATA_INTERFACE_EP_NUM 0x02
#define USBD_GS_CAN_DFU_INTERFACE 0x01
#define USBD_GS_CAN_DFU_INTERFACE_EP_NUM 0x00
#define USBD_GS_CAN_FIRST_INTERFACE USBD_GS_CAN_DATA_INTERFACE
#define USBD_GS_CAN_INTERFACE_NUM 0x02

#define USBD_CDC_CLASSID 1
#define USBD_CDC_CMD_INTERFACE 0x02
#define USBD_CDC_CMD_INTERFACE_EP_NUM 0x01
#define USBD_CDC_DATA_INTERFACE 0x03
#define USBD_CDC_DATA_INTERFACE_EP_NUM 0x02
#define USBD_CDC_FIRST_INTERFACE USBD_CDC_CMD_INTERFACE
#define USBD_CDC_INTERFACE_NUM 0x02


extern USBD_ClassTypeDef USBD_CMPSIT;


void USBD_Composite_Switch_GS_CAN(USBD_HandleTypeDef *pdev);
void USBD_Composite_Switch_CDC(USBD_HandleTypeDef *pdev);
#ifdef USE_USBD_COMPOSITE
void USBD_CMPSIT_AddClass(USBD_HandleTypeDef *pdev, USBD_ClassTypeDef *pclass, USBD_CompositeClassTypeDef classtype, uint8_t *EpAddr);
#endif

#endif
