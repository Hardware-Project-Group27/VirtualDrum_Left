
#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include "WebsocketCon.h"
#include <Battery.h>
#include "BatteryL.h"

int battery1Level;
int battery2Level;
int thisGlove;
int activationPin;
unsigned long lastBatteryCheck = 0;
Battery battery = Battery(5000,8400, SENSE_PIN, ADC_RESOLUTION);


BatteryL::BatteryL(int thisGloveBatteryNo ,int BatteryActivationPin){
    thisGlove = thisGloveBatteryNo;
    activationPin = BatteryActivationPin;
    battery1Level = 0;
    battery2Level = 0;
    analogReadResolution(ADC_RESOLUTION);
    battery.onDemand(activationPin, HIGH);
    battery.begin(3300, 2.4925, &asigmoidal);
}

void BatteryL::BatteryInit(Adafruit_SSD1306 *d, WebSocketCon *ws) {
    display = *d;
    wsCon = *ws;
}

void BatteryL::UpdateDisplay() {
  Serial.println("battery level window Selected");
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);


    // Draw battery 1
    display.setCursor(0, 0);
    display.println("B1:");
    display.drawRect(40, 0, 30, 16, SSD1306_WHITE); // Draw battery outline
    display.fillRect(42, 2, 26 * battery1Level / 100, 12, SSD1306_WHITE); // Draw battery level

    // Draw battery 2
    display.setCursor(0, 40);
    display.println("B2:");
    display.drawRect(40, 40, 30, 16, SSD1306_WHITE); // Draw battery outline
    display.fillRect(42, 42, 26 * battery2Level / 100, 12, SSD1306_WHITE); // Draw battery level

    display.setCursor(80,0);
    display.println(String(battery1Level)+"%");

    display.setCursor(80,40);
    display.println(String(battery2Level)+"%");

    display.display();
    
}

int BatteryL::getBattery1Level(){
  return battery1Level;
}
int BatteryL::getBattery2Level(){
  return battery2Level;
}
void BatteryL::setBattery1Level(int level){
  battery1Level = level;
}
void BatteryL::setBattery2Level(int level){
  battery2Level = level;
}   

void BatteryL::measureBatteryLevel(){
    if(millis() - lastBatteryCheck > CHECK_INTERVAL){
      int batVoltage = battery.voltage(STABALIZE_DELAY);
      int batLevel = battery.level(batVoltage);
      if(thisGlove == 0){
        battery1Level = batLevel;
        wsCon.sendMsg("batReq:");
      }
      else if(thisGlove == 1){
        battery2Level = batLevel;
      }
      lastBatteryCheck = millis();


      String s = "bat:" + String(thisGlove) + ":" + String(batLevel);
      wsCon.sendMsg(s);

      Serial.print("Battery Level ");
      Serial.println(batLevel);
      Serial.print("Battery Voltage ");
      Serial.println(batVoltage);
    }
}

