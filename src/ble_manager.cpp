#include <Arduino.h>
#include "ble_manager.h"

static NimBLECharacteristic* pCharacteristic = nullptr;
static bool deviceConnected = false;

class ServerCallbacks : public NimBLEServerCallbacks {
    void onConnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo) override {
        deviceConnected = true;
        Serial.println("Phone connected!");
        Serial.print("Peer BLE address: ");
        Serial.println(connInfo.getAddress().toString().c_str());
    }

    void onDisconnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo, int reason) override {
        deviceConnected = false;
        Serial.println("Phone disconnected!");
        Serial.print("Last peer BLE address: ");
        Serial.println(connInfo.getAddress().toString().c_str());
        Serial.print("Disconnect reason: ");
        Serial.println(reason);

        NimBLEDevice::startAdvertising();
        Serial.println("Advertising restarted...");
    }
};

void setupBLE() {
    NimBLEDevice::init("SmartBracelet-Hamed");

    Serial.print("ESP32 BLE MAC: ");
    Serial.println(NimBLEDevice::getAddress().toString().c_str());

    NimBLEServer* pServer = NimBLEDevice::createServer();
    pServer->setCallbacks(new ServerCallbacks());

    NimBLEService* pService = pServer->createService(SERVICE_UUID);

    pCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY
    );

    pCharacteristic->setValue("{\"status\":\"waiting\"}");

    NimBLEAdvertising* pAdvertising = NimBLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setName("SmartBracelet-Hamed");
    pAdvertising->enableScanResponse(true);
    pAdvertising->setAppearance(0x0000);
    pAdvertising->start();

    Serial.println("BLE advertising started...");
}

bool isBLEConnected() {
    return deviceConnected;
}

void sendBLEPayload(const char* payload) {
    if (deviceConnected && pCharacteristic != nullptr) {
        pCharacteristic->setValue(payload);
        pCharacteristic->notify();
    }
}
