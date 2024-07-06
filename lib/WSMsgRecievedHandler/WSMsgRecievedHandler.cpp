#include "WSMsgRecievedHandler.h"
#include <WebSockets.h>
#include <Adafruit_SSD1306.h>
#include "WebsocketCon.h"
#include "BatteryL.h"


WSMsgRecievedHandler::WSMsgRecievedHandler(int thisGlove){
  thisGlove = thisGlove;
}

void WSMsgRecievedHandler::setBatteryL(BatteryL* batteryL) {
  this->batteryL = batteryL;
}

void WSMsgRecievedHandler::handleMessage(const String& message,  WebSocketCon* webSocketCon ) {
  Serial.println("Received message: " + message);

    // batLevels:60:75
    // 1st is bat level of left glove, 2nd is bat level of right glove
    
    if (message.startsWith("batLevels:")) {

      int colonIndex = message.indexOf(':');
      int secondColonIndex = message.indexOf(':', colonIndex + 1);

      String leftGloveBatLevelStr = message.substring(colonIndex + 1, secondColonIndex);
      String rightGloveBatLevelStr = message.substring(secondColonIndex + 1);

      int leftGloveBatLevel = leftGloveBatLevelStr.toInt();
      int rightGloveBatLevel = rightGloveBatLevelStr.toInt();

      
      if (batteryL != NULL) {

        // we dont want to overwrite the local battery level from the servers data
        // so we only should store the data of the other glove not this glove
        if (thisGlove != 0) {
          batteryL->setBattery1Level(leftGloveBatLevel);
        } else if (thisGlove != 1) {
          batteryL->setBattery2Level(rightGloveBatLevel);

      } else {
        // for testing
        Serial.println("BatteryL is NULL, It is ok if it is intentional");
      }

      // display?
      Serial.print("Left glove battery level: ");
      Serial.println(leftGloveBatLevel);
      Serial.print("Right glove battery level: ");
      Serial.println(rightGloveBatLevel);

      // testing for fun
    }

    else if (message.startsWith("ping:")) 
    {
      if (webSocketCon != NULL) {
      webSocketCon->sendMsg("pong");
      } else {
        // for testing
        Serial.println("WebSocketCon is NULL, It is ok if it is intentional");
      }

      Serial.println("Pong ");
    }
    
  }
}





