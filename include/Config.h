
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <Secrets.h>
#define IR_RECEIVE_PIN D1

// =========================
// IR receiver
// =========================
// =========================
// Remote identification
// =========================

#define REMOTE_ADDRESS 0x00

// =========================
// Remote commands
// =========================

#define BUTTON_1 0x45
#define BUTTON_2 0x46
#define BUTTON_3 0x47
#define BUTTON_OFF 0x19
#define BUTTON_WIFI_SWITCH 0x0D
#define BUTTON_MUTE_SWITCH 0x16

// =========================
// Fan states
// =========================

#define STATE_OFF 0
#define STATE_SPEED1 1
#define STATE_SPEED2 2
#define STATE_SPEED3 3

// =========================
// Output pins
// =========================

#define SPEED1_PIN D5
#define SPEED2_PIN D6
#define SPEED3_PIN D7

#define BEEP_PIN D8

// =========================
// Buzzer Settings
// =========================

extern bool isMuted;

// =========================
// State → Pin map
// =========================

const int statePin[] = {
    0,
    SPEED1_PIN,
    SPEED2_PIN,
    SPEED3_PIN};

// =========================
// Current fan state
// =========================

extern int fanState;

extern bool stopWifi;
extern bool wifiConnected;

extern bool wifiConnecting;
extern unsigned long wifiConnectStart;

// =========================
// Wi-Fi retry settings
// =========================

#define WIFI_RETRY_INTERVAL 15000UL

extern unsigned long lastWiFiAttempt;

// Web server
extern ESP8266WebServer server;

// =========================
// Debug WebSocket server (RF debug console)
// Only exists in the debug build. Runs on its own port (81)
// so it cannot collide with the port-80 HTTP server or OTA.
// =========================

#ifdef DEBUG_FEATURE
WebSocketsServer debugWebSocket(81);
#endif