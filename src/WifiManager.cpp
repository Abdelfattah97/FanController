#include <Config.h>
#include <ESP8266mDNS.h>
#include <WifiManager.h>
#include <Debug.h>
#include <Buzzer.h>
#include <Debug.h>

bool stopWifi = false;
bool wifiConnected = false;

bool wifiConnecting = false;
unsigned long wifiConnectStart = 0;
unsigned long lastWiFiAttempt = 0;

void WifiManager::setup()
{
    lastWiFiAttempt = millis() - WIFI_RETRY_INTERVAL;
    handleWiFi();
    lastWiFiAttempt = millis();
}

void WifiManager::update()
{
    handleWiFi();
}

bool WifiManager::connectWiFi()
{

    if (stopWifi)
    {
        return false;
    }

    unsigned long now = millis();

    // =========================
    // Start new connection
    // =========================

    if (!wifiConnecting)
    {

        debugPrintln("Connecting to Wi-Fi...");

        WiFi.mode(WIFI_STA);
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

        wifiConnecting = true;
        wifiConnectStart = now;

        return false;
    }

    // =========================
    // Connection successful
    // =========================

    if (WiFi.status() == WL_CONNECTED)
    {

        wifiConnecting = false;
        wifiConnected = true;

        debugPrintln("Wi-Fi connected!");

        debugPrint("IP address: ");
        debugPrintln(WiFi.localIP().toString());

        return true;
    }

    // =========================
    // Connection timeout
    // =========================

    if (now - wifiConnectStart >= 10000UL)
    {

        wifiConnecting = false;

        debugPrintln("Wi-Fi connection failed.");

        WiFi.disconnect();
        lastWiFiAttempt = now;

        return false;
    }

    // =========================
    // Still connecting
    // =========================

    return false;
}

void WifiManager::handleWiFi()
{

    // =========================
    // Wi-Fi connected
    // =========================

    if (!wifiConnecting && WiFi.status() == WL_CONNECTED)
    {
        return;
    }

    // =========================
    // Connection currently in progress
    // =========================

    if (wifiConnecting)
    {

        connectWiFi();

        return;
    }

    // =========================
    // Retry interval
    // =========================
    unsigned long now = millis();
    if (now - lastWiFiAttempt < WIFI_RETRY_INTERVAL)
    {
        return;
    }

    connectWiFi();
}
void WifiManager::switchWifi(bool flag)
{
    stopWifi = flag;
    if (flag)
    {
        WiFi.disconnect();
        wifiConnected = false;
        Buzzer::beep(200, 1);
    }
    else
    {
        Buzzer::beep(200, 2);
    }
}