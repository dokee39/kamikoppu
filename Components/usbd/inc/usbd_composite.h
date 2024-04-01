#ifndef USBD_COMPOSITE_H
#define USBD_COMPOSITE_H
#include "usbd_def.h"

#define WBVAL(x) (x & 0xFF),((x >> 8) & 0xFF)

#define USBD_IAD_DESC_SIZE           0x08
#define USBD_IAD_DESCRIPTOR_TYPE     0x0B

#define USBD_COMPOSITE_DESC_SIZ (9 + 8 + 41 + 8 + 58) // cchere

extern USBD_ClassTypeDef USBD_CMPSIT;
#ifdef USE_USBD_COMPOSITE
void USBD_CMPSIT_AddClass(USBD_HandleTypeDef *pdev, USBD_ClassTypeDef *pclass, USBD_CompositeClassTypeDef classtype, uint8_t *EpAddr);
#endif
#endif
