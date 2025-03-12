#include "usart.h"
#include <stdio.h>

UART_HandleTypeDef huart1;

void USART1_UART_Init(void)
{
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  HAL_UART_Init(&huart1);
}

// Custom printf implementation
int debug_printf(const char *format, ...)
{
  char buffer[256];
  va_list args;
  va_start(args, format);
  int length = vsnprintf(buffer, sizeof(buffer), format, args);
  va_end(args);
  
  HAL_UART_Transmit(&huart1, (uint8_t*)buffer, length, HAL_MAX_DELAY);
  return length;
} 