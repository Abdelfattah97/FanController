#include <ESP8266WebServer.h>
#include <Config.h>
#include <LittleFS.h>
#include <ApiController.h>
#include <FanService.h>
// =======================================================
// WEB SERVER FUNCTIONS (unchanged)
// =======================================================

// =========================
// GET /api/status
// =========================

void ApiController::handleFanStatus()
{
    server.send(
        200,
        "application/json",
        fanService.getStatusJson());
}

// =========================
// POST /api/fan/off
// =========================

void ApiController::handleFanOff()
{

    fanService.setFanState(STATE_OFF);

    handleFanStatus();
}

// =========================
// POST /api/fan/speed1
// =========================

void ApiController::handleFanSpeed1()
{

    fanService.setFanState(STATE_SPEED1);

    handleFanStatus();
}

// =========================
// POST /api/fan/speed2
// =========================

void ApiController::handleFanSpeed2()
{

    fanService.setFanState(STATE_SPEED2);

    handleFanStatus();
}

// =========================
// POST /api/fan/speed3
// =========================

void ApiController::handleFanSpeed3()
{

    fanService.setFanState(STATE_SPEED3);

    handleFanStatus();
}

// =========================
// POST /api/beep/mute
// =========================

void ApiController::handleBeepMute()
{

    fanService.mute(true);
    handleFanStatus();
}

// =========================
// POST /api/beep/unmute
// =========================

void ApiController::handleBeepUnmute()
{

    fanService.mute(false);

    handleFanStatus();
}

// =========================
// POST /api/wifi/disconnect
// =========================

void ApiController::handleWifiDisconnect()
{

    fanService.setWifiEnabled(false);

    handleFanStatus();
}

void ApiController::handleSetFanOffTimer(int mins)
{
    fanService.setTimer(mins);

    handleFanStatus();
}

void ApiController::handleCancelFanOffTimer()
{
    fanService.cancelTimer();

    handleFanStatus();
}

#ifdef DEBUG_FEATURE

void ApiController::handleDebugIrPage()
{
    if (!LittleFS.exists("/debug-ir.html"))
    {
        server.send(404, "text/plain", "debug-ir.html not found");
        return;
    }

    File file = LittleFS.open("/debug-ir.html", "r");
    server.streamFile(file, "text/html");
    file.close();
}

void ApiController::handleDebugConsolePage()
{
    if (!LittleFS.exists("/debug-console.html"))
    {
        server.send(404, "text/plain", "debug-console.html not found");
        return;
    }

    File file = LittleFS.open("/debug-console.html", "r");
    server.streamFile(file, "text/html");
    file.close();
}

#endif
// =========================
// POST /api/wifi/connect
// =========================

void ApiController::handleWificonnect()
{

    fanService.setWifiEnabled(true);

    handleFanStatus();
}

// =========================
// Serve index.html
// =========================

void ApiController::handleRoot()
{

    if (!LittleFS.exists("/index.html"))
    {

        server.send(
            500,
            "text/plain",
            "index.html not found");

        return;
    }

    File file = LittleFS.open(
        "/index.html",
        "r");

    server.streamFile(
        file,
        "text/html");

    file.close();
}

// =========================
// Serve CSS
// =========================

void ApiController::handleCSS()
{

    if (!LittleFS.exists("/style.css"))
    {

        server.send(
            404,
            "text/plain",
            "style.css not found");

        return;
    }

    File file = LittleFS.open(
        "/style.css",
        "r");

    server.streamFile(
        file,
        "text/css");

    file.close();
}

// =========================
// Serve JavaScript
// =========================

void ApiController::handleJavaScript()
{

    if (!LittleFS.exists("/script.js"))
    {

        server.send(
            404,
            "text/plain",
            "script.js not found");

        return;
    }

    File file = LittleFS.open(
        "/script.js",
        "r");

    server.streamFile(
        file,
        "application/javascript");

    file.close();
}

// =========================
// 404
// =========================

void ApiController::handleNotFound()
{

    server.send(
        404,
        "text/plain",
        "404 - Not Found");
}
