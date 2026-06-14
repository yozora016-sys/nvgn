/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usbd_customhid.h"
#include <string.h>
#include <stdlib.h>

extern USBD_HandleTypeDef hUsbDeviceFS;
uint8_t Send_Buffer[64] = {0};
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

/* USER CODE BEGIN PV */
uint8_t rx_byte;          
char rx_buffer[50];       
uint8_t rx_index = 0;     
uint8_t data_ready = 0;   

uint8_t nhiet_do = 0;
uint8_t do_am = 0;
uint8_t nut_nhan = 0;

uint8_t tu_dong_gui = 0;         
uint8_t trang_thai_nut_cu = 1;  
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USB_DEVICE_Init();
  MX_USART1_UART_Init();
  
  /* USER CODE BEGIN 2 */
  HAL_UART_Receive_IT(&huart1, &rx_byte, 1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		if (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_ORE) != RESET) {
          __HAL_UART_CLEAR_OREFLAG(&huart1);
          
          huart1.RxState = HAL_UART_STATE_READY; 
          
          HAL_UART_Receive_IT(&huart1, &rx_byte, 1); // Khởi động lại bộ lắng nghe an toàn
      }
      // Bóc tách dữ liệu
      if (data_ready == 1) {
          char *p_temp = strstr(rx_buffer, "TEMP:");
          char *p_hum = strstr(rx_buffer, "HUM:");

          if (p_temp != NULL && p_hum != NULL) {
              nhiet_do = (uint8_t)atoi(p_temp + 5); 
              do_am = (uint8_t)atoi(p_hum + 4);    
          }
          data_ready = 0; 
      }

      // Quét nút bấm đổi trạng thái
      uint8_t trang_thai_nut_hien_tai = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1);
      
      if (trang_thai_nut_hien_tai == GPIO_PIN_RESET && trang_thai_nut_cu == GPIO_PIN_SET) {
          tu_dong_gui = !tu_dong_gui; 

          if (tu_dong_gui == 1) {
              HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET); 
          } else {
              HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);   
          }
          
          HAL_Delay(150);
      }
      trang_thai_nut_cu = trang_thai_nut_hien_tai;

      // Gửi USB
      if (tu_dong_gui == 1) {
          Send_Buffer[0] = 0x01;       
          Send_Buffer[1] = nhiet_do;   
          Send_Buffer[2] = do_am;      
          Send_Buffer[3] = 120;        
          Send_Buffer[4] = 0;          
          Send_Buffer[5] = tu_dong_gui;

          USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, Send_Buffer, 64);
      }

      HAL_Delay(50); 
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
// Hàm ngắt nhận UART chạy ngầm
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART1) {
        if (rx_byte == '#') {
            rx_buffer[rx_index] = '\0'; 
            data_ready = 1;
            rx_index = 0;
        } else if (rx_byte == '$') {
            rx_index = 0; 
        } else {
            if (rx_index < 49) {
                rx_buffer[rx_index++] = rx_byte;
            }
        }
        HAL_UART_Receive_IT(&huart1, &rx_byte, 1);
    }
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}

#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
}
#endif /* USE_FULL_ASSERT */