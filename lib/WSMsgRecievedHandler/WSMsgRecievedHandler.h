#ifndef WSMSGRECEIVEDHANDLER
#define WSMSGRECEIVEDHANDLER
#include <Arduino.h>
class BatteryL; // Forward declaration
class WebSocketCon; // Forward declaration

class WSMsgRecievedHandler {
   private:
    BatteryL* batteryL; 
    int thisGlove;
    
  public:
    WSMsgRecievedHandler(int thisGlove);
    void handleMessage(const String& message, WebSocketCon* webSocketCon );
    void setBatteryL(BatteryL* batteryL);

 
};
#endif
