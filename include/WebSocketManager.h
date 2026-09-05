#pragma once

#include <Arduino.h>
#include <WebSocketsServer.h>

class WebSocketManager
{
public:
    WebSocketManager();

    void setup();
    void loop();

    bool broadcast(const String &message);

private:
    WebSocketsServer webSocket;

    static void onWebSocketEvent(
        uint8_t clientId,
        WStype_t type,
        uint8_t *payload,
        size_t length);
};
