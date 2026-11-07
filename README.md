# Smart Bracelet: Embedded System

Firmware for a **smart health bracelet** built on the **ESP32-C3**. It reads **heart rate and blood-oxygen saturation (SpO₂)** from a **MAX30102** optical sensor and streams the readings to a phone over **Bluetooth Low Energy** as JSON notifications.

> The Android companion app lives here: **[Smart-Jewelry-App](https://github.com/Hamed-mohamed-Git/Smart-Jewelry-App)**

![ESP32-C3](https://img.shields.io/badge/MCU-ESP32--C3-E7352C?logo=espressif&logoColor=white)
![PlatformIO](https://img.shields.io/badge/Build-PlatformIO-F5822A?logo=platformio&logoColor=white)
![Arduino](https://img.shields.io/badge/Framework-Arduino-00979D?logo=arduino&logoColor=white)
![C++](https://img.shields.io/badge/Language-C%2B%2B-00599C?logo=cplusplus&logoColor=white)
![BLE](https://img.shields.io/badge/Bluetooth-NimBLE-0082FC?logo=bluetooth&logoColor=white)

---

## Table of contents

- [Overview](#overview)
- [Hardware](#hardware)
- [Wiring](#wiring)
- [Firmware architecture](#firmware-architecture)
- [BLE interface](#ble-interface)
- [Build and flash](#build-and-flash)
- [Serial output](#serial-output)
- [Project structure](#project-structure)
- [Roadmap](#roadmap)

---

## Overview

```
 MAX30102 ──I²C──▶ PulseSensor ──▶ Maxim HR/SpO₂ algorithm ──▶ PulsePayload (JSON) ──▶ BLEManager ──notify──▶ 📱 App
 (red + IR)        100-sample           every ≥ 1 s                {"hr":..,"spo2":..}     NimBLE GATT
                   rolling buffer
```

- Samples red and infrared light at **100 Hz** into a rolling 100-sample window. After each calculation, the newest 75 samples are kept.
- Runs Maxim's `maxim_heart_rate_and_oxygen_saturation` algorithm at most once per second.
- Builds a small JSON payload and sends it as a BLE **notification**, only while a phone is connected.
- **Restarts advertising automatically** when the phone disconnects.
- If the sensor is missing, the board keeps running and logs `Running without sensor...` instead of crashing.

## Hardware

| Part | Notes |
|---|---|
| **ESP32-C3** (`esp32-c3-devkitm-1`) | RISC-V MCU with built-in BLE 5. USB CDC serial is enabled on boot. |
| **MAX30102** pulse oximeter | Red and IR LEDs with a photodetector, connected over I²C |
| Battery and enclosure | For the wearable form factor |

## Wiring

| MAX30102 | ESP32-C3 |
|---|---|
| `VIN` | `3V3` |
| `GND` | `GND` |
| `SDA` | `GPIO 8` |
| `SCL` | `GPIO 9` |

The pins are passed to the `PulseSensor(sdaPin, sclPin)` constructor in `src/main.cpp`.

**Sensor configuration** (`src/pulse_sensor.cpp`):

| Parameter | Value |
|---|---|
| LED brightness | 10 |
| Sample average | 4 |
| LED mode | 2 (red + IR) |
| Sample rate | 100 Hz |
| Pulse width | 411 µs |
| ADC range | 4096 |
| I²C speed | Fast mode (400 kHz) |

## Firmware architecture

The code is split into small classes, each with one job:

| Module | Responsibility |
|---|---|
| `main.cpp` | Wires everything together: set up the sensor and BLE, then loop over sample → calculate → publish |
| `PulseSensor` | Configures the MAX30102, fills the rolling red/IR buffers, runs the HR/SpO₂ algorithm |
| `JsonBuilder` | Lightweight, fluent JSON object builder (`add(key, value)`) with no heap-heavy dependencies |
| `PulsePayload` | Extends `JsonBuilder`. Maps a `PulseData` reading to the wire format. |
| `BLEManager` | Creates the NimBLE server, service and characteristic. Starts advertising. `sendPayload()` notifies the phone. |
| `BLECallbacks` | Tracks the connection state and restarts advertising on disconnect |

## BLE interface

| Item | Value |
|---|---|
| Device name | `Smart Bracelet Mawadah` (`DEVICE_NAME` in `main.cpp`) |
| Service UUID | `12345678-1234-1234-1234-1234567890ab` |
| Characteristic UUID | `abcdefab-1234-1234-1234-abcdefabcdef` |
| Properties | `READ`, `NOTIFY` |
| Initial value | `{"status":"waiting"}` |

**Notification payload:**

```json
{ "hr": 72, "spo2": 98, "validHr": 1, "validSpo2": 1 }
```

| Field | Meaning |
|---|---|
| `hr` | Heart rate (bpm) |
| `spo2` | Blood-oxygen saturation (%) |
| `validHr` / `validSpo2` | `1` if the reading is valid, `0` if not (for example, the finger or wrist is not on the sensor) |

You can test without the app using **nRF Connect**: connect to the device, find the service above, and enable notifications on the characteristic.

## Build and flash

**Requirements:** [PlatformIO](https://platformio.org/) (VS Code extension or CLI)

```bash
git clone https://github.com/Hamed-mohamed-Git/Smart-Bracelet-Embedded-System.git
cd Smart-Bracelet-Embedded-System

pio run                 # build
pio run -t upload       # flash the ESP32-C3
pio device monitor      # serial monitor at 115200 baud
```

PlatformIO installs these libraries automatically:

- `sparkfun/SparkFun MAX3010x Pulse and Proximity Sensor Library`
- `h2zero/NimBLE-Arduino`
- `bblanchon/ArduinoJson`

## Serial output

```
Starting...
ESP32 BLE MAC: 10:00:3b:xx:xx:xx
BLE advertising started...
I2C started
MAX30102 ready...
System ready...
Phone connected!
HR: 72 | SPO2: 98 | BLE payload: {"hr":72,"spo2":98,"validHr":1,"validSpo2":1}
```

Note the **BLE MAC** printed at boot. The Android app can target the bracelet by this address.

## Project structure

```
├── platformio.ini            # board, framework, libraries
├── include/
│   ├── ble/
│   │   ├── ble_manager.h     # BLEManager class, service and characteristic UUIDs
│   │   └── ble_callbacks.h   # connect/disconnect handling
│   ├── pulse_sensor.h        # PulseSensor + PulseData
│   ├── pulse_payload.h       # PulseData → JSON
│   └── json_builder.h        # minimal JSON builder
└── src/
    ├── main.cpp
    ├── pulse_sensor.cpp
    └── ble/
        ├── ble_manager.cpp
        └── ble_callbacks.cpp
```

> `include/ble_manager.h` and `src/ble_manager.cpp` at the top level are an earlier, function-based version of the BLE layer. `main.cpp` uses the class-based version under `ble/`.

## Roadmap

- [ ] Wrist-detection flag in the payload (to drive the app's *Bracelet not on wrist* alert)
- [ ] Battery level reporting (standard Battery Service `0x180F`)
- [ ] Step counting with an accelerometer
- [ ] Low-power mode: deep sleep between readings and a longer advertising interval
- [ ] Remove the legacy top-level `ble_manager.*` files
- [ ] Unit tests for `JsonBuilder` and `PulsePayload` under `test/`
