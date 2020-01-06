#ifndef __WEB_UPDATER_H__
#define __WEB_UPDATER_H__
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

/**
 * Functions to handle the complete web update
 */
class WebUpdater
{
private:
    static ESP8266WebServer webServer;
    static void redirectHome();
    static void showUploader();
    static void postUpdate();
    static void onRecvUploadFile();

public:
    static void initWebServer();
    static void handleLoop();
};

#endif /* #ifndef __WEB_UPDATER_H__ */