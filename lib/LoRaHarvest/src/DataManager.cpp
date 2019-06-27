#include "DataManager.h"

uint8_t _buffer[255];
uint8_t _index = 0;

void clearData()
{
    _index = 0;
}

uint8_t dataIndex()
{
    return _index;
}

void recordData(uint8_t data)
{
    _buffer[_index++] = data;
}

uint8_t numBatches(uint8_t batch_size)
{
    return _index / batch_size + _index % batch_size;
}

uint8_t *getBatch(uint8_t batch_no, uint8_t *batch_size)
{
    uint8_t batch_index = batch_no * *batch_size;
    if ( (batch_index + *batch_size) > _index) *batch_size = (_index - batch_index);
    return &_buffer[batch_index];
}