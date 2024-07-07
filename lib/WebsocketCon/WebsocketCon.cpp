#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <WebSocketsClient.h>
#include <WiFi.h>
#include <WebSockets.h>
#include "WebsocketCon.h"
#include "WSMsgRecievedHandler.h"
#include "WiFi_Pass.h"



WebSocketsClient  webSocket;

WebSocketCon::WebSocketCon() {
  // costructor

}


void WebSocketCon::setup_websocket() {

    webSocket.begin(serverIP, port, "/");
    // event handler
    // webSocket.onEvent(webSocketEvent);
	 webSocket.onEvent([this](WStype_t type, uint8_t * payload, size_t length) {
        this->webSocketEvent(type, payload, length);
    });
    // use HTTP Basic Authorization this is optional remove if not needed
    // webSocket.setAuthorization("user", "Password");
	webSocket.enableHeartbeat(15000, 3000, 2);
    webSocket.setReconnectInterval(1000);
}

void WebSocketCon::setup(int gloveNo)
{
  this->gloveNo = gloveNo;

  setup_websocket();

  
}


void WebSocketCon::setWSMsgRecievedHandler(WSMsgRecievedHandler* handler) {
  wsMsgRecievedHandler = handler;
}



/* websocket functions */
void WebSocketCon::hexdump(const void *mem, uint32_t len, uint8_t cols) {
	const uint8_t* src = (const uint8_t*) mem;
	Serial.printf("\n[HEXDUMP] Address: 0x%08X len: 0x%X (%d)", (ptrdiff_t)src, len, len);
	for(uint32_t i = 0; i < len; i++) {
		if(i % cols == 0) {
			Serial.printf("\n[0x%08X] 0x%08X: ", (ptrdiff_t)src, i);
		}
		Serial.printf("%02X ", *src);
		src++;
	}
	Serial.printf("\n");
}

void WebSocketCon::webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
	switch(type) {
		case WStype_DISCONNECTED:
			Serial.printf("[WSc] Disconnected!\n");
			if (isConnectedToServer) {
				wsDisconnectedTime = millis(); // get the time at first disconnect
			}
			isConnectedToServer = false;

			break;
		case WStype_CONNECTED:
			Serial.printf("[WSc] Connected to url: %s\n", payload);
			isConnectedToServer = true;
			Serial.printf("isConnectedToServer: %d\n", isConnectedToServer);
			// send message to server when Connected
			webSocket.sendTXT("con:" + String(gloveNo));
			break;
		case WStype_TEXT:
			Serial.printf("[WSc] get text: %s\n", payload);

			// handleMsg((char*)payload, this);
			wsMsgRecievedHandler->handleMessage((char*)payload, this);

			break;
		case WStype_BIN:
			Serial.printf("[WSc] get binary length: %u\n", length);
			hexdump(payload, length);

			// send data to server
			// webSocket.sendBIN(payload, length);
			break;
		case WStype_ERROR:			
		case WStype_FRAGMENT_TEXT_START:
		case WStype_FRAGMENT_BIN_START:
		case WStype_FRAGMENT:
		case WStype_FRAGMENT_FIN:
			break;
	}

}

void WebSocketCon::sendMsg(String msg) {
  // send message to server
  
  webSocket.sendTXT(msg);
}


void WebSocketCon::loop()
{
  webSocket.loop();
}

