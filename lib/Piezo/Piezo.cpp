#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include "WebsocketCon.h"
#include "Piezo.h"

long LastTriggeredTime[SENSOR_COUNT] = {0,0,0,0} ;
int LastReadValue[SENSOR_COUNT] = {0,0,0,0} ;
int triggerLevel = 0;
bool isEnable = false;
unsigned int sensitivity = 3900;

Piezo::Piezo(){
    pinMode(PIEZO1_PIN, INPUT);
    pinMode(PIEZO2_PIN, INPUT);
    pinMode(PIEZO3_PIN, INPUT);
    pinMode(PIEZO4_PIN, INPUT);
}

void Piezo::PiezoInit(Adafruit_SSD1306 *d, WebSocketCon *ws) {
    display = *d;
    wsCon = *ws;

}

void Piezo::loop() {
  if(isEnable){
    for(int sensorId=0;sensorId<SENSOR_COUNT;sensorId++){
      int reading = ReadPiezoInput(sensorId);
      if((millis() - LastTriggeredTime[sensorId] < CHECK_INTERVAL) || (reading >= LastReadValue[sensorId])){
        LastReadValue[sensorId]=reading;
        continue;
      }
      else if(reading < LastReadValue[sensorId]){
        triggerLevel = LastReadValue[sensorId];
      }
      else{
        triggerLevel = reading;
        LastTriggeredTime[sensorId] = millis();
      }
      if(triggerLevel>MAX_SENSITIVITY-sensitivity){
        SendSerialTrigerSignal(sensorId,triggerLevel);
        LastReadValue[sensorId] = 0;
        LastTriggeredTime[sensorId] = millis();
        triggerLevel=0;
      }
    }
  }
}

void Piezo::UpdateDisplay(){
  Serial.println("Piezo updateDisplay");
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(22,10);
  display.println("Piezo");
  display.setTextSize(1);
  display.setCursor(20,35);
  if(isEnable){
    display.println("State: Enabled");
  }
  else{
    display.println("State: Disabled");
  }
  display.setCursor(20,45);
  display.println("Sensitivity: "+String(sensitivity));
  display.display();
}

bool Piezo::getIsEnabled(){
  return isEnable;
}

void Piezo::SensitivityUP(){
  if(sensitivity<MAX_SENSITIVITY){
    sensitivity+=1;
  }
}

void Piezo::SensitivityDown(){
  if(sensitivity>0){
    sensitivity-=1;
  }
}

void Piezo::Tougle(){
  isEnable = !isEnable;
}

void Piezo::SendSerialTrigerSignal(int sensorId, int reading){
  wsCon.sendMsg("play:drum" + String(sensorId+5) + ":" + String(reading)); // starting from play:drum4
  Serial.print("play:drum");
  Serial.print(sensorId+1);
  Serial.print(":");
  Serial.println(reading);
}


int Piezo::ReadPiezoInput(short c){
  int reading = 0;
  switch(c){
  case 0:
    reading = analogRead(PIEZO1_PIN);
    break;
  case 1:
    reading = analogRead(PIEZO2_PIN);
    break;
  case 2:
    reading = analogRead(PIEZO3_PIN);
    break;
  case 3:
    reading = analogRead(PIEZO4_PIN);
    break;
}
  return reading;
   
}