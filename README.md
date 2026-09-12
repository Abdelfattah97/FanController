# FanController

ESP8266 fan controller for a NodeMCU board with NEC infrared remote control, Wi-Fi web control, LittleFS-hosted web assets, buzzer feedback, and automatic fan-off timers.

## What's New In This Release

This release corresponds to the latest commit, `d969011` (`Added updates for visibility changes, introduced getBaseUrl() and getBaseWsUrl() to support external mDNS resolution, and added a fetch timeout`).

### Latest Updates (Commits d969011 – b102969)

- **API Response Unification**: All API endpoints now return the complete fan status object for consistency (previously returned individual fields).
- **CORS Support**: Enabled CORS headers for all API requests, allowing cross-origin access.
- **Fetch Timeout**: Added timeout handling for HTTP requests to improve reliability over unstable connections.
- **DOM Caching**: Optimized script.js with `cacheDomElements()` function for better performance.

### Earlier Enhancements

- Added a web interface for setting a fan shutoff timer with quick presets (15 min, 30 min, 1 hr, 2 hrs) and custom durations.
- Timer starts the fan at speed 1 when off and turns it off automatically at expiration.
- Added timer status and remaining-time display to the web interface.
- Added API validation for missing or invalid timer durations.
- Implemented `FanService` application layer shared by the HTTP API and IR remote.
- Added `StateBroadcaster` and `WebSocketManager` for realtime status updates.
- Replaced periodic browser status polling with a status WebSocket on port 81.

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
|   |-- Debug.h              Compile-time debug logging interface
|   |-- Fan.h                Fan state and output control interface
|   |-- LittleFsServer.h     LittleFS initialization interface
|   |-- RemoteControl.h      IR receiver and command handling interface
|   |-- Secrets.h             Local Wi-Fi credentials (ignored by Git)
|   |-- FanService.h          Application-level fan operations
|   |-- StateBroadcaster.h    Application state output interface
|   |-- Timer.h              Fan-off timer interface
|   |-- WebServer.h           HTTP server and mDNS interface
|   |-- WifiManager.h         Wi-Fi connection management interface
|-- src/                    C++ implementations
|   |-- ApiController.cpp     Web pages and REST API handlers
|   |-- Buzzer.cpp            Buzzer output and mute state
|   |-- Debug.cpp             Serial and WebSocket debug output
|   |-- Fan.cpp               Fan output pins and speed state
|   |-- FanService.cpp        Application service coordination
|   |-- LittleFsServer.cpp    LittleFS startup
|   |-- RemoteControl.cpp     NEC IR decoding and command mapping
|   |-- StateBroadcaster.cpp  State output coordination
|   |-- Timer.cpp             Timer scheduling and expiration
|   |-- WebServer.cpp         HTTP routes, mDNS, OTA, and request loop
|   |-- WebSocketManager.cpp  WebSocket transport implementation
|   |-- WifiManager.cpp       Wi-Fi connection and retry logic
|   |-- main.cpp              Arduino setup and loop orchestration
|-- data/                   LittleFS web files
|   |-- index.html            Fan control interface
|   |-- debug-ir.html         IR command debug page
|   |-- debug-console.html    Debug message console page
|   |-- script.js             Browser controls and API calls
|   |-- style.css             Web interface styles
|-- lib/                    Project-local libraries
|-- test/                   PlatformIO test directory
```

## Web API

All control endpoints are served by the ESP8266 web server. All endpoints return the complete fan status (except errors).

### API Endpoints

| Method | Endpoint                | Purpose                                    |
| ------ | ----------------------- | ------------------------------------------ |
| GET    | `/api/status`           | Get fan state, mute status, and timer      |
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

### Response Format

All control endpoints (POST) and the status endpoint (GET) return a complete status object:

```json
{
  "state": "SPEED1",
  "mute": false,
  "timer": 0
}
```

Where:

- `state`: Current fan state ("OFF", "SPEED1", "SPEED2", or "SPEED3")
- `mute`: Whether buzzer is muted (true/false)
- `timer`: Remaining timer duration in minutes (0 if inactive)

### Timer Requests

Set a timer with a POST request to `/api/fan/timer/set` including the `minutes` parameter:

```text
POST /api/fan/timer/set
Content-Type: application/x-www-form-urlencoded

minutes=30
```

Timer requests with no `minutes` parameter or a value less than or equal to zero return HTTP 400.

### CORS Support

All endpoints include CORS headers, enabling requests from web applications on different domains or ports.

## Realtime Status Updates

The browser connects to the status WebSocket using dynamic URL resolution:

```javascript
const baseUrl = getBaseUrl(); // Checks for native getFanBaseUrl(), falls back to location.origin
const wsUrl = getBaseWsUrl(); // Builds WebSocket URL from base URL
const ws = new WebSocket(wsUrl);
```

### WebSocket Message Format

The WebSocket sends a typed status envelope:

```json
{ "type": "status", "data": { "state": "SPEED1", "mute": false, "timer": 30 } }
```

Fan, mute, and timer changes from either the web API or IR remote go through `FanService`. The service broadcasts the resulting state through `StateBroadcaster` and `WebSocketManager`.

The same WebSocket also carries typed debug messages when `DEBUG_FEATURE` is enabled. Status messages use `type: "status"`; debug messages use `type: "debug"` with either the `ir` or `log` channel.

### Network Connectivity

#### Direct Web Access

Access the device at `http://fan.local/` on the same Wi-Fi network as the ESP8266 using standard mDNS name resolution. The web interface automatically uses `window.location.origin` as the default base URL.

#### Mobile App Integration (APK with WebView)

The web interface is designed to integrate with native mobile applications through a WebView. The native app can implement an mDNS resolver service to reliably discover and connect to the device, solving issues with dynamic IPs and mDNS name resolution failures in WebView clients.

**Implementation Pattern:**

The native layer (APK) must provide a `getFanBaseUrl()` function via the JavaScript bridge:

```javascript
// Called by native layer (e.g., during app initialization or network changes)
// The native mDNS resolver discovers and passes the device IP
window.getFanBaseUrl = function () {
  return resolvedIp ? `http://${resolvedIp}` : "";
};

// Called by native layer to invalidate cache when network changes
window.clearResolvedFanHost = function () {
  resolvedIp = null;
};
```

**Example Native Implementation (APK):**

```javascript
let resolvedIp = null;

// Called when native mDNS resolver finds the device
window.setResolvedFanHost = function (ip) {
  resolvedIp = ip;
  console.log("Native layer resolved fan host to:", ip);
};

// Called to clear cached IP
window.clearResolvedFanHost = function () {
  console.log("Native layer invalidated cached IP");
  resolvedIp = null;
};

// Bridge function that web script calls via getBaseUrl()
window.getFanBaseUrl = function () {
  const url = resolvedIp ? `http://${resolvedIp}` : "";
  console.log("HTTP base url:", url);
  return url;
};
```

**Benefits of This Architecture:**

- **Dynamic IP Handling**: Router DHCP changes are handled by the native mDNS resolver
- **Reliable Resolution**: Bypasses mDNS name resolution issues in WebView clients
- **Native Control**: The native layer manages IP discovery and caching
- **Backward Compatible**: Direct web server access via `http://fan.local/` is unaffected
- **Fallback Support**: Without native integration, falls back to `window.location.origin`

#### Fetch Timeout

HTTP requests include a timeout mechanism to handle unreliable connections. If a request exceeds the timeout duration, it will be automatically cancelled.

## Configuration

Before building, create or update `include/Secrets.h` with the local Wi-Fi credentials:

```cpp
#define WIFI_SSID "your-wifi-name"
#define WIFI_PASSWORD "your-wifi-password"
```

The `include/Secrets.h` file is excluded by `.gitignore` and should not be committed.

Update the hardware and remote settings in `include/Config.h`:

- `IR_RECEIVE_PIN`
- `SPEED1_PIN`, `SPEED2_PIN`, and `SPEED3_PIN` (Active-High)
- `BEEP_PIN` (Active-High)
- IR remote address and button command constants

## Requirements

### Hardware

- **Microcontroller:** NodeMCU 1.0 with ESP8266 ESP-12E module
- **Fan Control:** Compatible relay modules or PWM-capable GPIO pins for fan speed control (3 speeds)
- **Infrared Receiver:** NEC-compatible IR receiver module (e.g., VS1838B)
- **Infrared Remote:** NEC-compatible IR remote (most TV/appliance remotes work)
- **Buzzer:** Active buzzer (with built-in oscillator) for feedback
- **Power:** USB cable for programming; suitable power supply for fan control circuit
- **Network:** 2.4 GHz Wi-Fi network (802.11b/g/n)

### Software & Tools

- **PlatformIO Core** or VS Code with PlatformIO extension
- **ESP8266 Board Support** in PlatformIO (auto-installed)
- **Git** (for version control tracking)

## Build

### Prerequisites

- VS Code with PlatformIO extension installed (recommended), or PlatformIO Core
- NodeMCU 1.0 / ESP8266 ESP12 board

### Build Firmware

From the project directory, run:

```powershell
platformio run -e nodemcuv2
```

Or using the full path on Windows if `platformio` is not on `PATH`:

```powershell
& "$env:USERPROFILE\.platformio\penv\Scripts\platformio.exe" run -e nodemcuv2
```

Firmware output is generated under `.pio/build/nodemcuv2/firmware.bin`.

### Build Filesystem Image

The web interface is stored in LittleFS. Build the filesystem image after changing files in `data/`:

```powershell
platformio run -e nodemcuv2 -t buildfs
```

Output: `.pio/build/nodemcuv2/littlefs.bin`

## Upload

Upload the firmware and filesystem images over USB to the connected NodeMCU board.

### Upload Firmware

```powershell
platformio run -e nodemcuv2 -t upload
```

### Upload Filesystem

```powershell
platformio run -e nodemcuv2 -t uploadfs
```

### Upload Both

```powershell
platformio run -e nodemcuv2 -t uploadfs && platformio run -e nodemcuv2 -t upload
```

**Note:** The board must be connected, and the correct upload port must be available to PlatformIO (auto-detected in most cases).

## First Startup Checklist

1. **Configure Wi-Fi Credentials**  
   Create `include/Secrets.h`:

   ```cpp
   #define WIFI_SSID "your-wifi-name"
   #define WIFI_PASSWORD "your-wifi-password"
   ```

2. **Configure Hardware Pins**  
   Review and update pin mappings in `include/Config.h`:
   - IR receiver pin
   - Fan speed pins (PWM or relay pins)
   - Buzzer pin

3. **Connect NodeMCU**  
   Connect the board via USB data cable to your development machine.

4. **Build and Upload Firmware**

   ```powershell
   platformio run -e nodemcuv2 -t upload
   ```

5. **Build and Upload Filesystem**

   ```powershell
   platformio run -e nodemcuv2 -t buildfs && platformio run -e nodemcuv2 -t uploadfs
   ```

6. **Monitor Serial Output**  
   Open the serial monitor at 115200 baud to see startup logs and confirm Wi-Fi connection.

7. **Access the Web Interface**  
   Open a browser to `http://fan.local/` (or the device's IP address from serial output).

8. **Test Functionality**
   - Control fan speed using the web UI buttons
   - Set a timer and verify it counts down
   - Test the IR remote (if available)
   - Check buzzer muting

The web server is available only while Wi-Fi is connected. The IR remote remains available through the hardware receiver.

## Development & Debugging

### JavaScript API for Native Layers

If you're building a native mobile app (APK) with a WebView, implement the following JavaScript bridge functions for dynamic host resolution:

#### Native Bridge Functions

The native layer must implement:

```javascript
// Return the base HTTP URL for API calls
// Called by web script: getBaseUrl() checks if this exists
window.getFanBaseUrl(); // Should return "http://IP_ADDRESS" or empty string

// Optional: Clear cached IP when network changes
window.clearResolvedFanHost(); // Clears the native layer's cached resolution
```

The web script calls `getBaseUrl()` and `getBaseWsUrl()` which:

1. Check if `window.getFanBaseUrl()` is defined (native layer available)
2. Call it if available
3. Fall back to `window.location.origin` if not

#### Example Implementation

Implement this in your native layer (APK):

```javascript
let resolvedIp = null;

// Native mDNS resolver calls this when device is discovered
window.setResolvedFanHost = function (ip) {
  resolvedIp = ip;
  console.log("Native layer resolved fan host to:", ip);
};

// Clear cached IP when network changes or device disconnects
window.clearResolvedFanHost = function () {
  console.log("Native layer invalidated cached IP");
  resolvedIp = null;
};

// Bridge function that web script calls via getBaseUrl()
window.getFanBaseUrl = function () {
  const url = resolvedIp ? `http://${resolvedIp}` : "";
  console.log("HTTP base url:", url);
  return url;
};
```

### Debug Features

To enable debug pages and global debug logging, add the build flag to `platformio.ini`:

```ini
[env:nodemcuv2]
build_flags = -D DEBUG_FEATURE
```

Rebuild and upload firmware. Access debug pages at:

```text
http://fan.local/debug/ir       # IR command decoder
http://fan.local/debug/console  # Debug message log
```

Both pages use the shared WebSocket on port 81 for real-time updates. Without `DEBUG_FEATURE`, debug functions compile to no-ops and debug routes are not included.

### Architecture

The project uses a layered architecture:

- **Hardware Layer** (`Fan.h`, `Buzzer.h`, `RemoteControl.h`): Direct GPIO and sensor control
- **Service Layer** (`FanService.h`): Application-level operations, coordinates state changes
- **Transport Layer** (`WebServer.h`, `WebSocketManager.h`, `StateBroadcaster.h`): HTTP API and real-time updates
- **Utilities** (`Timer.h`, `Debug.h`, `WifiManager.h`): Supporting services

### Key Classes

- **FanService**: Centralizes fan operations, ensures consistency across UI and IR remote
- **StateBroadcaster**: Coordinates state changes and distributes updates
- **WebSocketManager**: Manages WebSocket connections and message dispatch
- **RemoteControl**: NEC IR protocol decoder and command mapper

## Troubleshooting

### Device not connecting to Wi-Fi

- Verify SSID and password in `include/Secrets.h`
- Check that the Wi-Fi network is 2.4 GHz (ESP8266 does not support 5 GHz)
- Monitor serial output at 115200 baud for connection logs

### Cannot access `http://fan.local/`

- Ensure the device is connected to the same Wi-Fi network
- Verify mDNS is enabled on your network (most home networks support it)
- Use the device's IP address (shown in serial logs) instead of the mDNS name
- Check that your firewall allows mDNS (port 5353 UDP)

### Web interface is slow or unresponsive

- Reduce Wi-Fi interference (change channel or move router)
- Check that the device has stable Wi-Fi signal strength
- Verify that no other devices are flooding the network
- The fetch timeout will cancel requests that take too long

### Timer does not work

- Confirm the timer API requests are reaching the device (check debug console)
- Ensure the fan is receiving power and responding to other commands
- Verify that the timer cancel button stops the countdown

### IR remote commands not detected

- Enable DEBUG_FEATURE and visit `/debug/ir` to verify commands are being received
- Test the remote at close range (30 cm) and gradually increase distance
- Check that the IR receiver module is properly connected to `IR_RECEIVE_PIN`
- Verify the remote is NEC-compatible (most TV remotes are)

### OTA firmware update fails

- Ensure sufficient free flash memory on the device
- Upload via USB directly if OTA fails
- Check that the binary file is compatible with the `nodemcuv2` environment

## License & Attribution

Created by Abdelfattah Mok.

This project uses:

- [PlatformIO](https://platformio.org/) for build and upload
- [Arduino Core for ESP8266](https://github.com/esp8266/Arduino)
- [IRremoteESP8266](https://github.com/crankyoldgit/IRremoteESP8266) for IR decoding
- [ESP8266 LittleFS](https://github.com/esp8266/arduino-esp8266fs-plugin) for filesystem storage
