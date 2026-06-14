# 1 vài chỉnh sửa:


## chủ yếu chỉnh sửa trong file usb...custom_hid_if.c
```bash
/** Usb HID report descriptor. */
__ALIGN_BEGIN static uint8_t CUSTOM_HID_ReportDesc_FS[USBD_CUSTOM_HID_REPORT_DESC_SIZE] __ALIGN_END =
{
  0x06, 0x00, 0xFF,      // 3 bytes
  0x09, 0x01,            // 2 bytes
  0xa1, 0x01,            // 2 bytes
  0x85, 0x01,            // 2 bytes (Khai báo Report ID = 1 -> Chiếm 1 byte)
  0x09, 0x01,            // 2 bytes
  0x15, 0x00,            // 2 bytes
  0x26, 0xff, 0x00,      // 3 bytes
  0x75, 0x08,            // 2 bytes
  0x95, 0x3F,            // SỬA THÀNH 0x3F (63 Data Bytes + 1 ID = 64 Bytes vừa khít)
  0x81, 0x02,            // 2 bytes
  0x85, 0x01,            // 2 bytes
  0x09, 0x01,            // 2 bytes
  0x15, 0x00,            // 2 bytes
  0x26, 0xff, 0x00,      // 3 bytes
  0x75, 0x08,            // 2 bytes
  0x95, 0x3F,            // SỬA THÀNH 0x3F 
  0x91, 0x02,            // 2 bytes
  0xc0                   // 1 byte 
};
```

```bash
static int8_t CUSTOM_HID_OutEvent_FS(uint8_t event_idx, uint8_t state)
{
  /* USER CODE BEGIN 6 */

  extern UART_HandleTypeDef huart1; 

  USBD_CUSTOM_HID_HandleTypeDef *hhid = (USBD_CUSTOM_HID_HandleTypeDef*)hUsbDeviceFS.pClassData;
  uint8_t *rx_buffer = hhid->Report_buf;

  if (rx_buffer[1] == 0x01) {
      uint8_t cmd = '1';
      HAL_UART_Transmit(&huart1, &cmd, 1, 100);
  } 
  else if (rx_buffer[1] == 0x02) {
      uint8_t cmd = '0';
      HAL_UART_Transmit(&huart1, &cmd, 1, 100);
  }

  return (USBD_OK);
  /* USER CODE END 6 */
}
```


## Đổi giá trị của 2 biến này trong file usbd_conf.h:

```bash
#define USBD_CUSTOMHID_OUTREPORT_BUF_SIZE     64
/*---------- -----------*/
#define USBD_CUSTOM_HID_REPORT_DESC_SIZE     38
```


## Lưu ý các file .c khác của usb để biết chi tiết cấu hình


## các phần khác đọc trong code
