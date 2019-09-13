#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <stdint.h>
#include <string.h>

#define BATCH_SIZE 100
#define MAX_BATCHES 10

void recordData(uint8_t data);
void recordData(char *data, size_t len);
void clearData();
uint8_t dataIndex();
uint8_t numBatches(uint8_t batch_size);
uint8_t *getBatch(uint8_t batch_no, uint8_t *batch_size);
char *getBatch(uint8_t index);
char *getCurrentBatch();

struct DataRecord {
    uint8_t id;
    uint8_t len;
    char data[BATCH_SIZE];
};
//extern std::queue<DataRecord> records;
size_t getRecordCount();
DataRecord getNextRecord();
DataRecord peekFirstRecord();
DataRecord peekLastRecord();

//extern struct DataRecord;
//extern std::queue<DataRecord> records;

#endif