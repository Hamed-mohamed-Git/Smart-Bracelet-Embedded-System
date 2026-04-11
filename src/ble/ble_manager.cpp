#include <Arduino.h>
#include "ble/ble_manager.h"

BLEManager::BLEManager(const char* deviceName) {
    NimBLEDevice::init(deviceName);

    Serial.print("ESP32 BLE MAC: ");
    Serial.println(NimBLEDevice::getAddress().toString().c_str());

    NimBLEServer* pServer = NimBLEDevice::createServer();

    callbacks = new BLECallbacks();
    pServer->setCallbacks(callbacks);

    NimBLEService* pService = pServer->createService(SERVICE_UUID);

    characteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID,
        READ | NOTIFY
    );

    characteristic->setValue(R"({"status":"waiting"})");

    NimBLEAdvertising* pAdvertising = NimBLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setName(deviceName);
    pAdvertising->enableScanResponse(true);
    pAdvertising->setAppearance(0x0000);
    pAdvertising->start();

    Serial.println("BLE advertising started...");
}

bool BLEManager::isConnected() const {
    return callbacks != nullptr && callbacks->isConnected();
}

void BLEManager::sendPayload(const char* payload) const {
    if (isConnected() && characteristic != nullptr) {
        characteristic->setValue(payload);
        characteristic->notify();
    }
}
