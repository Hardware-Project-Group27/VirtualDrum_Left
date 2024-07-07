#include <Battery.h>
#include <Adafruit_SSD1306.h>

#include <WebServer.h>
#include <DNSServer.h>
#include <Preferences.h>
#include "WiFiManager.h"
#include "AccessPoint.h"
#include "WebsocketCon.h"
#include "WSMsgRecievedHandler.h"

#include "menu.h"
#include "metronome.h"
#include "Handler.h"
#include "BatteryL.h"
#include "Piezo.h"

#define GLOVE_NO 0


const char *defaultAPSSID = "DRUM_GLOVE_0";
const char *defaultAPPassword = "";


String ssid;
String password;
String serverIP;
int port;

bool isConnectedToServer = false; // this will be set to true when the glove is connected to the server


WebServer server(80);
DNSServer dnsServer;
Preferences preferences;

WebSocketCon ws = WebSocketCon();
WiFiManager wifiManager = WiFiManager();
AccessPoint accessPoint = AccessPoint();
WSMsgRecievedHandler wsMsgRecievedHandler = WSMsgRecievedHandler(GLOVE_NO);


String command;
Menu menu = Menu();
Metronome metronome = Metronome();
Handler handler = Handler();
BatteryL batteryL = BatteryL(GLOVE_NO,ACTIVATION_PIN);
Piezo piezo = Piezo();
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


Btn upbtn = Btn(4);
Btn downbtn = Btn(13);
Btn selectbtn = Btn(12);
Btn backbtn = Btn(27);

 void serialDebuger();
 void nullFunction();
 void ShowHomeScreen();
 void goToWindow();
 void backToWindow();
 void updateWindow();
 void changeBtnFunctionContex();
 void printWindow();
 void onSelect();
 void onBack();
 void onUp();
 void onDown();

unsigned long wsDisconnectedTime = 0;
unsigned long timeAfterSetup = 0;

void setup() {

  Serial.begin(115200);

  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);

  ShowHomeScreen();
  delay(2000);
  wifiManager.loadCredentials();
  if (GLOVE_NO == 0) {
    defaultAPSSID = "DRUM_GLOVE_LEFT";
  } else {
    defaultAPSSID = "DRUM_GLOVE_RIGHT";
  }

  if (ssid == "" || password == "") {
    accessPoint.start();
  } else {
    if (wifiManager.connectToWiFi()) {
      Serial.println("\nConnected to WiFi");
      Serial.println("IP Address: " + WiFi.localIP().toString());
    
      ws.setup(GLOVE_NO);  // the global flag will be set to true by this if connect
      // we dont start the server connection here, the onevent needs to detect the connection
      // so we check in the main loop
      
    } else {
      Serial.println("\nFailed to connect to WiFi. Starting AP mode.");
      accessPoint.start();
    }
  }

  batteryL.BatteryInit(&display, &ws);
  wsMsgRecievedHandler.setBatteryL(&batteryL);


  menu.MenuInit(&display);
  menu.MenuSetItem("Metronome",&metronome.Open);
  menu.MenuSetItem("Battery",&nullFunction);
  menu.MenuSetItem("Reset",&esp_restart);
  menu.MenuSetItem("Exit",&ShowHomeScreen);

  handler.setFucnctions(&nullFunction,&nullFunction,&nullFunction,&nullFunction);
  upbtn.setup(onUp);
  downbtn.setup(onDown);
  selectbtn.setup(onSelect);
  backbtn.setup(onBack);

  metronome.MetronomeInit(&display);

  wsDisconnectedTime = millis(); 
  timeAfterSetup = millis();

}

void loop(){
    serialDebuger();

    if (!accessPoint.isStarted()) {
      ws.loop();
    }

      // if 4 seconds passed after setup
    if ((millis() - timeAfterSetup > 4000) && !isConnectedToServer && !accessPoint.isStarted()) {
      // if 4 seconds after last disconnection
      if (millis() - wsDisconnectedTime > 4000) {
        Serial.println("Failed to connect to server. Starting AP mode.");
        accessPoint.start();
      }
    }
    metronome.UpdateMetronome();
    // batteryL.setBattery1Level(batt.level());
    batteryL.measureBatteryLevel();
    // Serial.println("battery loop done");
    piezo.loop();
    // Serial.println("piezo loop done");
    // ws.loop();
    // Serial.println("Ws loop done");
    upbtn.check();
    downbtn.check();
    selectbtn.check();
    backbtn.check();


    // for wifi disconnection
  if (!wifiManager.isConnectedToWifi() && !accessPoint.isStarted()) {
      unsigned long startTime = millis();
      while (!wifiManager.connectToWiFi() && millis() - startTime < 5000) {
        delay(500);
        Serial.print(".");
      }

      if (wifiManager.isConnectedToWifi()) {
        Serial.println("\nConnected to WiFi");
        Serial.println("IP Address: " + WiFi.localIP().toString());
      } else {
        Serial.println("\nFailed to connect to WiFi. Starting AP mode.");
        accessPoint.start();
      }
  }

  if (accessPoint.isStarted()) {
    dnsServer.processNextRequest();
    // Serial.println("AP mode");
    server.handleClient();
  }

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
    onUp();
  }
    else if (command == "d")
  {
    onDown();
  }
      else if (command == "s")
  {
    onSelect();  
  }
  else if(command == "b"){
    onBack();
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

void onSelect(){
  handler.Select();
  goToWindow();    
  Serial.println("select");
  updateWindow();
  changeBtnFunctionContex();
}
void onBack(){
  handler.Back();
  backToWindow();
  Serial.println("back");
  updateWindow();
  changeBtnFunctionContex();
}
void onUp(){
  handler.Up();
  Serial.println("up");
  updateWindow();
  // changeBtnFunctionContex();
}
void onDown(){
  handler.Down();
  Serial.println("down");
  updateWindow();
  // changeBtnFunctionContex();
}

