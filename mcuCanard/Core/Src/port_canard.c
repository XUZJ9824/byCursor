#include "port_canard.h"
#include "can.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "libcanard/libcanard.h"

static CanardInstance canard_instance;
static SemaphoreHandle_t canard_mutex;
static CanardTxQueue canard_tx_queue;

// Memory allocator for libcanard
static void* canardAllocate(CanardInstance* const ins, const size_t amount)
{
    (void)ins;
    return pvPortMalloc(amount);
}

static void canardFree(CanardInstance* const ins, void* const pointer)
{
    (void)ins;
    vPortFree(pointer);
}

void canardInit(uint32_t node_id)
{
    canard_instance = canardInit(&canardAllocate, &canardFree);
    canard_instance.node_id = node_id;
    canard_tx_queue = canardTxInit(100); // Adjust queue size as needed
    canard_mutex = xSemaphoreCreateMutex();
}

bool canardTransmit(const CanardTxTransfer* transfer)
{
    xSemaphoreTake(canard_mutex, portMAX_DELAY);
    
    FDCAN_TxHeaderTypeDef tx_header = {
        .Identifier = transfer->frame.extended_can_id,
        .IdType = FDCAN_EXTENDED_ID,
        .TxFrameType = FDCAN_DATA_FRAME,
        .DataLength = transfer->frame.payload_size,
        .ErrorStateIndicator = FDCAN_ESI_ACTIVE,
        .BitRateSwitch = FDCAN_BRS_OFF,
        .FDFormat = FDCAN_CLASSIC_CAN,
        .TxEventFifoControl = FDCAN_NO_TX_EVENTS,
        .MessageMarker = 0
    };
    
    const HAL_StatusTypeDef status = HAL_FDCAN_AddMessageToTxFifoQ(
        &hfdcan2, &tx_header, transfer->frame.payload);
    
    xSemaphoreGive(canard_mutex);
    return status == HAL_OK;
}

void canardProcessRx(FDCAN_RxHeaderTypeDef* rx_header, uint8_t* rx_data)
{
    CanardFrame frame = {
        .extended_can_id = rx_header->Identifier,
        .payload_size = rx_header->DataLength >> 16,
        .payload = rx_data
    };
    
    CanardTransfer transfer;
    if (canardRxAccept(&canard_instance, &frame, &transfer)) {
        // Handle received transfer here
    }
}

void canardRxTask(void *argument)
{
    FDCAN_RxHeaderTypeDef rx_header;
    uint8_t rx_data[64];
    
    for(;;) {
        if(HAL_FDCAN_GetRxMessage(&hfdcan2, FDCAN_RX_FIFO0, &rx_header, rx_data) == HAL_OK) {
            canardProcessRx(&rx_header, rx_data);
        }
        vTaskDelay(pdMS_TO_TICKS(1)); // Yield to other tasks
    }
} 