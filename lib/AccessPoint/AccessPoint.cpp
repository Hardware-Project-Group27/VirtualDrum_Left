#include "AccessPoint.h"

AccessPoint::AccessPoint() : started(false) {}

void AccessPoint::start() {
  WiFi.softAP(defaultAPSSID, defaultAPPassword);
  dnsServer.start(53, "*", WiFi.softAPIP());
  
  server.on("/", handleRoot);
  server.on("/save", handleSave);
  server.begin();

  Serial.println("Access Point Started");
  Serial.println("IP Address: " + WiFi.softAPIP().toString());
  started = true;
}

void AccessPoint::handleRoot() {
  String html = "<html><head><style>";
  html += "body { font-family: Arial, sans-serif; margin: 0; padding: 0; background-color: #f4f4f9; }";
  html += "form { max-width: 400px; margin: 50px auto; padding: 20px; background: white; border-radius: 8px; box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1); }";
  html += "input[type='text'], input[type='password'] { width: calc(100% - 22px); padding: 10px; margin-bottom: 10px; border: 1px solid #ccc; border-radius: 4px; }";
  html += "input[type='submit'] { background-color: #4CAF50; color: white; padding: 10px 15px; border: none; border-radius: 4px; cursor: pointer; }";
  html += "input[type='submit']:hover { background-color: #45a049; }";
  html += "</style></head><body><form action='/save' method='POST'>";
  html += "<h2>ESP32 Configuration</h2>";
  html += "SSID: <input type='text' name='ssid' value='" + ssid + "' required><br>";
  html += "Password: <input type='password' name='password' value='" + password + "' required><br>";
  html += "Server IP: <input type='text' name='serverIP' value='" + serverIP + "' required><br>";
  html += "Port: <input type='text' name='port' value='" + String(port) + "' required><br>";
  html += "<input type='submit' value='Save'></form></body></html>";

  server.send(200, "text/html", html);
}

bool AccessPoint::isStarted() {
  return started;
}


void AccessPoint::handleSave() {
  ssid = server.arg("ssid");
  password = server.arg("password");
  serverIP = server.arg("serverIP");
  port = server.arg("port").toInt();

  preferences.begin("wifi", false);
  preferences.putString("ssid", ssid);
  preferences.putString("password", password);
  preferences.putString("serverIP", serverIP);
  preferences.putInt("port", port); 
  preferences.end();

  server.send(200, "text/html", "Settings Saved. ESP will restart.");
  delay(2000);
  ESP.restart();
}
