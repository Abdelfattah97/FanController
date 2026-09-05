#include "WebSocketManager.h"
#include <Debug.h>

WebSocketManager::WebSocketManager()
    : webSocket(81)
{
}

void WebSocketManager::setup()
{
    webSocket.begin();
    webSocket.onEvent(onWebSocketEvent);

    debugPrintln("WebSocket server started.");
}

void WebSocketManager::loop()
{
    webSocket.loop();
}

bool WebSocketManager::broadcast(const String &message)
{
    String mutableMessage = message;
    return webSocket.broadcastTXT(mutableMessage);
}

void WebSocketManager::onWebSocketEvent(
    uint8_t clientId,
    WStype_t type,
    uint8_t *payload,
    size_t length)
{
    switch (type)
    {
    case WStype_CONNECTED:
        debugPrintf(
            "WebSocket client %u connected.\n",
            clientId);
        break;

    case WStype_DISCONNECTED:
        debugPrintf(
            "WebSocket client %u disconnected.\n",
            clientId);
        break;

    default:
        break;
    }
}