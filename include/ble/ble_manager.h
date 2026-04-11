#ifndef BLE_MANAGER_H
#define BLE_MANAGER_H

#include <NimBLEDevice.h>
#include "ble/ble_callbacks.h"

#define SERVICE_UUID        "12345678-1234-1234-1234-1234567890ab"
#define CHARACTERISTIC_UUID "abcdefab-1234-1234-1234-abcdefabcdef"

class BLEManager {
public:
    explicit BLEManager(const char* deviceName);
    bool isConnected() const;
    void sendPayload(const char* payload) const;

private:
    BLECallbacks* callbacks = nullptr;
    NimBLECharacteristic* characteristic = nullptr;
};

#endif
