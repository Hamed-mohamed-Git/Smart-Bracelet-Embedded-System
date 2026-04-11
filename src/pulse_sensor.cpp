#include <Arduino.h>
#include <Wire.h>
#undef I2C_BUFFER_LENGTH
#include "MAX30105.h"
#include "spo2_algorithm.h"
#include "pulse_sensor.h"

static MAX30105 particleSensor;

PulseSensor::PulseSensor(const uint8_t sdaPin,const uint8_t sclPin) {
    Wire.begin(sdaPin, sclPin);
    Serial.println("I2C started");

    if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
        Serial.println("Sensor not found");
        sensorReady = false;
        return;
    }

    particleSensor.setup(
        10,   // brightness
        4,    // sample avg
        2,    // red + IR
        100,  // sample rate
        411,  // pulse width
        4096  // ADC range
    );

    sensorReady = true;
    Serial.println("MAX30102 ready...");
}

bool PulseSensor::isReady() const {
    return sensorReady;
}

void PulseSensor::collectSample() {
    if (particleSensor.available()) {
        redBuffer[bufferIndex] = particleSensor.getRed();
        irBuffer[bufferIndex]  = particleSensor.getIR();
        particleSensor.nextSample();
        bufferIndex++;

        if (bufferIndex >= BUFFER_SIZE) {
            bufferIndex = KEEP_COUNT;
            for (int i = 0; i < KEEP_COUNT; i++) {
                redBuffer[i] = redBuffer[i + (BUFFER_SIZE - KEEP_COUNT)];
                irBuffer[i]  = irBuffer[i + (BUFFER_SIZE - KEEP_COUNT)];
            }
        }
    } else {
        particleSensor.check();
    }
}

bool PulseSensor::calculateIfReady(PulseData& data) {
    if (millis() - lastCalcTime < 1000 || bufferIndex < KEEP_COUNT) {
        return false;
    }

    lastCalcTime = millis();

    maxim_heart_rate_and_oxygen_saturation(
        irBuffer,
        BUFFER_SIZE,
        redBuffer,
        &data.spo2,
        &data.validSPO2,
        &data.heartRate,
        &data.validHeartRate
    );

    return true;
}
