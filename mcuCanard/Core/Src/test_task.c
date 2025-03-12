#include "test_task.h"
#include "port_canard.h"
#include "usart.h"

void test_task(void *argument)
{
    canardInit(42); // Set node ID
    
    const CanardPortID data_type_id = 1234;
    uint8_t test_data[] = {0x01, 0x23, 0x45, 0x67};
    
    for(;;) {
        CanardTransfer transfer = {
            .transfer_type = CanardTransferTypeBroadcast,
            .data_type_id = data_type_id,
            .priority = CanardPriorityNominal,
            .payload_size = sizeof(test_data),
            .payload = test_data
        };
        
        if(canardTransmit(&transfer)) {
            debug_printf("Message sent successfully\r\n");
        }
        
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
} 