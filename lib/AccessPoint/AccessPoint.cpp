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
  String html = R"rawliteral(
<!DOCTYPE html>
<html lang='en'>
<head>
    <meta charset='UTF-8'>
    <meta name='viewport' content='width=device-width, initial-scale=1.0'>
    <title>ESP32 Configuration</title>
    <link href='https://fonts.googleapis.com/css2?family=Russo+One&display=swap' rel='stylesheet'>
    <style>
        body {
            font-family: Arial, sans-serif;
            margin: 0;
            padding: 0;
            background-color: #0e0e0e;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            color: white;
        }

        .container {
            text-align: center;
        }

        .neonText {
            color: #fff;
            text-shadow:
                0 0 7px #fff,
                0 0 10px #282828,
                0 0 21px rgb(0, 221, 255),
                0 0 42px rgb(0, 221, 255),
                0 0 82px rgb(0, 221, 255),
                0 0 92px rgb(0, 221, 255),
                0 0 102px rgb(0, 221, 255),
                0 0 151px rgb(0, 221, 255);
        }

        .neonText-large {
            font-size: 50px;
            margin: 2px 0;
            font-weight: 700;
            font-family: 'Russo One', sans-serif;
        }

        .neonText-medium {
            font-size: 20px;
            margin: 2px 0;
            font-weight: 700;
            font-family: 'Russo One', sans-serif;
        }

        .neonText-small {
            font-size: 15px;
            margin: 6px 0;
            font-weight: 700;
            font-family: 'Russo One', sans-serif;
        }

        .form-box {
            background-color: #111;
            padding: 20px;
            border-radius: 10px;
            box-shadow: 0 0 20px rgba(0, 221, 255, 0.5);
            display: inline-block;
            text-align: left;
            width: 500px;
        }

        .form-box input[type='text'],
        .form-box input[type='password'] {
            width: calc(100% - 22px);
            padding: 10px;
            margin: 10px 0;
            border: none;
            border-radius: 5px;
            display: block;
        }

        .form-box input[type='submit'] {
            background-color: #4CAF50;
            color: white;
            cursor: pointer;
            font-weight: bold;
            padding: 8px;
            margin: 10px 0;
            border: none;
            border-radius: 5px;
            display: block;
            font-size: 15px;
        }

        .form-box input[type='submit']:hover {
            background-color: #45a049;
        }

        .form-box label {
            margin: 10px 0;
        }

        .header {
            text-align: center;
            margin-bottom: 40px;
        }

        @media (max-width: 600px) {
            .form-box {
                width: 90%;
                padding: 10px;
            }
        }
    </style>
</head>
<body>
    <div class='container'>
        <div class='header'>
            <p class='neonText neonText-large'>BeatFusion</p>
            <p class='neonText neonText-medium'>Virtual Drum Gloves</p>
            <p class='neonText neonText-small'>By IT Group 27</p>
        </div>
        <div class='form-box'>
            <form action='/save' method='POST'>
                <label for='ssid'>SSID:</label>
                <input type='text' id='ssid' name='ssid' required><br>
                <label for='password'>Password:</label>
                <input type='password' id='password' name='password' required><br>
                <label for='serverIP'>Server IP:</label>
                <input type='text' id='serverIP' name='serverIP' required><br>
                <label for='port'>Port:</label>
                <input type='text' id='port' name='port' required><br>
                <input type='submit' value='Save'>
            </form>
        </div>
    </div>
</body>
</html>
)rawliteral";

  server.send(200, "text/html", html);
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

bool AccessPoint::isStarted() {
  return started;
}
