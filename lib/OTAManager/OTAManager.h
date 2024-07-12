#ifndef OTAMANAGER_H
#define OTAMANAGER_H

#include <ArduinoOTA.h>
#include <WebServer.h>
#include <Update.h>
#include <functional> 

typedef void (*MenuAlertCallBack)(String, String);

class OTAManager {
public:
    OTAManager();
    void begin(WebServer* server, MenuAlertCallBack menuAlertCallback);
    void handle();

private:
    void setupOTA();
    void setupWebUpdate(WebServer* server,  MenuAlertCallBack menuAlertCallback);
    MenuAlertCallBack menuAlert;
};

#endif // OTAMANAGER_H