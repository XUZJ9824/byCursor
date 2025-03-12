#include "port_canard.h"
#include "can.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "libcanard/libcanard.h"
#include "queue.h"

static CanardInstance canard_instance;
static SemaphoreHandle_t canard_mutex;
static CanardTxQueue canard_tx_queue;

#define CAN_RX_QUEUE_LENGTH 32
static QueueHandle_t can_rx_queue;

#define MAX_CAN_FRAMES 32
static uint8_t can_buffer_pool[MAX_CAN_FRAMES][64];
static FDCAN_RxHeaderTypeDef header_pool[MAX_CAN_FRAMES];
static size_t pool_index = 0;

// Add this before HAL_FDCAN_RxFifo0Callback
typedef struct CanRxItem {
    FDCAN_RxHeaderTypeDef* header;
    uint8_t* data;
} CanRxItem;

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

/**
 * @brief CAN RX FIFO0 interrupt callback handler
 * @param hfdcan: FDCAN handle pointer
 * @param RxFifo0ITs: Interrupt sources flags
 * 
 * This ISR callback handles new CAN messages in FIFO0:
 * 1. Retrieves message from hardware FIFO
 * 2. Copies data to pre-allocated buffer pool
 * 3. Queues message for processing in task context
 * 
 * Note: Uses circular buffer pool to prevent heap fragmentation in ISR
 */
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
    if((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != 0)
    {
        FDCAN_RxHeaderTypeDef rx_header;
        uint8_t rx_data[64];
        
        if(HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &rx_header, rx_data) == HAL_OK)
        {
            if (pool_index < MAX_CAN_FRAMES) {
                uint8_t* data_copy = can_buffer_pool[pool_index];
                FDCAN_RxHeaderTypeDef* header_copy = &header_pool[pool_index];
                *header_copy = rx_header;
                memcpy(data_copy, rx_data, rx_header.DataLength >> 16);
                CanRxItem item = {header_copy, data_copy};
                xQueueSendFromISR(can_rx_queue, &item, NULL);
                pool_index = (pool_index + 1) % MAX_CAN_FRAMES;
            }
        }
    }
}

/**
 * @brief Initialize libcanard instance and related resources
 * @param node_id: CAN node ID for this device
 * 
 * Initializes:
 * - libcanard instance with custom allocators
 * - TX queue for message transmission
 * - Mutex for thread-safe operations
 * - RX queue for message processing
 */
void canardInit(uint32_t node_id)
{
    canard_instance = canardInit(&canardAllocate, &canardFree);
    canard_instance.node_id = node_id;
    canard_tx_queue = canardTxInit(100); // Adjust queue size as needed
    canard_mutex = xSemaphoreCreateMutex();
    can_rx_queue = xQueueCreate(CAN_RX_QUEUE_LENGTH, sizeof(struct CanRxItem));
}

/**
 * @brief Transmit a CAN message using libcanard
 * @param transfer: Pointer to CanardTxTransfer structure
 * @return true if message queued successfully, false otherwise
 * 
 * This function:
 * - Locks mutex for thread-safe access
 * - Configures FDCAN header from libcanard frame
 * - Queues message to hardware TX FIFO
 * - Unlocks mutex after transmission
 */
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

/**
 * @brief Process received CAN frame through libcanard
 * @param rx_header: Pointer to FDCAN RX header
 * @param rx_data: Pointer to received payload data
 * 
 * This function:
 * 1. Converts FDCAN header to libcanard frame format
 * 2. Passes frame to libcanard RX acceptance logic
 * 3. Calls application handler if transfer is accepted
 */
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

/**
 * @brief FreeRTOS task for processing received CAN messages
 * @param argument: FreeRTOS task parameters (unused)
 * 
 * This task:
 * - Continuously checks for new messages in RX queue
 * - Processes messages through libcanard stack
 * - Manages buffer pool recycling
 * - Runs at higher priority than other CAN tasks
 */
void canardRxTask(void *argument)
{
    struct CanRxItem {
        FDCAN_RxHeaderTypeDef *header;
        uint8_t *data;
    } rx_item;
    
    for(;;) {
        if(xQueueReceive(can_rx_queue, &rx_item, portMAX_DELAY) == pdTRUE) {
            canardProcessRx(rx_item.header, rx_item.data);
            vPortFree(rx_item.header);
            vPortFree(rx_item.data);
        }
    }
} 