#ifndef __WIFI_FUNCTIONS_H__
#define __WIFI_FUNCTIONS_H__
#include <ESP8266WiFi.h>
#include <WiFiManager.h>

/**
 * Functions to handle the complete wifi initialize
 */
class WifiFunctions
{
private:  
    static void wifiManagerConfigModeCallback(WiFiManager *myWiFiManager);   

public:
    static void initWifi();
    static void handleLoop();
};

#endif /* #ifndef __WIFI_FUNCTIONS_H__ */