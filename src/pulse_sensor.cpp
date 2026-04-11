#include <Arduino.h>
#include <Wire.h>
#undef I2C_BUFFER_LENGTH
#include "MAX30105.h"
#include "spo2_algorithm.h"
#include "pulse_sensor.h"

static MAX30105 particleSensor;

static uint32_t irBuffer[100];
static uint32_t redBuffer[100];
static int bufferIndex = 0;
static unsigned long lastCalcTime = 0;
static bool sensorReady = false;

bool setupPulseSensor() {
    Wire.begin(8, 9);
    Serial.println("I2C started");

    if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
        Serial.println("Sensor not found");
        sensorReady = false;
        return false;
    }

    particleSensor.setup(
        60,   // brightness
        4,    // sample avg
        2,    // red + IR
        100,  // sample rate
        411,  // pulse width
        4096  // ADC range
    );

    sensorReady = true;
    Serial.println("MAX30102 ready...");
    return true;
}

bool isSensorReady() {
    return sensorReady;
}

void collectSample() {
    if (particleSensor.available()) {
        redBuffer[bufferIndex] = particleSensor.getRed();
        irBuffer[bufferIndex]  = particleSensor.getIR();
        particleSensor.nextSample();
        bufferIndex++;

        if (bufferIndex >= 100) {
            bufferIndex = 75;
            for (int i = 0; i < 75; i++) {
                redBuffer[i] = redBuffer[i + 25];
                irBuffer[i]  = irBuffer[i + 25];
            }
        }
    } else {
        particleSensor.check();
    }
}

bool calculateIfReady(PulseData& data) {
    if (millis() - lastCalcTime < 1000 || bufferIndex < 75) {
        return false;
    }

    lastCalcTime = millis();

    maxim_heart_rate_and_oxygen_saturation(
        irBuffer,
        100,
        redBuffer,
        &data.spo2,
        &data.validSPO2,
        &data.heartRate,
        &data.validHeartRate
    );

    return true;
}
