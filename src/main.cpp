#include <Arduino.h>
#include <Config.h>
#include <WifiManager.h>
#include <LittleFS.h>
#include <WebServer.h>
#include <LittleFsServer.h>
#include <RemoteControl.h>
#include <Fan.h>
#include <Buzzer.h>
#include <Timer.h>

#ifdef DEBUG_FEATURE
#include <WebSocketsServer.h>
#endif

// =======================================================
// DEBUG-ONLY: RF debug console (WebSocket + web page)
//
// This entire block compiles to nothing when DEBUG_FEATURE
// is not defined. It exists purely so you can point a remote
// at the receiver and see what it decodes, live, in a browser,
// while learning a new remote.
// =======================================================

#ifdef DEBUG_FEATURE

// WebSocket is one-way (device -> browser), so the only thing
// we care about here is connect/disconnect for a Serial log line.
void onDebugWsEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length)
{

  switch (type)
  {

  case WStype_CONNECTED:
    Serial.printf("Debug WebSocket client #%u connected\n", num);
    break;

  case WStype_DISCONNECTED:
    Serial.printf("Debug WebSocket client #%u disconnected\n", num);
    break;

  default:
    break;
  }
}

// =========================
// debugRF()
//
// Sends the just-decoded IR frame to USB Serial and to any
// connected debug WebSocket browser clients. Uses a fixed-size
// stack buffer (no String, no growing log history in RAM) and
// only reports the fields the IRremote library actually exposes.
// =========================

void debugRF()
{

  char buf[160];

  snprintf(
      buf,
      sizeof(buf),
      "RF COMMAND RECEIVED\n"
      "Protocol: %s\n"
      "Address: 0x%02X\n"
      "Command: 0x%02X\n"
      "Raw Data: 0x%08lX\n"
      "Bits: %d",
      getProtocolString(IrReceiver.decodedIRData.protocol),
      IrReceiver.decodedIRData.address,
      IrReceiver.decodedIRData.command,
      (unsigned long)IrReceiver.decodedIRData.decodedRawData,
      IrReceiver.decodedIRData.numberOfBits);

  Serial.println(buf);

  debugWebSocket.broadcastTXT(buf);

  // Full pulse/timing breakdown - this call is already provided
  // by IRremote itself and streams straight to Serial, so it
  // doesn't need any extra buffer of ours.
  IrReceiver.printIRResultRawFormatted(&Serial);
}

#endif // DEBUG_FEATURE

// =========================
// SETUP
// =========================

void setup()
{

  Serial.begin(115200);

  delay(2000);

  // Remote
  RemoteControl::setup();

  // Buzzer
  Buzzer::setup();

  // Fan
  Fan::setup();

  // LittleFS
  LittleFsServer::setup();

  // WIFI
  WifiManager::setup();

  // WEB SERVER
  WebServer::setup();

  // =====================================================
  // DEBUG WEBSOCKET (debug build only)
  // =====================================================

#ifdef DEBUG_FEATURE
  debugWebSocket.begin();
  debugWebSocket.onEvent(onDebugWsEvent);
  Serial.println("Debug WebSocket server started on port 81");
  Serial.println("Open http://<device-ip>/debug to view RF debug console");
#endif
}

// =========================
// LOOP
// =========================

void loop()
{
  // WIFI
  WifiManager::update();

  // WEB SERVER
  WebServer::update();

  // IR
  RemoteControl::update();

  // TIMER
  Timer::update();
}
