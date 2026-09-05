#include <StateBroadcaster.h>

void StateBroadcaster::setup()
{
    webSocketManager.setup();
}

void StateBroadcaster::loop()
{
    webSocketManager.loop();
}

bool StateBroadcaster::broadcast(const String &message)
{
    return webSocketManager.broadcast(message);
}

StateBroadcaster stateBroadcaster;
