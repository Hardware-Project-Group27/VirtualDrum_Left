#include "OTAManager.h"

OTAManager::OTAManager() {}

void OTAManager::begin(WebServer* server, MenuAlertCallBack menuAlertCallback) {
    setupOTA();
    this->menuAlert = menuAlertCallback;
    setupWebUpdate(server, this->menuAlert);
}

void OTAManager::handle() {
    ArduinoOTA.handle();
}

void OTAManager::setupOTA() {
    ArduinoOTA.onStart([]() {
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH) {
            type = "sketch";
        } else {
            type = "filesystem";
        }
        Serial.println("Start updating " + type);
    });
    ArduinoOTA.onEnd([]() {
        Serial.println("\nEnd");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
        else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
        else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
        else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });
    ArduinoOTA.begin();
}

void OTAManager::setupWebUpdate(WebServer* server, MenuAlertCallBack menuAlertCallback) {
    server->on("/update", HTTP_GET, [server]() {
        server->sendHeader("Connection", "close");
        server->send(200, "text/html", 
            "<form method='POST' action='/update' enctype='multipart/form-data'>"
            "<input type='file' name='update'>"
            "<input type='submit' value='Update'>"
            "</form>");
    });


    server->on("/update", HTTP_POST, [server, menuAlertCallback]() {
        server->sendHeader("Connection", "close");
        server->send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
        Serial.println("Test print");

        ESP.restart();
    }, [server, menuAlertCallback]() {
        HTTPUpload& upload = server->upload();
        if (upload.status == UPLOAD_FILE_START) {
            Serial.printf("Update: %s\n", upload.filename.c_str());
            if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {
                Update.printError(Serial);
            }
        } else if (upload.status == UPLOAD_FILE_WRITE) {
            if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
                Update.printError(Serial);
            }
        } else if (upload.status == UPLOAD_FILE_END) {
            if (Update.end(true)) {
                Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
                menuAlertCallback("Success", "Firmware Update Success");

            } else {
                Update.printError(Serial);
                menuAlertCallback("Failed", "Firmware Update Failed");

            }
        }
    });
}