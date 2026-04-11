#ifndef PULSE_PAYLOAD_H
#define PULSE_PAYLOAD_H

#include "json_builder.h"
#include "pulse_sensor.h"

class PulsePayload : public JsonBuilder {
public:
    explicit PulsePayload(const PulseData& data) {
        add("hr", data.heartRate);
        add("spo2", data.spo2);
        add("validHr", (int32_t)data.validHeartRate);
        add("validSpo2", (int32_t)data.validSPO2);
    }
};

#endif
