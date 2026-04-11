#ifndef BLE_CALLBACKS_H
#define BLE_CALLBACKS_H

#include <NimBLEDevice.h>

class BLECallbacks : public NimBLEServerCallbacks {
public:
    void onConnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo) override;
    void onDisconnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo, int reason) override;
    bool isConnected() const;

private:
    bool connected = false;
};

#endif
