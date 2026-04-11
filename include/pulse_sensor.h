#ifndef PULSE_SENSOR_H
#define PULSE_SENSOR_H

#include <stdint.h>

struct PulseData {
    int32_t heartRate;
    int32_t spo2;
    int8_t  validHeartRate;
    int8_t  validSPO2;
};

bool setupPulseSensor();
bool isSensorReady();
void collectSample();
bool calculateIfReady(PulseData& data);

#endif
