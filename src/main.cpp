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
#include "OTAManager.h"

#define GLOVE_NO 0

unsigned long alertTime = 9000000; // show setup alert for 15 minutes

const char *defaultAPSSID = "DRUM_GLOVE";
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
BatteryL batteryL = BatteryL(GLOVE_NO, ACTIVATION_PIN);
Piezo piezo = Piezo();
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
OTAManager otaManager;

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
bool canAllowBtnAction();
void MenuAlertWrapperFunc(String, String);

unsigned long wsDisconnectedTime = 0;
unsigned long timeAfterSetup = 0;

void setup()
{

  Serial.begin(115200);
  pinMode(PIN_MOTOR, OUTPUT);
  analogWrite(PIN_MOTOR, 0);

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }

  ws.setWSMsgRecievedHandler(&wsMsgRecievedHandler);
  batteryL.BatteryInit(&display, &ws);
  wsMsgRecievedHandler.setBatteryL(&batteryL);

  ShowHomeScreen();
  menu.MenuInit(&display);
  // delay(2000);
  wifiManager.loadCredentials();
  if (GLOVE_NO == 0)
  {
    defaultAPSSID = "DRUM_GLOVE_LEFT";
  }
  else
  {
    defaultAPSSID = "DRUM_GLOVE_RIGHT";
  }

  if (ssid == "" || password == "")
  {
    accessPoint.start();
    menu.Alert("Config", "Connect to:\n" + String(defaultAPSSID) + " Wifi network & Visit\nhttp://esp.local");
  }
  else
  {
    if (wifiManager.connectToWiFi())
    {
      Serial.println("\nConnected to WiFi");
      Serial.println("IP Address: " + WiFi.localIP().toString());
      menu.Alert("Connected", "IP Address:\n" + WiFi.localIP().toString() + "\nWaiting for server");

      ws.setup(GLOVE_NO); // the global flag will be set to true by this if connect
      // we dont start the server connection here, the onevent needs to detect the connection
      // so we check in the main loop
    }
    else
    {
      Serial.println("\nFailed to connect to WiFi. Starting AP mode.");
      menu.Alert("Failed", "Connection failed!,\nConnect to " + String(defaultAPSSID) + " & Visit\nhttp://esp.local");
      accessPoint.start();
      otaManager.begin(&server, MenuAlertWrapperFunc);

    }
  }

  menu.MenuSetItem("Metronome", &metronome.Open);
  menu.MenuSetItem("Battery", &nullFunction);
  menu.MenuSetItem("Piezo", &nullFunction);
  menu.MenuSetItem("Reset", &esp_restart);
  handler.setFucnctions(&nullFunction, &nullFunction, &nullFunction, &nullFunction);

  piezo.PiezoInit(&display, &ws);

  upbtn.setup(onUp);
  downbtn.setup(onDown);
  selectbtn.setup(onSelect);
  backbtn.setup(onBack);
  metronome.MetronomeInit(&display);

  wsDisconnectedTime = millis();
  timeAfterSetup = millis();

  if (!accessPoint.isStarted())
  {
    alertTime = 1500;
  }
}

void loop()
{
  serialDebuger();

  if (!accessPoint.isStarted())
  {
    piezo.loop();
    ws.loop();
  }

  // if 4 seconds passed after setup
  if ((millis() - timeAfterSetup > 4000) && !isConnectedToServer && !accessPoint.isStarted())
  {
    // if 4 seconds after last disconnection
    if (millis() - wsDisconnectedTime > 4000)
    {
      Serial.println("Failed to connect to server. Starting AP mode.");
      alertTime = 9000000;
      menu.Alert("Failed", "Server Not Found!,\nConnect to " + String(defaultAPSSID) + " & Visit\nhttp://esp.local");
      accessPoint.start();
      otaManager.begin(&server, MenuAlertWrapperFunc);  // Initialize OTA

    }
  }


  metronome.UpdateMetronome();
  batteryL.measureBatteryLevel();

  upbtn.check();
  downbtn.check();
  selectbtn.check();
  backbtn.check();

  // clear Alert after ALERT_TIME
  if (menu.isAlertShown && millis() - menu.getAlertShownTime() > alertTime)
  {
    menu.ClearAlert();
    updateWindow();
  }

  // for wifi disconnection
  if (!wifiManager.isConnectedToWifi() && !accessPoint.isStarted())
  {
    unsigned long startTime = millis();
    while (!wifiManager.connectToWiFi() && millis() - startTime < 5000)
    {
      delay(500);
      Serial.print(".");
    }

    if (wifiManager.isConnectedToWifi())
    {
      Serial.println("\nConnected to WiFi");
      Serial.println("IP Address: " + WiFi.localIP().toString());
    }
    else
    {
      Serial.println("\nFailed to connect to WiFi. Starting AP mode.");
      accessPoint.start();
    }
  }

  if (accessPoint.isStarted())
  {
    dnsServer.processNextRequest();
    // Serial.println("AP mode");
    server.handleClient();
    otaManager.handle();  // Handle OTA updates

  }
}


void MenuAlertWrapperFunc(String title, String message) {
    menu.Alert(title, message);
}

void nullFunction()
{
  Serial.println("function invoked");
}

void showBatteryLevel()
{
  delay(1000);
  Serial.print("current battery level: ");
  // Serial.println(batt.level());
}

void ShowHomeScreen()
{
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(8, 10);
  display.println("BeatFusion"); // Print text
  display.setCursor(20, 40);
  display.println("Group 27");
  display.display();
}

void serialDebuger()
{
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
  else if (command == "b")
  {
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

void backToWindow()
{
  switch (handler.currentWindow)
  {
  case WINDOW_MENU:
    handler.currentWindow = WINDOW_HOME;
    break;
  case WINDOW_BATTERY:
    handler.currentWindow = WINDOW_MENU;
    break;
  case WINDOW_METRONOME:
    handler.currentWindow = WINDOW_MENU;
    break;
  case WINDOW_PIEZO:
    handler.currentWindow = WINDOW_MENU;
    break;
  }
  printWindow();
}

void goToWindow()
{
  if (handler.currentWindow == WINDOW_MENU)
  {
    switch (menu.getSelectedIndex())
    {
    case 0:
      handler.currentWindow = WINDOW_METRONOME;
      break;

    case 1:
      handler.currentWindow = WINDOW_BATTERY;
      break;

    case 2:
      handler.currentWindow = WINDOW_PIEZO;
      break;

    default:
      handler.currentWindow = WINDOW_HOME;
      break;
    }
  }
  else if (handler.currentWindow == WINDOW_HOME)
  {
    handler.currentWindow = WINDOW_MENU;
  }
  else if (handler.currentWindow == WINDOW_BATTERY)
  {
    handler.currentWindow = WINDOW_MENU;
  }
  printWindow();
}

void printWindow()
{
  Serial.print("current window is ");
  Serial.println(handler.currentWindow);
}

void updateWindow()
{
  if (handler.currentWindow == WINDOW_MENU)
  {
    menu.UpdateMenu();
  }
  else if (handler.currentWindow == WINDOW_METRONOME)
  {
    metronome.UpdateDisplay();
  }
  else if (handler.currentWindow == WINDOW_HOME)
  {
    ShowHomeScreen();
  }
  else if (handler.currentWindow == WINDOW_BATTERY)
  {
    batteryL.UpdateDisplay();
  }
  else if (handler.currentWindow == WINDOW_PIEZO)
  {
    piezo.UpdateDisplay();
    Serial.println("piezo window Selected");
  }
  else
  {
    handler.currentWindow = WINDOW_HOME;
    ShowHomeScreen();
  }
}

void changeBtnFunctionContex()
{
  switch (handler.currentWindow)
  {
  case WINDOW_MENU:
    handler.setFucnctions(menu.MenuUp, menu.MenuDown, menu.MenuSelect, menu.MenuBack);
    break;

  case WINDOW_HOME:
    handler.setFucnctions(&nullFunction, &nullFunction, &nullFunction, &nullFunction);
    break;

  case WINDOW_BATTERY:
    handler.setFucnctions(&nullFunction, &nullFunction, &nullFunction, &nullFunction);
    break;

  case WINDOW_METRONOME:
    handler.setFucnctions(&metronome.MetronomeUp, &metronome.MetronomeDown, &metronome.Tougle, &nullFunction);
    break;

  case WINDOW_PIEZO:
    handler.setFucnctions(&piezo.SensitivityUP, &piezo.SensitivityDown, &piezo.Tougle, &nullFunction);
    break;

  default:
    break;
  }
}

bool canAllowBtnAction()
{
  if (alertTime < 5000 && menu.isAlertShown)
  {
    return false;
  }
  else if (menu.isAlertShown)
  {
    menu.ClearAlert();
    alertTime = 1500;
    return true;
  }
  else
  {
    return true;
  }
}

void onSelect()
{
  if (canAllowBtnAction())
  {
    handler.Select();
    goToWindow();
    Serial.println("select");
    updateWindow();
    changeBtnFunctionContex();
  }
}
void onBack()
{
  if (canAllowBtnAction())
  {
    handler.Back();
    backToWindow();
    Serial.println("back");
    updateWindow();
    changeBtnFunctionContex();
  }
}
void onUp()
{
  if (canAllowBtnAction())
  {
    handler.Up();
    Serial.println("up");
    updateWindow();
    // changeBtnFunctionContex();
  }
}
void onDown()
{
  if (canAllowBtnAction())
  {
    handler.Down();
    Serial.println("down");
    updateWindow();
    // changeBtnFunctionContex();
  }
}
