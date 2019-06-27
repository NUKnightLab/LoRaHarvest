#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <stdint.h>

void recordData(uint8_t data);
void clearData();
uint8_t dataIndex();
uint8_t numBatches(uint8_t batch_size);
uint8_t *getBatch(uint8_t batch_no, uint8_t *batch_size);

#endif