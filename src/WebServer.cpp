#include <ESP8266WebServer.h>
#include <Config.h>
#include <ApiController.h>
#include <WebServer.h>
#include <ESP8266mDNS.h>
#include <Debug.h>

ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdater;

void WebServer::setup()
{
    setupWebServer();
    setupMDNS();
}
#include <Debug.h>

void WebServer::update()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        MDNS.update();
        server.handleClient();

        // TODO: Move to Debug
    }
}
// Setup WEB SERVER
void WebServer::setupMDNS()
{
    if (MDNS.begin("fan"))
    {

        debugPrintln("mDNS started!");
        debugPrintln("Open: http://fan.local");

        MDNS.addService("http", "tcp", 80);
    }
    else
    {

        debugPrintln("mDNS failed!");
    }
}
// SETUP WEB SERVER
void WebServer::setupWebServer()
{

    // =========================
    // Web page
    // =========================

    server.on(
        "/",
        HTTP_GET,
        ApiController::handleRoot);

    // =========================
    // Static files
    // =========================

    server.on(
        "/style.css",
        HTTP_GET,
        ApiController::handleCSS);

    server.on(
        "/script.js",
        HTTP_GET,
        ApiController::handleJavaScript);

    // =========================
    // API
    // =========================

    server.on(
        "/api/status",
        HTTP_GET,
        ApiController::handleFanStatus);

    server.on(
        "/api/fan/off",
        HTTP_POST,
        ApiController::handleFanOff);

    server.on(
        "/api/fan/speed1",
        HTTP_POST,
        ApiController::handleFanSpeed1);

    server.on(
        "/api/fan/speed2",
        HTTP_POST,
        ApiController::handleFanSpeed2);

    server.on(
        "/api/fan/speed3",
        HTTP_POST,
        ApiController::handleFanSpeed3);

    // =========================
    // Beep control
    // =========================

    server.on(
        "/api/beep/mute",
        HTTP_POST,
        ApiController::handleBeepMute);

    server.on(
        "/api/beep/unmute",
        HTTP_POST,
        ApiController::handleBeepUnmute);

    // =========================
    // Wifi control
    // =========================

    server.on(
        "/api/wifi/disconnect",
        HTTP_POST,
        ApiController::handleWifiDisconnect);

    server.on(
        "/api/wifi/connect",
        HTTP_POST,
        ApiController::handleWificonnect);

    // Timer

    server.on(
        "/api/fan/timer/set",
        HTTP_POST, []()
        { 
            if (!server.hasArg("minutes")) {
            server.send(400, "application/json",
                        "{\"error\":\"Missing minutes parameter\"}");
            return;
        }

        int mins = server.arg("minutes").toInt();

        if (mins <= 0) {
            server.send(400, "application/json",
                        "{\"error\":\"Invalid minutes\"}");
            return;
        }
            ApiController::handleSetFanOffTimer(mins); });

    server.on(
        "/api/fan/timer/cancel",
        HTTP_POST,
        ApiController::handleCancelFanOffTimer);

    // =========================
    // Debug console (debug build only)
    // =========================

#ifdef DEBUG_FEATURE
    server.on(
        "/debug/ir",
        HTTP_GET,
        ApiController::handleDebugIrPage);

    server.on(
        "/debug/console",
        HTTP_GET,
        ApiController::handleDebugConsolePage);
#endif

    // =========================
    // Unknown URLs
    // =========================

    server.onNotFound(
        ApiController::handleNotFound);

    // =========================
    // OTA update
    // =========================

    httpUpdater.setup(&server, "/update");

    server.begin();

    debugPrintln("Web server started");
}