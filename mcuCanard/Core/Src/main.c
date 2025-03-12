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
    HAL_Init();
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