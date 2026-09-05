#include <LittleFS.h>
#include <LittleFsServer.h>

void LittleFsServer::setup()
{
    // =====================================================
    // LITTLEFS
    // =====================================================

    Serial.println();
    Serial.println("Initializing LittleFS...");

    if (!LittleFS.begin())
    {

        Serial.println("LittleFS initialization failed!");
    }
    else
    {
        Serial.println("LittleFS initialized");
    }
}
