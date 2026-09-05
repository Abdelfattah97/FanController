#include <ESP8266WebServer.h>
#include <Config.h>
#include <Fan.h>
#include <LittleFS.h>
#include <ApiController.h>
#include <Buzzer.h>
#include <WifiManager.h>
#include <Timer.h>
// =======================================================
// WEB SERVER FUNCTIONS (unchanged)
// =======================================================

// =========================
// Fan state → String
// =========================

const char *ApiController::getFanStateString()
{

    switch (fanState)
    {

    case STATE_SPEED1:
        return "SPEED1";

    case STATE_SPEED2:
        return "SPEED2";

    case STATE_SPEED3:
        return "SPEED3";

    case STATE_OFF:
    default:
        return "OFF";
    }
}

// =========================
// GET /api/status
// =========================

void ApiController::handleFanStatus()
{

    String json = "{";

    json += "\"state\":\"";
    json += getFanStateString();
    json += "\"";

    json += ",\"mute\":";
    json += isMuted ? "true" : "false";

    json += ",\"timer\":";
    json += Timer::isFanOffTimerActive()
                ? String(Timer::getRemainingFanOffMins())
                : "null";

    json += "}";

    server.send(
        200,
        "application/json",
        json);
}

// =========================
// POST /api/fan/off
// =========================

void ApiController::handleFanOff()
{

    Fan::switchState(STATE_OFF);

    handleFanStatus();
}

// =========================
// POST /api/fan/speed1
// =========================

void ApiController::handleFanSpeed1()
{

    Fan::switchState(STATE_SPEED1);

    handleFanStatus();
}

// =========================
// POST /api/fan/speed2
// =========================

void ApiController::handleFanSpeed2()
{

    Fan::switchState(STATE_SPEED2);

    handleFanStatus();
}

// =========================
// POST /api/fan/speed3
// =========================

void ApiController::handleFanSpeed3()
{

    Fan::switchState(STATE_SPEED3);

    handleFanStatus();
}

// =========================
// POST /api/beep/mute
// =========================

void ApiController::handleBeepMute()
{

    Buzzer::mute(true);

    server.send(
        200,
        "application/json",
        "{\"mute\":true}");
}

// =========================
// POST /api/beep/unmute
// =========================

void ApiController::handleBeepUnmute()
{

    Buzzer::mute(false);

    server.send(
        200,
        "application/json",
        "{\"mute\":false}");
}

// =========================
// POST /api/wifi/disconnect
// =========================

void ApiController::handleWifiDisconnect()
{

    WifiManager::switchWifi(true);

    server.send(
        200,
        "application/json",
        "{\"wifi\":false}");
}

void ApiController::handleSetFanOffTimer(int mins)
{
    Timer::setFanOffTimer(mins);
    server.send(
        200,
        "application/json",
        getFanTimerJson());
}

void ApiController::handleCancelFanOffTimer()
{
    Timer::cancelFanOffTimer();
    server.send(
        200,
        "application/json",
        getFanTimerJson());
}

#ifdef DEBUG_FEATURE

void ApiController::handleDebugPage()
{

    // Small, self-contained debug page. Embedded in PROGMEM rather
    // than LittleFS because the production index.html/script.js/
    // style.css are not touched by this feature at all.
    const char DEBUG_HTML[] = R"HTMLPAGE(
<!DOCTYPE html>
<html>
<head>
<meta charset="utf-8">
<title>RF Debug Console</title>
<style>
  body { font-family: monospace; background:#111; color:#0f0; margin:0; padding:1em; }
  h2 { color:#fff; margin-top:0; }
  #log { white-space: pre-wrap; border:1px solid #333; padding:0.5em; height:70vh; overflow-y:auto; background:#000; }
  button { background:#222; color:#0f0; border:1px solid #0f0; padding:0.4em 1em; cursor:pointer; margin-bottom:0.5em; }
  #status { color:#888; font-size:0.85em; margin-bottom:0.5em; }
</style>
</head>
<body>
<h2>RF Debug Console</h2>
<div id="status">connecting...</div>
<button onclick="clearLog()">Clear</button>
<div id="log"></div>
<script>
  var log = document.getElementById('log');
  var status = document.getElementById('status');
  var ws = new WebSocket('ws://' + location.hostname + ':81/');

  ws.onopen = function() { status.textContent = 'connected'; };
  ws.onclose = function() { status.textContent = 'disconnected'; };
  ws.onerror = function() { status.textContent = 'error'; };

  ws.onmessage = function(evt) {
    log.textContent += evt.data + "\n--------------------------------\n";
    log.scrollTop = log.scrollHeight;
  };

  function clearLog() {
    log.textContent = '';
  }
</script>
</body>
</html>
)HTMLPAGE";

    server.send_P(200, "text/html", DEBUG_HTML);
}
#endif
// =========================
// POST /api/wifi/connect
// =========================

void ApiController::handleWificonnect()
{

    WifiManager::switchWifi(false);

    server.send(
        200,
        "application/json",
        "{\"wifi\":true}");
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

String ApiController::getFanTimerJson()
{
    String json = "{\"timer\":";

    json += Timer::isFanOffTimerActive()
                ? String(Timer::getRemainingFanOffMins())
                : "null";

    json += "}";

    return json;
}