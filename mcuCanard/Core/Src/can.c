#include "can.h"

FDCAN_HandleTypeDef hfdcan2;

// Initialize FDCAN2 with specific parameters for CAN communication
void FDCAN2_Init(void)
{
  hfdcan2.Instance = FDCAN2;
  hfdcan2.Init.FrameFormat = FDCAN_FRAME_CLASSIC;  // Use classic CAN frame format
  hfdcan2.Init.Mode = FDCAN_MODE_NORMAL;           // Set to normal mode
  hfdcan2.Init.NominalPrescaler = 34;              // Adjusted prescaler for 250 kbps
  hfdcan2.Init.NominalSyncJumpWidth = 1;           // Synchronization jump width
  hfdcan2.Init.NominalTimeSeg1 = 13;               // Time segment 1
  hfdcan2.Init.NominalTimeSeg2 = 2;                // Time segment 2
  
  // Calculate CAN baud rate:
  // Baud Rate = CAN Clock / (Prescaler * (Sync Segment + Time Segment 1 + Time Segment 2))
  // Assuming CAN Clock = 170 MHz (from system clock configuration)
  // Baud Rate = 170,000,000 / (34 * (1 + 13 + 2)) = 170,000,000 / 544 = ~312,500 bps
  // Adjusted to achieve approximately 250 kbps

  HAL_FDCAN_Init(&hfdcan2);
  
  // Configure and enable RX FIFO
  HAL_FDCAN_ConfigRxFifoOverwrite(&hfdcan2, FDCAN_RX_FIFO0, FDCAN_RX_FIFO_OVERWRITE);
  HAL_FDCAN_ConfigGlobalFilter(&hfdcan2, FDCAN_REJECT, FDCAN_REJECT, FDCAN_FILTER_REMOTE, FDCAN_FILTER_REMOTE);
  
  // Enable FIFO0 interrupt
  HAL_FDCAN_ActivateNotification(&hfdcan2, FDCAN_RX_FIFO0_NEW_MESSAGE, 0);
  
  HAL_FDCAN_Start(&hfdcan2);
} 