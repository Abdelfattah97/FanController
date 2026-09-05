#include <StateBroadcaster.h>

void StateBroadcaster::setup()
{
    webSocketManager.setup();
    initialized = true;
}

void StateBroadcaster::loop()
{
    webSocketManager.loop();
}

bool StateBroadcaster::broadcast(const String &message)
{
    if (!initialized)
    {
        return false;
    }

    return webSocketManager.broadcast(message);
}

bool StateBroadcaster::broadcastStatus(const String &message)
{
    return broadcast(String("{\"type\":\"status\",\"data\":") + message + "}");
}

bool StateBroadcaster::broadcastDebug(const String &channel, const String &message)
{
    String escapedMessage = message;
    escapedMessage.replace("\\", "\\\\");
    escapedMessage.replace("\"", "\\\"");
    escapedMessage.replace("\n", "\\n");
    escapedMessage.replace("\r", "\\r");

    String payload = "{\"type\":\"debug\",\"channel\":\"";
    payload += channel;
    payload += "\",\"message\":\"";
    payload += escapedMessage;
    payload += "\"}";

    return broadcast(payload);
}

StateBroadcaster stateBroadcaster;
