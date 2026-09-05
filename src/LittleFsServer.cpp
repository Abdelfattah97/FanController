#include <LittleFS.h>
#include <LittleFsServer.h>
#include <Debug.h>

void LittleFsServer::setup()
{
    // =====================================================
    // LITTLEFS
    // =====================================================

    debugPrintln();
    debugPrintln("Initializing LittleFS...");

    if (!LittleFS.begin())
    {

        debugPrintln("LittleFS initialization failed!");
    }
    else
    {
        debugPrintln("LittleFS initialized");
    }
}
