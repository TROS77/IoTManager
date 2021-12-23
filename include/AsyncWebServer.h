#pragma once
#include "Global.h"
#ifdef ASYNC_WEB_SERVER
extern AsyncWebSocket ws;
extern AsyncEventSource events;
void asyncWebServerInit();
void asyncWebSocketsInit();
void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);
#endif
