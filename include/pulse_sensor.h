#ifndef PULSE_SENSOR_H
#define PULSE_SENSOR_H

#include <stdint.h>

struct PulseData {
    int32_t heartRate;
    int32_t spo2;
    int8_t  validHeartRate;
    int8_t  validSPO2;
};

class PulseSensor {
public:
    PulseSensor(uint8_t sdaPin, uint8_t sclPin);
    bool isReady() const;
    void collectSample();
    bool calculateIfReady(PulseData& data);

private:
    static const int BUFFER_SIZE = 100;
    static const int KEEP_COUNT  = 75;

    uint32_t irBuffer[BUFFER_SIZE]  = {};
    uint32_t redBuffer[BUFFER_SIZE] = {};
    int bufferIndex = 0;
    unsigned long lastCalcTime = 0;
    bool sensorReady = false;
};

#endif
