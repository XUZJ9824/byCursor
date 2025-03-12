#pragma once
#include "stm32g4xx_hal.h"
#include "libcanard/libcanard.h"
#include "can.h"

void canardInit(uint32_t node_id);
bool canardTransmit(const CanardTxTransfer* transfer);
void canardProcessRx(FDCAN_RxHeaderTypeDef* rx_header, uint8_t* rx_data);
void canardRxTask(void *argument); 