#include <Battery.h>
#include <Adafruit_SSD1306.h>
#include "menu.h"
#include "metronome.h"
#include "Handler.h"
#include "WebsocketCon.h"
#include "BatteryL.h"
#include "Piezo.h"

#define GLOVE_NO 0



String command;
Menu menu = Menu();
Metronome metronome = Metronome();
Handler handler = Handler();
WebSocketCon ws = WebSocketCon();
BatteryL batteryL = BatteryL(GLOVE_NO,ACTIVATION_PIN);
Piezo piezo = Piezo();
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

 void serialDebuger();
 void nullFunction();
 void ShowHomeScreen();
 void goToWindow();
 void backToWindow();
 void updateWindow();
 void changeBtnFunctionContex();
 void printWindow();

void setup() {

  Serial.begin(115200);
  
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  ws.setup();

  ShowHomeScreen();
  delay(2000);


  batteryL.BatteryInit(&display, &ws);

  menu.MenuInit(&display);
  menu.MenuSetItem("Metronome",&metronome.Open);
  menu.MenuSetItem("Battery",&nullFunction);
  menu.MenuSetItem("Reset",&esp_restart);
  menu.MenuSetItem("Exit",&ShowHomeScreen);

  handler.setFucnctions(&nullFunction,&nullFunction,&nullFunction,&nullFunction);

  metronome.MetronomeInit(&display);

}

void loop(){
    serialDebuger();
    metronome.UpdateMetronome();
    // batteryL.setBattery1Level(batt.level());
    batteryL.measureBatteryLevel();
    // Serial.println("battery loop done");
    piezo.loop();
    // Serial.println("piezo loop done");
    ws.loop();
    // Serial.println("Ws loop done");
}




void nullFunction(){
  Serial.println("function invoked");
}
 

void showBatteryLevel(){
  delay(1000);
  Serial.print("current battery level: ");
  // Serial.println(batt.level());
}

void ShowHomeScreen(){
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(22,10);
  display.println("Group27");
  display.setCursor(30,40);
  display.println("Drum");
  display.display();
}

void serialDebuger(){
  bool isExecuted = true;
  if (Serial.available())
  {
    command = Serial.readStringUntil('\n');
  }
  if (command == "test")
  {
    Serial.println("test pass");
  }
  else if (command == "u")
  {
    // menu.MenuUp();
    // menu.UpdateMenu();
    handler.Up();
  }
    else if (command == "d")
  {
    // menu.MenuDown();
    // menu.UpdateMenu();
    handler.Down();
  }
      else if (command == "s")
  {
    // menu.MenuSelect();
    // menu.UpdateMenu();
    handler.Select();
    goToWindow();    
  }
  else if(command == "b"){
    handler.Back();
    backToWindow();
  }
  else
  {
    isExecuted = false;
  }
  if (isExecuted)
  {
   updateWindow();
   changeBtnFunctionContex();
  }
  
  command = "";
}

void backToWindow(){
  switch (handler.currentWindow){
    case WINDOW_MENU:
      handler.currentWindow = WINDOW_HOME;
      break;
    case WINDOW_BATTERY:
      handler.currentWindow = WINDOW_MENU;
      break;
    case WINDOW_METRONOME:
      handler.currentWindow = WINDOW_MENU;
      break;
  }
  printWindow();
}

void goToWindow(){
  if(handler.currentWindow == WINDOW_MENU){
    switch (menu.getSelectedIndex())
      {
      case 0:
        handler.currentWindow = WINDOW_METRONOME;
        break;

      case 1:
        handler.currentWindow = WINDOW_BATTERY;
        break;

      case 3:
        handler.currentWindow = WINDOW_HOME;
      
      default:
        handler.currentWindow = WINDOW_HOME;
        break;
      }
  }
  else if(handler.currentWindow == WINDOW_HOME){
    handler.currentWindow = WINDOW_MENU;
  }
  else if(handler.currentWindow == WINDOW_BATTERY){
    handler.currentWindow = WINDOW_MENU;
  }
  printWindow();
}

void printWindow(){
      Serial.print("current window is ");
    Serial.println(handler.currentWindow);
}

void updateWindow(){
    if(handler.currentWindow == WINDOW_MENU){
      menu.UpdateMenu();
    }
    else if(handler.currentWindow == WINDOW_METRONOME){
      metronome.UpdateDisplay();
    }
    else if(handler.currentWindow == WINDOW_HOME){
      ShowHomeScreen();
    }
    else if(handler.currentWindow == WINDOW_BATTERY){
      batteryL.UpdateDisplay();
    }
    else{
      handler.currentWindow = WINDOW_HOME;
      ShowHomeScreen();
    }
}


void changeBtnFunctionContex(){
    switch (handler.currentWindow)
    {
    case WINDOW_MENU:
      handler.setFucnctions(menu.MenuUp,menu.MenuDown,menu.MenuSelect, menu.MenuBack);
      break;
    
    case WINDOW_HOME:
      handler.setFucnctions(&nullFunction,&nullFunction,&nullFunction,&nullFunction);
      break;

    case WINDOW_BATTERY:
      handler.setFucnctions(&nullFunction,&nullFunction,&nullFunction,&nullFunction);
      break;

    case WINDOW_METRONOME:
      handler.setFucnctions(&metronome.MetronomeUp,&metronome.MetronomeDown,&metronome.Tougle,&nullFunction);
      break;

    default:
      break;
    }
}

