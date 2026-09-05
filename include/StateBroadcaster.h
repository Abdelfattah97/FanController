#pragma once

#include <Arduino.h>
#include <WebSocketManager.h>

class StateBroadcaster
{
public:
    void setup();
    void loop();
    bool broadcast(const String &message);
    bool broadcastStatus(const String &message);
    bool broadcastDebug(const String &channel, const String &message);

private:
    WebSocketManager webSocketManager;
    bool initialized = false;
};

extern StateBroadcaster stateBroadcaster;
