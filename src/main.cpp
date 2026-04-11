#include <Arduino.h>
#include "ble/ble_manager.h"
#include "pulse_sensor.h"
#include "pulse_payload.h"

#define BAUD_RATE 115200
#define DELAY 2000
#define DEVICE_NAME "Smart Bracelet Mawadah"

BLEManager* ble = nullptr;
PulseSensor* sensor = nullptr;

void setup() {
    Serial.begin(BAUD_RATE);
    delay(DELAY);

    Serial.println("Starting...");

    ble = new BLEManager(DEVICE_NAME);
    sensor = new PulseSensor(8, 9);

    Serial.println("System ready...");
}

void loop() {
    if (!sensor->isReady()) {
        static unsigned long lastMsg = 0;
        if (millis() - lastMsg > 2000) {
            lastMsg = millis();
            Serial.println("Running without sensor...");
        }
        return;
    }

    sensor->collectSample();

    PulseData data{};
    if (sensor->calculateIfReady(data)) {
        const PulsePayload json(data);
        const String payload = json.build();

        Serial.print("HR: ");
        Serial.print(data.heartRate);
        Serial.print(" | SPO2: ");
        Serial.print(data.spo2);
        Serial.print(" | BLE payload: ");
        Serial.println(payload);

        ble->sendPayload(payload.c_str());

        delay(3500);
    }
}
