# FanController

ESP8266 fan controller for a NodeMCU board with NEC infrared remote control, Wi-Fi web control, LittleFS-hosted web assets, buzzer feedback, and automatic fan-off timers.

## What's New In This Release

This release corresponds to the latest commit, `008899e` (`Implemented Timer Functionality through web`).

- Added a web interface for setting a fan shutoff timer.
- Added quick timer presets for 15 minutes, 30 minutes, 1 hour, and 2 hours.
- Added custom timer durations in minutes.
- Added a timer cancel action.
- The timer starts the fan at speed 1 when the fan is off.
- The timer turns the fan off automatically when the duration expires.
- Added timer status and remaining-time display to the web interface.
- Added API validation for missing or invalid timer durations.

## Features

- Control fan states OFF, SPEED 1, SPEED 2, and SPEED 3.
- Control the fan using a NEC-compatible IR remote.
- Control the fan from a browser over Wi-Fi.
- Display the current fan state in the web UI.
- Mute or unmute buzzer feedback from the web UI or IR remote.
- Enable or disable Wi-Fi from the web UI or IR remote.
- Set, monitor, and cancel an automatic fan-off timer.
- Serve HTML, CSS, and JavaScript from the ESP8266 LittleFS filesystem.
- Provide OTA firmware update support at `/update`.
- Advertise the device through mDNS as `fan.local`.
- Optionally enable an IR debug console with `DEBUG_FEATURE`.

## Project Structure

```text
FanController/
|-- platformio.ini          PlatformIO build configuration
|-- FanController.ino        Arduino project entry point/reference
|-- README.md               Project documentation
|-- include/                Public headers and shared configuration
|   |-- ApiController.h      HTTP route handler declarations
|   |-- Buzzer.h             Buzzer control interface
|   |-- Config.h             Pins, remote commands, constants, and globals
|   |-- Fan.h                Fan state and output control interface
|   |-- LittleFsServer.h     LittleFS initialization interface
|   |-- RemoteControl.h      IR receiver and command handling interface
|   |-- Secrets.h             Local Wi-Fi credentials (ignored by Git)
|   |-- Timer.h              Fan-off timer interface
|   |-- WebServer.h           HTTP server and mDNS interface
|   |-- WifiManager.h         Wi-Fi connection management interface
|-- src/                    C++ implementations
|   |-- ApiController.cpp     Web pages and REST API handlers
|   |-- Buzzer.cpp            Buzzer output and mute state
|   |-- Fan.cpp               Fan output pins and speed state
|   |-- LittleFsServer.cpp    LittleFS startup
|   |-- RemoteControl.cpp     NEC IR decoding and command mapping
|   |-- Timer.cpp             Timer scheduling and expiration
|   |-- WebServer.cpp         HTTP routes, mDNS, OTA, and request loop
|   |-- WifiManager.cpp       Wi-Fi connection and retry logic
|   |-- main.cpp              Arduino setup and loop orchestration
|-- data/                   LittleFS web files
|   |-- index.html            Fan control interface
|   |-- script.js             Browser controls and API calls
|   |-- style.css             Web interface styles
|-- lib/                    Project-local libraries
|-- test/                   PlatformIO test directory
```

## Web API

All control endpoints are served by the ESP8266 web server.

| Method | Endpoint                | Purpose                                    |
| ------ | ----------------------- | ------------------------------------------ |
| GET    | `/api/status`           | Get fan and mute status                    |
| POST   | `/api/fan/off`          | Turn the fan off                           |
| POST   | `/api/fan/speed1`       | Set speed 1                                |
| POST   | `/api/fan/speed2`       | Set speed 2                                |
| POST   | `/api/fan/speed3`       | Set speed 3                                |
| POST   | `/api/beep/mute`        | Mute buzzer feedback                       |
| POST   | `/api/beep/unmute`      | Enable buzzer feedback                     |
| POST   | `/api/wifi/disconnect`  | Disable Wi-Fi client connection attempts   |
| POST   | `/api/wifi/connect`     | Re-enable Wi-Fi client connection attempts |
| POST   | `/api/fan/timer/set`    | Set timer using `minutes` parameter        |
| POST   | `/api/fan/timer/cancel` | Cancel the active timer                    |
| GET    | `/update`               | OTA firmware update page                   |

Example timer request:

```text
POST /api/fan/timer/set
Content-Type: application/x-www-form-urlencoded

minutes=30
```

Timer requests with no `minutes` parameter or a value less than or equal to zero return HTTP 400.

## Configuration

Before building, create or update `include/Secrets.h` with the local Wi-Fi credentials:

```cpp
#define WIFI_SSID "your-wifi-name"
#define WIFI_PASSWORD "your-wifi-password"
```

The `include/Secrets.h` file is excluded by `.gitignore` and should not be committed.

Update the hardware and remote settings in `include/Config.h`:

- `IR_RECEIVE_PIN`
- `SPEED1_PIN`, `SPEED2_PIN`, and `SPEED3_PIN`
- `BEEP_PIN`
- IR remote address and button command constants

## Requirements

- VS Code with the PlatformIO extension, or PlatformIO Core.
- NodeMCU 1.0 / ESP8266 NodeMCU board.
- USB data cable for firmware upload.
- An NEC-compatible IR remote and IR receiver.
- Fan control hardware connected to the configured output pins.
- A 2.4 GHz Wi-Fi network supported by the ESP8266.

## Build

From the project directory, run:

```powershell
platformio run -e nodemcuv2
```

If `platformio` is not on `PATH`, use the PlatformIO virtual environment executable on Windows:

```powershell
& "$env:USERPROFILE\.platformio\penv\Scripts\platformio.exe" run -e nodemcuv2
```

The firmware output is generated under `.pio/build/nodemcuv2/`.

## Build And Upload Filesystem

The web interface is stored in LittleFS. Build the filesystem image after changing files under `data/`:

```powershell
platformio run -e nodemcuv2 -t buildfs
```

Upload the filesystem over USB:

```powershell
platformio run -e nodemcuv2 -t uploadfs
```

Upload the firmware over USB:

```powershell
platformio run -e nodemcuv2 -t upload
```

Upload both firmware and filesystem as needed. The board must be connected and the correct upload port must be available to PlatformIO.

## First Startup

1. Configure Wi-Fi credentials in `include/Secrets.h` and hardware pins in `include/Config.h`.
2. Connect the NodeMCU board over USB.
3. Build and upload the firmware.
4. Build and upload the LittleFS filesystem.
5. Open the serial monitor at 115200 baud.
6. Wait for the device to connect to Wi-Fi and print its IP address.
7. Open `http://fan.local` or the printed IP address in a browser.

The web server is available only while Wi-Fi is connected. The IR remote remains available through the hardware receiver.

## Optional Debug Console

To enable the IR debug console, add this build flag to the `nodemcuv2` environment in `platformio.ini`:

```ini
build_flags = -D DEBUG_FEATURE
```

Rebuild and upload the firmware. Then open:

```text
http://fan.local/debug
```

The debug console uses a WebSocket on port 81 and reports decoded IR frames.
