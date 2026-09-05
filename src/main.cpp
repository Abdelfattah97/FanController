#include <Arduino.h>
#include <Config.h>
#include <WifiManager.h>
#include <LittleFS.h>
#include <WebServer.h>
#include <LittleFsServer.h>
#include <RemoteControl.h>
#include <Fan.h>
#include <Buzzer.h>
#include <FanService.h>
#include <StateBroadcaster.h>

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

  // STATUS BROADCASTER
  stateBroadcaster.setup();
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

  // STATUS BROADCASTER
  stateBroadcaster.loop();

  // IR
  RemoteControl::update();

  // APPLICATION SERVICE
  fanService.update();
}
