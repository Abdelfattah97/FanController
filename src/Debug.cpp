#include <Debug.h>

#ifdef DEBUG_FEATURE

#include <StateBroadcaster.h>
#include <stdarg.h>

namespace
{
    void broadcastLog(const String &message)
    {
        stateBroadcaster.broadcastDebug("log", message);
    }

    void broadcastIr(const String &message)
    {
        stateBroadcaster.broadcastDebug("ir", message);
    }
}

void debugPrint(const char *message)
{
    Serial.print(message);
    broadcastLog(message);
}

void debugPrint(const String &message)
{
    Serial.print(message);
    broadcastLog(message);
}

void debugPrint(int value)
{
    Serial.print(value);
    broadcastLog(String(value));
}

void debugPrintln()
{
    Serial.println();
    broadcastLog("");
}

void debugPrintln(const char *message)
{
    Serial.println(message);
    broadcastLog(message);
}

void debugPrintln(const String &message)
{
    Serial.println(message);
    broadcastLog(message);
}

void debugPrintln(int value)
{
    Serial.println(value);
    broadcastLog(String(value));
}

void debugPrintf(const char *format, ...)
{
    char buffer[192];
    va_list arguments;
    va_start(arguments, format);
    vsnprintf(buffer, sizeof(buffer), format, arguments);
    va_end(arguments);

    Serial.print(buffer);
    broadcastLog(buffer);
}

void debugRemoteCommand(uint16_t address, uint8_t command, bool accepted)
{
    char buffer[128];
    snprintf(
        buffer,
        sizeof(buffer),
        "IR command address=0x%04X command=0x%02X accepted=%s",
        address,
        command,
        accepted ? "true" : "false");

    Serial.println(buffer);
    broadcastIr(buffer);
}

#endif
