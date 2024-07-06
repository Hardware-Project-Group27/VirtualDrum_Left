#include "WiFiManager.h"

WiFiManager::WiFiManager() {}

void WiFiManager::loadCredentials() {
  preferences.begin("wifi", true);
  ssid = preferences.getString("ssid", "");
  password = preferences.getString("password", "");
  serverIP = preferences.getString("serverIP", "");
  port = preferences.getInt("port", 7075);
  preferences.end();
}

bool WiFiManager::isConnectedToWifi() {
  return WL_CONNECTED;
}

// void WiFiManager::saveCredentials() {
//   preferences.begin("wifi", false);
//   preferences.putString("ssid", ssid);
//   preferences.putString("password", password);
//   preferences.putString("serverIP", serverIP);
//   preferences.putInt("port", port);
//   preferences.end();
// }

bool WiFiManager::connectToWiFi() {
  WiFi.begin(ssid.c_str(), password.c_str());

  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startTime < 5000) {
    delay(500);
    Serial.print(".");
  }

  return WiFi.status() == WL_CONNECTED;
}

