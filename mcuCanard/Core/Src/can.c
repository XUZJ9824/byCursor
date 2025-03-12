#include "can.h"

FDCAN_HandleTypeDef hfdcan2;

void FDCAN2_Init(void)
{
  hfdcan2.Instance = FDCAN2;
  hfdcan2.Init.FrameFormat = FDCAN_FRAME_CLASSIC;
  hfdcan2.Init.Mode = FDCAN_MODE_NORMAL;
  hfdcan2.Init.NominalPrescaler = 4;
  hfdcan2.Init.NominalSyncJumpWidth = 1;
  hfdcan2.Init.NominalTimeSeg1 = 13;
  hfdcan2.Init.NominalTimeSeg2 = 2;
  
  HAL_FDCAN_Init(&hfdcan2);
  
  // Configure and enable RX FIFO
  HAL_FDCAN_ConfigRxFifoOverwrite(&hfdcan2, FDCAN_RX_FIFO0, FDCAN_RX_FIFO_OVERWRITE);
  HAL_FDCAN_ConfigGlobalFilter(&hfdcan2, FDCAN_REJECT, FDCAN_REJECT, FDCAN_FILTER_REMOTE, FDCAN_FILTER_REMOTE);
  
  // Enable FIFO0 interrupt
  HAL_FDCAN_ActivateNotification(&hfdcan2, FDCAN_RX_FIFO0_NEW_MESSAGE, 0);
  
  HAL_FDCAN_Start(&hfdcan2);
} 