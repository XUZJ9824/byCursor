#include "main.h"
#include "usart.h"
#include "can.h"
#include "test_task.h"
#include "FreeRTOS.h"
#include "task.h"
#include "port_canard.h"

void SystemClock_Config(void);

int main(void)
{
    // Add error handling for HAL initialization
    if (HAL_Init() != HAL_OK) {
        // Initialization Error
        Error_Handler();
    }

    SystemClock_Config();
    
    USART1_UART_Init();
    FDCAN2_Init();
    
    // Enable FDCAN2 interrupts
    HAL_NVIC_SetPriority(FDCAN2_IT0_IRQn, 5, 0);  // Lower priority than FreeRTOS
    HAL_NVIC_EnableIRQ(FDCAN2_IT0_IRQn);

    xTaskCreate(test_task, "CAN_Test", 256, NULL, 2, NULL);
    xTaskCreate(canardRxTask, "CAN_Rx", 256, NULL, 3, NULL);  // Higher priority
    vTaskStartScheduler();

    while(1) {}
}

// System clock configuration and other HAL callbacks... 

// Ensure SystemClock_Config is properly implemented
void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    // Configure the main internal regulator output voltage
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 1;  // Division factor for PLL input clock
    RCC_OscInitStruct.PLL.PLLN = 85; // Multiplication factor for VCO
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2; // Division factor for main system clock
    RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2; // Division factor for USB, SDMMC, RNG
    RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2; // Division factor for system clock
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1; // AHB clock divider
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;  // APB1 clock divider
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;  // APB2 clock divider

    // Final system clock frequency calculation:
    // HSE Frequency = 8 MHz
    // PLL output frequency = (HSE / PLLM) * PLLN / PLLP = (8 MHz / 1) * 85 / 2 = 340 MHz
    // System clock frequency = PLL output frequency / PLLR = 340 MHz / 2 = 170 MHz

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK) {
        Error_Handler();
    }
} 