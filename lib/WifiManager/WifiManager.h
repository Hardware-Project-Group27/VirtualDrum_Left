#ifndef WIFIMANAGER_H
#define WIFIMANAGER_H

#include <WiFi.h>
#include <Preferences.h>

extern Preferences preferences;
extern String ssid;
extern String password;
extern String serverIP;
extern int port;


class WiFiManager {
public:
  WiFiManager();
  void loadCredentials();
//   void saveCredentials();
  bool isConnectedToWifi();
  bool connectToWiFi();
};

#endif
