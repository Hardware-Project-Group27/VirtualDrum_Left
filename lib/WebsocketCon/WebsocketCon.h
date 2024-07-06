#ifndef WEBSOCKETCON
#define WEBSOCKETCON

#include <WebSockets.h>

class WSMsgRecievedHandler; // forward declaration 

extern String serverIP;
extern int port;

extern bool isConnectedToServer;
extern unsigned long wsDisconnectedTime;
class WebSocketCon{


    private:
        int gloveNo;    
        WSMsgRecievedHandler* wsMsgRecievedHandler;
        void setup_websocket();
        void hexdump(const void *mem, uint32_t len, uint8_t cols = 16);
        void webSocketEvent(WStype_t type, uint8_t * payload, size_t length);


    public:
        WebSocketCon();
        void setup(int thisGloveNo);
        void loop();
        void sendMsg(String msg);
        void setWSMsgRecievedHandler(WSMsgRecievedHandler* wsMsgRecievedHandler);

};

#endif