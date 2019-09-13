#include <map>
#include <queue>
#include "DataManager.h"
#include <console.h>
#include <Arduino.h>


uint8_t _buffer[255];
uint8_t _index = 0;

char batches[MAX_BATCHES][BATCH_SIZE] = {};
char packets[MAX_BATCHES][BATCH_SIZE] = {};
/* We track max_batch separately from current_batch so we can overfill our buffer and
   still return as much of the historical data as possible. */
uint8_t current_batch = 0;
uint8_t max_batch = 0;
uint8_t batch_index = 0;
uint8_t next_packet = 0;
uint8_t packet_index = 0; 

/*
struct DataRecord {
    uint8_t id;
    uint8_t len;
    char data[BATCH_SIZE];
};
*/
std::queue<DataRecord> records;

uint8_t current_record_data_index;
uint8_t record_number = 1;

size_t getRecordCount()
{
    return records.size();
}

DataRecord getNextRecord()
{
    DataRecord record = records.front();
    records.pop();
    return record;
}

DataRecord peekFirstRecord()
{
    return records.front();
}

DataRecord peekLastRecord()
{
    return records.back();
}

void clearData()
{
    _index = 0;
    memset(batches, 0, BATCH_SIZE*MAX_BATCHES*(sizeof(char)));
    current_batch = 0;
    max_batch = 0;
    batch_index = 0;
}

void clearBatch(uint8_t batch)
{
    memset(batches[batch], 0, BATCH_SIZE*(sizeof(char)));
}

uint8_t dataIndex()
{
    return _index;
}

void incrementBatch()
{
    current_batch++; max_batch++;
    if (current_batch >= MAX_BATCHES) current_batch = 0;
    if (max_batch >= MAX_BATCHES) max_batch = MAX_BATCHES - 1;
    println("INCREMENTED BATCH TO %d", current_batch);
    println("MAX BATCH is %d", max_batch);
    clearBatch(current_batch);
    batch_index = 0;
}

void recordData(uint8_t data)
{
    _buffer[_index++] = data;
}


void recordData(char *data, size_t len)
{
    println("memcopy length: %d", len);
    if (len == 0) return;
    if (batch_index + len > BATCH_SIZE) incrementBatch();
    //if (records.size() == 0 || current_record_data_index + len > BATCH_SIZE) {
    //    record_number++;
    //    if (record_number == 0) record_number++;
    //    records.push({ .id=record_number });
    //}
    records.push({});
    records.back().len = len;
    if (batch_index > 0) memset(&batches[current_batch][batch_index++], ',', 1);
    //if (current_record_data_index > 0) memset(&records.back().data[current_record_data_index++], ',', 1);
    println("RECORDING THIS DATA:");
    for (int i=0; i<len; i++) print("%c", data[i]);
    memcpy(&batches[current_batch][batch_index], data, len);
    //memcpy(&records.back().data[current_record_data_index++], data, len);
    memcpy(records.back().data, data, len);
    batch_index += len;
    //current_record_data_index += len;
    println("Recorded data. Current batch:");
    for (int i=0; i<batch_index; i++) print("%c", batches[current_batch][i]);
    println("");
    if (batch_index == BATCH_SIZE) incrementBatch();
    println("*** Done recording data to record set ***");
    println("*** Records SIZE: %d", records.size());
    print("Last record ID: %d", records.back().id); print("; DATA: ");
    Serial.println(records.back().data);
    println("Completed recordData");
    
}


uint8_t numBatches(uint8_t batch_size)
{
    return max_batch + 1;
    /*
    uint8_t num = _index / batch_size;
    if (_index % batch_size > 0) num++;
    return num;
    */
}

/*
uint8_t *getBatch(uint8_t batch_no, uint8_t *batch_size)
{
    uint8_t batch_index = batch_no * *batch_size;
    println("INDEX: %d; BATCH_INDEX: %d; BATCH_SIZE: %d; NUM_BATCHES: %d",
        _index, batch_index, *batch_size, numBatches(*batch_size));
    if ( (batch_index + *batch_size) > _index) *batch_size = (_index - batch_index);
    return &_buffer[batch_index];
}
*/

char *getBatch(uint8_t index)
{
    static char packet_buffer[BATCH_SIZE] = {};
    static uint8_t packet_buffer_index = 0;
    static uint8_t packet_id = 0;
    if ( !(index == packet_id && packet_buffer_index > 0)) {
        if (packet_buffer_index > 0) {
            memset(packet_buffer, 0, BATCH_SIZE*(sizeof(char)));
            packet_buffer_index = 0;
        }
        while (records.size() > 0 && packet_buffer_index + records.front().len < BATCH_SIZE) {
            if (packet_buffer_index > 0) memset(&packet_buffer[packet_buffer_index++], ',', 1);
            memcpy(&packet_buffer[packet_buffer_index], records.front().data, records.front().len);
            packet_buffer_index += records.front().len;
            records.pop();
        }
        packet_id = index;
    }
    return packet_buffer;
}

char *getCurrentBatch()
{
    return batches[current_batch];
}
