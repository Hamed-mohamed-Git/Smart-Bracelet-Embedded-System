#include <Arduino.h>
#include "ble_manager.h"
#include "pulse_sensor.h"

void setup() {
    Serial.begin(115200);
    delay(2000);

    Serial.println("Starting...");

    setupBLE();
    setupPulseSensor();

    Serial.println("System ready...");
}

void loop() {
    if (!isSensorReady()) {
        static unsigned long lastMsg = 0;
        if (millis() - lastMsg > 2000) {
            lastMsg = millis();
            Serial.println("Running without sensor...");
        }
        return;
    }

    collectSample();

    PulseData data;
    if (calculateIfReady(data)) {
        String payload = "{";
        payload += "\"hr\":" + String(data.heartRate) + ",";
        payload += "\"spo2\":" + String(data.spo2) + ",";
        payload += "\"validHr\":" + String(data.validHeartRate) + ",";
        payload += "\"validSpo2\":" + String(data.validSPO2);
        payload += "}";

        Serial.print("HR: ");
        Serial.print(data.heartRate);
        Serial.print(" | SPO2: ");
        Serial.print(data.spo2);
        Serial.print(" | BLE payload: ");
        Serial.println(payload);

        sendBLEPayload(payload.c_str());

        delay(3500);
    }
}
