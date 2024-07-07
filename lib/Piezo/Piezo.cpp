#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include "WebsocketCon.h"
#include "Piezo.h"

long LastTriggeredTime[SENSOR_COUNT] = {0,0,0,0} ;
int LastReadValue[SENSOR_COUNT] = {0,0,0,0} ;
int triggerLevel = 0;
bool isEnable = false;


#define SENSOR_COUNT 4

#define THRESHOLD 10 // analog readings of below and equal to THRESHOLD will not be send to the server
#define CHECK_INTERVAL 40

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
        //triggerLevel = LastReadValue;
      }
      else if(reading < LastReadValue[sensorId]){
        triggerLevel = LastReadValue[sensorId];
      }
      else{
        triggerLevel = reading;
        LastTriggeredTime[sensorId] = millis();
      }
      if(triggerLevel>THRESHOLD){
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
  display.setCursor(30,40);
  if(isEnable){
    display.println("Enabled");
  }
  else{
    display.println("Disabled");
  }
  display.display();
}

void Piezo::Tougle(){
  isEnable = !isEnable;
}

void Piezo::SendSerialTrigerSignal(int sensorId, int reading){
  wsCon.sendMsg("play:drum" + String(sensorId+1) + ":" + String(reading));
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