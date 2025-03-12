#pragma once
#include "stm32g4xx_hal.h"
#include <stdarg.h>

void USART1_UART_Init(void);
int debug_printf(const char *format, ...); 