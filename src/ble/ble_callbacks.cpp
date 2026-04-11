#include <Arduino.h>
#include "ble/ble_callbacks.h"

void BLECallbacks::onConnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo) {
    connected = true;
    Serial.println("Phone connected!");
    Serial.print("Peer BLE address: ");
    Serial.println(connInfo.getAddress().toString().c_str());
}

void BLECallbacks::onDisconnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo, const int reason) {
    connected = false;
    Serial.println("Phone disconnected!");
    Serial.print("Last peer BLE address: ");
    Serial.println(connInfo.getAddress().toString().c_str());
    Serial.print("Disconnect reason: ");
    Serial.println(reason);

    NimBLEDevice::startAdvertising();
    Serial.println("Advertising restarted...");
}

bool BLECallbacks::isConnected() const {
    return connected;
}
