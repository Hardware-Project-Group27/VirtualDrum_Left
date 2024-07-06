#ifndef ACCESSPOINT_H
#define ACCESSPOINT_H

#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <Preferences.h>

extern WebServer server;
extern DNSServer dnsServer;
extern Preferences preferences;
extern String ssid;
extern String password;
extern String serverIP;
extern int port;

extern const char *defaultAPSSID;
extern const char *defaultAPPassword;

class AccessPoint {
public:
  AccessPoint();
  void start();
  static void handleRoot();
  static void handleSave();
  bool isStarted();

  private:
    bool started = false;
};

#endif 
