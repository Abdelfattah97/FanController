#pragma once

#include <Arduino.h>
#include <WebSocketManager.h>

class StateBroadcaster
{
public:
    void setup();
    void loop();
    bool broadcast(const String &message);

private:
    WebSocketManager webSocketManager;
};

extern StateBroadcaster stateBroadcaster;
