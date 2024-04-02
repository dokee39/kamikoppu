#include "usbd_composite.h"
#include "usbd_cdc.h"


USBD_GS_CAN_HandleTypeDef hGS_CAN;
USBD_GS_CAN_HandleTypeDef *phGS_CAN;
USBD_CDC_HandleTypeDef hCDC;
USBD_CDC_HandleTypeDef *phCDC;

// extern uint8_t  USBD_CDC_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
// extern uint8_t  USBD_CDC_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
// extern uint8_t  USBD_CDC_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);
// extern uint8_t  USBD_CDC_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum);
// extern uint8_t  USBD_CDC_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum);
// extern uint8_t  USBD_CDC_EP0_RxReady(USBD_HandleTypeDef *pdev);

static uint8_t  USBD_Composite_Init (USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static uint8_t  USBD_Composite_DeInit (USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static uint8_t  USBD_Composite_Setup (USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);
static uint8_t  USBD_Composite_DataIn (USBD_HandleTypeDef *pdev, uint8_t epnum);
static uint8_t  USBD_Composite_DataOut (USBD_HandleTypeDef *pdev, uint8_t epnum);
static uint8_t  USBD_Composite_SOF (USBD_HandleTypeDef *pdev);
static uint8_t  *USBD_Composite_GetHSCfgDesc (uint16_t *length);
static uint8_t  *USBD_Composite_GetFSCfgDesc (uint16_t *length);
static uint8_t  *USBD_Composite_GetOtherSpeedCfgDesc (uint16_t *length);
static uint8_t  *USBD_Composite_GetDeviceQualifierDescriptor (uint16_t *length);
static uint8_t USBD_Composite_EP0_RxReady (USBD_HandleTypeDef *pdev);
void USBD_Composite_Switch_GS_CAN(USBD_HandleTypeDef *pdev);
void USBD_Composite_Switch_CDC(USBD_HandleTypeDef *pdev);

USBD_ClassTypeDef  USBD_CMPSIT =
{
  USBD_Composite_Init,
  USBD_Composite_DeInit,
  USBD_Composite_Setup,
  NULL, /*EP0_TxSent*/
  USBD_Composite_EP0_RxReady, /*EP0_RxReady*/
  USBD_Composite_DataIn,
  USBD_Composite_DataOut,
  USBD_Composite_SOF, /*SOF */
  NULL,
  NULL,
  USBD_Composite_GetHSCfgDesc,
  USBD_Composite_GetFSCfgDesc,
  USBD_Composite_GetOtherSpeedCfgDesc,
  USBD_Composite_GetDeviceQualifierDescriptor,
};

/*   All Descriptors (Configuration, Interface, Endpoint, Class, Vendor */
__ALIGN_BEGIN uint8_t USBD_Composite_CfgDesc[USBD_COMPOSITE_DESC_SIZ] __ALIGN_END =
{
  /*Configuration Descriptor*/
  0x09,   /* bLength: Configuration Descriptor size */
  USB_DESC_TYPE_CONFIGURATION,      /* bDescriptorType: Configuration */
  LOBYTE(USBD_COMPOSITE_DESC_SIZ),   /* wTotalLength:no of returned bytes */ // cchere
  HIBYTE(USBD_COMPOSITE_DESC_SIZ),
  USBD_INTERFACE_NUM,   /* bNumInterfaces: 4 interface */ /* cdc:2 gs_can:2 */ // cchere
  0x01,   /* bConfigurationValue: Configuration value */
  0x04,   /* iConfiguration: Index of string descriptor describing the configuration */
#if (USBD_SELF_POWERED == 1U)
  0xC0,                                       /* bmAttributes: Bus Powered according to user configuration */
#else
  0x80,                                       /* bmAttributes: Bus Powered according to user configuration */
#endif /* USBD_SELF_POWERED */
  USBD_MAX_POWER,                             /* MaxPower (mA) */

  /** gs_can *******************************************************************/
  /* Interface Association Descriptor for GS_CAN */ // cchere
  USBD_IAD_DESC_SIZE,  // bLength: Interface Descriptor size，固定值
  USBD_IAD_DESCRIPTOR_TYPE,  // bDescriptorType: IAD，固定值
  USBD_GS_CAN_FIRST_INTERFACE,  // bFirstInterface，第一个接口的起始序号，从0开始 // cchere
  USBD_GS_CAN_INTERFACE_NUM,  // bInterfaceCount，本IAD下的接口数量
  0x00,                           /* bInterfaceClass: Vendor Specific*/
  0x00,                           /* bInterfaceSubClass: Vendor Specific */
  0x00,                           /* bInterfaceProtocol: Vendor Specific */
  0X00,  // iFunction

	/*---------------------------------------------------------------------------*/
	/* GS_USB Interface Descriptor */
	0x09,                           /* bLength */
	USB_DESC_TYPE_INTERFACE,        /* bDescriptorType */
	USBD_GS_CAN_DATA_INTERFACE,                           /* bInterfaceNumber */ // cchere
	0x00,                           /* bAlternateSetting */
	USBD_GS_CAN_DATA_INTERFACE_EP_NUM,                           /* bNumEndpoints */
	0xFF,                           /* bInterfaceClass: Vendor Specific*/
	0xFF,                           /* bInterfaceSubClass: Vendor Specific */
	0xFF,                           /* bInterfaceProtocol: Vendor Specific */
	0x00,                           /* iInterface */
	/*---------------------------------------------------------------------------*/

	/*---------------------------------------------------------------------------*/
	/* EP1 descriptor */
	0x07,                           /* bLength */
	USB_DESC_TYPE_ENDPOINT,         /* bDescriptorType */
	GSUSB_ENDPOINT_IN,              /* bEndpointAddress */
	0x02,                           /* bmAttributes: bulk */
	LOBYTE(CAN_DATA_MAX_PACKET_SIZE), /* wMaxPacketSize */
	HIBYTE(CAN_DATA_MAX_PACKET_SIZE),
	0x00,                           /* bInterval: */
	/*---------------------------------------------------------------------------*/

	/*---------------------------------------------------------------------------*/
	/* EP2 descriptor */
	0x07,                           /* bLength */
	USB_DESC_TYPE_ENDPOINT,         /* bDescriptorType */
	GSUSB_ENDPOINT_OUT,             /* bEndpointAddress */
	0x02,                           /* bmAttributes: bulk */
	LOBYTE(CAN_DATA_MAX_PACKET_SIZE), /* wMaxPacketSize */
	HIBYTE(CAN_DATA_MAX_PACKET_SIZE),
	0x00,                           /* bInterval: */
	/*---------------------------------------------------------------------------*/

	/*---------------------------------------------------------------------------*/
	/* DFU Interface Descriptor */
	/*---------------------------------------------------------------------------*/
	0x09,                           /* bLength */
	USB_DESC_TYPE_INTERFACE,        /* bDescriptorType */
	USBD_GS_CAN_DFU_INTERFACE,              /* bInterfaceNumber */ // cchere
	0x00,                           /* bAlternateSetting */
	USBD_GS_CAN_DFU_INTERFACE_EP_NUM,                           /* bNumEndpoints */
	0xFE,                           /* bInterfaceClass: Vendor Specific*/
	0x01,                           /* bInterfaceSubClass */
	0x01,                           /* bInterfaceProtocol : Runtime mode */
	DFU_INTERFACE_STR_INDEX,        /* iInterface */

	/*---------------------------------------------------------------------------*/
	/* Run-Time DFU Functional Descriptor */
	/*---------------------------------------------------------------------------*/
	0x09,                           /* bLength */
	0x21,                           /* bDescriptorType: DFU FUNCTIONAL */
	0x0B,                           /* bmAttributes: detach, upload, download */
	0xFF, 0x00,                     /* wDetachTimeOut */
	0x00, 0x08,                     /* wTransferSize */
	0x1a, 0x01,                     /* bcdDFUVersion: 1.1a */

  /** cdc **********************************************************************/
  /* Interface Association Descriptor for CDC */ // cchere
  USBD_IAD_DESC_SIZE,  // bLength: Interface Descriptor size，固定值
  USBD_IAD_DESCRIPTOR_TYPE,  // bDescriptorType: IAD，固定值
  USBD_CDC_FIRST_INTERFACE,  // bFirstInterface，第一个接口的起始序号，从0开始 // cchere
  USBD_CDC_INTERFACE_NUM,  // bInterfaceCount，本IAD下的接口数量
  0X02,  // bFunctionClass: CDC，表明该IAD是一个CDC类型的设备
  0X02,  // bFunctionSubClass：子类型，默认即可
  0X01,  // bFunctionProtocol：控制协议，默认即可
  0X00,  // iFunction

  /*Interface Descriptor */
  0x09,   /* bLength: Interface Descriptor size */
  USB_DESC_TYPE_INTERFACE,  /* bDescriptorType: Interface */
  /* Interface descriptor type */
  USBD_CDC_CMD_INTERFACE,   /* bInterfaceNumber: Number of Interface */ // cchere
  0x00,   /* bAlternateSetting: Alternate setting */
  USBD_CDC_CMD_INTERFACE_EP_NUM,   /* bNumEndpoints: One endpoints used */
  0x02,   /* bInterfaceClass: Communication Interface Class */
  0x02,   /* bInterfaceSubClass: Abstract Control Model */
  0x01,   /* bInterfaceProtocol: Common AT commands */
  0x00,   /* iInterface: */

  /*Header Functional Descriptor*/
  0x05,   /* bLength: Endpoint Descriptor size */
  0x24,   /* bDescriptorType: CS_INTERFACE */
  0x00,   /* bDescriptorSubtype: Header Func Desc */
  0x10,   /* bcdCDC: spec release number */
  0x01,

  /*Call Management Functional Descriptor*/
  0x05,   /* bFunctionLength */
  0x24,   /* bDescriptorType: CS_INTERFACE */
  0x01,   /* bDescriptorSubtype: Call Management Func Desc */
  0x00,   /* bmCapabilities: D0+D1 */
  0x01,   /* bDataInterface: 1 */

  /*ACM Functional Descriptor*/
  0x04,   /* bFunctionLength */
  0x24,   /* bDescriptorType: CS_INTERFACE */
  0x02,   /* bDescriptorSubtype: Abstract Control Management desc */
  0x02,   /* bmCapabilities */

  /*Union Functional Descriptor*/
  0x05,   /* bFunctionLength */
  0x24,   /* bDescriptorType: CS_INTERFACE */
  0x06,   /* bDescriptorSubtype: Union func desc */
  USBD_CDC_CMD_INTERFACE,   /* bMasterInterface: Communication class interface */ // cchere /* +++lakun:这里是用来指示CDC通信接口的编号的 */
  USBD_CDC_DATA_INTERFACE,   /* bSlaveInterface0: Data Class Interface */          // cchere /* +++lakun:这里是用来指示CDC数据接口的编号的 */

  /*Endpoint 2 Descriptor*/
  0x07,                           /* bLength: Endpoint Descriptor size */
  USB_DESC_TYPE_ENDPOINT,   /* bDescriptorType: Endpoint */
  CDC_CMD_EP,                     /* bEndpointAddress */
  0x03,                           /* bmAttributes: Interrupt */
  LOBYTE(CDC_CMD_PACKET_SIZE),     /* wMaxPacketSize: */
  HIBYTE(CDC_CMD_PACKET_SIZE),
  CDC_FS_BINTERVAL,                           /* bInterval: */
  /*---------------------------------------------------------------------------*/

  /*Data class interface descriptor*/
  0x09,   /* bLength: Endpoint Descriptor size */
  USB_DESC_TYPE_INTERFACE,  /* bDescriptorType: */
  USBD_CDC_DATA_INTERFACE,   /* bInterfaceNumber: Number of Interface */ // cchere
  0x00,   /* bAlternateSetting: Alternate setting */
  USBD_CDC_DATA_INTERFACE_EP_NUM,   /* bNumEndpoints: Two endpoints used */
  0x0A,   /* bInterfaceClass: CDC */
  0x00,   /* bInterfaceSubClass: */
  0x00,   /* bInterfaceProtocol: */
  0x00,   /* iInterface: */

  /*Endpoint OUT Descriptor*/
  0x07,   /* bLength: Endpoint Descriptor size */
  USB_DESC_TYPE_ENDPOINT,      /* bDescriptorType: Endpoint */
  CDC_OUT_EP,                        /* bEndpointAddress */
  0x02,                              /* bmAttributes: Bulk */
  LOBYTE(CDC_DATA_FS_MAX_PACKET_SIZE),  /* wMaxPacketSize: */
  HIBYTE(CDC_DATA_FS_MAX_PACKET_SIZE),
  0x00,                              /* bInterval: ignore for Bulk transfer */

  /*Endpoint IN Descriptor*/
  0x07,   /* bLength: Endpoint Descriptor size */
  USB_DESC_TYPE_ENDPOINT,      /* bDescriptorType: Endpoint */
  CDC_IN_EP,                         /* bEndpointAddress */
  0x02,                              /* bmAttributes: Bulk */
  LOBYTE(CDC_DATA_FS_MAX_PACKET_SIZE),  /* wMaxPacketSize: */
  HIBYTE(CDC_DATA_FS_MAX_PACKET_SIZE),
  0x00,                               /* bInterval: ignore for Bulk transfer */

} ;


/* USB Standard Device Descriptor */
uint8_t USBD_Composite_DeviceQualifierDesc[USB_LEN_DEV_QUALIFIER_DESC] =
{
  USB_LEN_DEV_QUALIFIER_DESC,
  USB_DESC_TYPE_DEVICE_QUALIFIER,
  0x00,
  0x02,
  0x00,
  0x00,
  0x00,
  0X40,
  0x01,
  0x00,
};


// static void USBD_Composite_GS_CAN_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
// {
// 	USBD_Composite_Switch_GS_CAN(pdev);
//
// 	/* Open EP OUT */
// 	USBD_LL_OpenEP(pdev, GSUSB_ENDPOINT_OUT, USBD_EP_TYPE_BULK, CAN_DATA_MAX_PACKET_SIZE);
// 	pdev->ep_out[GSUSB_ENDPOINT_OUT & 0xFU].is_used = 1U;
//
// 	/* Open EP IN */
// 	USBD_LL_OpenEP(pdev, GSUSB_ENDPOINT_IN,	 USBD_EP_TYPE_BULK, CAN_DATA_MAX_PACKET_SIZE);
// 	pdev->ep_in[GSUSB_ENDPOINT_IN & 0xFU].is_used = 1U;
//
// 	/* Prepare Out endpoint to receive next packet */
// 	USBD_GS_CAN_PrepareReceive(pdev);
// }
//
// static void USBD_Composite_CDC_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
// {
// 	USBD_CDC_HandleTypeDef   *hcdc = NULL;
//
// 	USBD_Composite_Switch_CDC(pdev);
//
// 	/* Open EP IN */
// 	USBD_LL_OpenEP(pdev, CDC_IN_EP, USBD_EP_TYPE_BULK, CDC_DATA_FS_IN_PACKET_SIZE);
// 	pdev->ep_in[CDC_IN_EP & 0xFU].is_used = 1U;
//
// 	/* Open EP OUT */
// 	USBD_LL_OpenEP(pdev, CDC_OUT_EP, USBD_EP_TYPE_BULK,CDC_DATA_FS_OUT_PACKET_SIZE);
// 	pdev->ep_out[CDC_OUT_EP & 0xFU].is_used = 1U;
//
// 	/* Open Command IN EP */
// 	USBD_LL_OpenEP(pdev, CDC_CMD_EP, USBD_EP_TYPE_INTR, CDC_CMD_PACKET_SIZE);
// 	pdev->ep_in[CDC_CMD_EP & 0xFU].is_used = 1U;
//
// 	hcdc = (USBD_CDC_HandleTypeDef *) pdev->pClassData;
//
// 	/* Init  physical Interface components */
// 	((USBD_CDC_ItfTypeDef *)pdev->pUserData)->Init();
//
// 	/* Init Xfer states */
// 	hcdc->TxState = 0U;
// 	hcdc->RxState = 0U;
//
// 	/* Prepare Out endpoint to receive next packet */
// 	USBD_LL_PrepareReceive(pdev, CDC_OUT_EP, hcdc->RxBuffer, CDC_DATA_FS_OUT_PACKET_SIZE);
// }

uint8_t USBD_Composite_Init (USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
    uint8_t res = 0;

    pdev->classId = USBD_GS_CAN_CLASSID;
    pdev->pUserData[pdev->classId] = NULL;
    USBD_GS_CAN_Init(pdev, &hGS_CAN);
    phGS_CAN = pdev->pClassData;
    res += USBD_GS_CAN.Init(pdev, cfgidx);

    pdev->classId = USBD_CDC_CLASSID;
    pdev->pUserData[pdev->classId] = &USBD_Interface_fops_HS;
    res += USBD_CDC.Init(pdev, cfgidx);
    phCDC = pdev->pClassData;
    
	return res;
}

uint8_t  USBD_Composite_DeInit (USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
    uint8_t res = 0;
    
    pdev->classId = USBD_GS_CAN_CLASSID;
    pdev->pClassData = phGS_CAN;
    res += USBD_GS_CAN.DeInit(pdev, cfgidx);
    
    pdev->classId = USBD_CDC_CLASSID;
    pdev->pClassData = phCDC;
    res += USBD_CDC.DeInit(pdev, cfgidx);

    return res;
}

uint8_t  USBD_Composite_Setup (USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
    switch (req->bmRequest & USB_REQ_RECIPIENT_MASK)
    {
        case USB_REQ_RECIPIENT_INTERFACE:
            switch(req->wIndex)
            {
                case USBD_GS_CAN_DATA_INTERFACE:
                case USBD_GS_CAN_DFU_INTERFACE:
                    USBD_Composite_Switch_GS_CAN(pdev);
                    return (USBD_GS_CAN.Setup(pdev, req));
                case USBD_CDC_CMD_INTERFACE:
                case USBD_CDC_DATA_INTERFACE:
                    USBD_Composite_Switch_CDC(pdev);
                    return (USBD_CDC.Setup(pdev, req));
                default:
                    break;
            }
            break;
        case USB_REQ_RECIPIENT_ENDPOINT:
            switch(req->wIndex)
            {
                case GSUSB_ENDPOINT_IN:
                case GSUSB_ENDPOINT_OUT:
                    USBD_Composite_Switch_GS_CAN(pdev);
                    return (USBD_GS_CAN.Setup(pdev, req));
                case CDC_CMD_EP:
                case CDC_IN_EP:
                case CDC_OUT_EP:
                    USBD_Composite_Switch_CDC(pdev);
                    return (USBD_CDC.Setup(pdev, req));
                default:
                    break;
            }
            break;
        default:
            break;
    }
    return USBD_OK;
}

uint8_t  USBD_Composite_DataIn (USBD_HandleTypeDef *pdev, uint8_t epnum)
{
	switch(epnum | 0X80)
	{
	case GSUSB_ENDPOINT_IN:
	    USBD_Composite_Switch_GS_CAN(pdev);
        return (USBD_GS_CAN.DataIn(pdev, epnum));
		break;
	case CDC_IN_EP:
	    USBD_Composite_Switch_CDC(pdev);
        return (USBD_CDC.DataIn(pdev, epnum));
		break;
	default:break;
	}

	return USBD_FAIL;
}

uint8_t  USBD_Composite_DataOut (USBD_HandleTypeDef *pdev, uint8_t epnum)
{
	switch(epnum)
	{
	case GSUSB_ENDPOINT_OUT:
		USBD_Composite_Switch_GS_CAN(pdev);
        return (USBD_GS_CAN.DataOut(pdev, epnum));
		break;
	case CDC_OUT_EP:
		USBD_Composite_Switch_CDC(pdev);
        return (USBD_CDC.DataOut(pdev, epnum));
		break;
	default:
        break;
	}
	return USBD_FAIL;
}

uint8_t  USBD_Composite_SOF (USBD_HandleTypeDef *pdev)
{
    switch (pdev->request.bmRequest & USB_REQ_RECIPIENT_MASK)
    {
        case USB_REQ_RECIPIENT_INTERFACE:
            switch(pdev->request.wIndex)
            {
                case USBD_GS_CAN_DATA_INTERFACE:
                case USBD_GS_CAN_DFU_INTERFACE:
                    USBD_Composite_Switch_GS_CAN(pdev);
                    return (USBD_GS_CAN.SOF(pdev));
                default:
                    break;
            }
            break;
        case USB_REQ_RECIPIENT_ENDPOINT:
            switch(pdev->request.wIndex)
            {
                case GSUSB_ENDPOINT_IN:
                case GSUSB_ENDPOINT_OUT:
                    USBD_Composite_Switch_GS_CAN(pdev);
                    return (USBD_GS_CAN.SOF(pdev));
                default:
                    break;
            }
            break;
        default:
            break;
    }
    return USBD_OK;
}

uint8_t  *USBD_Composite_GetHSCfgDesc (uint16_t *length)
{
  *length = sizeof (USBD_Composite_CfgDesc);
  return USBD_Composite_CfgDesc;
}

uint8_t  *USBD_Composite_GetFSCfgDesc (uint16_t *length)
{
  *length = sizeof (USBD_Composite_CfgDesc);
  return USBD_Composite_CfgDesc;
}

uint8_t  *USBD_Composite_GetOtherSpeedCfgDesc (uint16_t *length)
{
  *length = sizeof (USBD_Composite_CfgDesc);
  return USBD_Composite_CfgDesc;
}

uint8_t  *USBD_Composite_GetDeviceQualifierDescriptor (uint16_t *length)
{
  *length = sizeof (USBD_Composite_DeviceQualifierDesc);
  return USBD_Composite_DeviceQualifierDesc;
}

static uint8_t  USBD_Composite_EP0_RxReady (USBD_HandleTypeDef *pdev)
{
    switch (pdev->request.bmRequest & USB_REQ_RECIPIENT_MASK)
    {
        case USB_REQ_RECIPIENT_INTERFACE:
            switch(pdev->request.wIndex)
            {
                case USBD_GS_CAN_DATA_INTERFACE:
                case USBD_GS_CAN_DFU_INTERFACE:
                    USBD_Composite_Switch_GS_CAN(pdev);
                    return (USBD_GS_CAN.EP0_RxReady(pdev));
                case USBD_CDC_CMD_INTERFACE:
                case USBD_CDC_DATA_INTERFACE:
                    USBD_Composite_Switch_CDC(pdev);
                    return (USBD_CDC.EP0_RxReady(pdev));
                default:
                    break;
            }
            break;
        case USB_REQ_RECIPIENT_ENDPOINT:
            switch(pdev->request.wIndex)
            {
                case GSUSB_ENDPOINT_IN:
                case GSUSB_ENDPOINT_OUT:
                    USBD_Composite_Switch_GS_CAN(pdev);
                    return (USBD_GS_CAN.EP0_RxReady(pdev));
                case CDC_CMD_EP:
                case CDC_IN_EP:
                case CDC_OUT_EP:
                    USBD_Composite_Switch_CDC(pdev);
                    return (USBD_CDC.EP0_RxReady(pdev));
                default:
                    break;
            }
            break;
        default:
            break;
    }
    return USBD_OK;
}

void USBD_Composite_Switch_GS_CAN(USBD_HandleTypeDef *pdev)
{
    pdev->classId = USBD_GS_CAN_CLASSID;
    pdev->pClassData = phGS_CAN;
  // if (USBD_GS_CAN_Init(&hUSB, &hGS_CAN) != USBD_OK)
  //   {
  //       Error_Handler();
  //   }
}

void USBD_Composite_Switch_CDC(USBD_HandleTypeDef *pdev)
{
    pdev->classId = USBD_CDC_CLASSID;
    pdev->pClassData = phCDC;
	// static USBD_CDC_HandleTypeDef USBD_CDC_Handle;
	// USBD_CDC_RegisterInterface(pdev, &USBD_Interface_fops_HS);
	// pdev->pClassData = (void *)&USBD_CDC_Handle;
}

