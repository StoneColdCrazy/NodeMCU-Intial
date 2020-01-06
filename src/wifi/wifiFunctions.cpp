#include "wifiFunctions.h"
#include "../params.h"
#include <ESP8266mDNS.h>

/**
 * Intialize WiFi with manager
 */
void WifiFunctions::initWifi()
{
    WiFiManager wifiManager;
    wifiManager.setAPCallback(wifiManagerConfigModeCallback);   
    wifiManager.setConnectTimeout(30);

    String hostname(HOSTNAME);
    hostname += String(ESP.getChipId(), HEX);
    dbgSerialPrintln("Try autoconnect WiFi");

    if (!wifiManager.autoConnect((const char *)hostname.c_str())) {
        delay(3000);
        WiFi.disconnect(true);
        ESP.reset();
        delay(5000);
    }

    // start mdns
    if (!MDNS.begin(hostname)) {
        dbgSerialPrintln("Error setting up MDNS responder!");
        while(1) {
            delay(1000);
        }
    }
    dbgSerialPrintln("mDNS responder started");
    dbgSerialPrintln("WiFi autoconnect success");
    dbgSerialPrint("Connected with IP: ");
    dbgSerialPrintln(WiFi.localIP());
}

/**
 * Handle loop requests
 */
void WifiFunctions::handleLoop()
{
}

/**
 * We entered configuration mode
 */
void WifiFunctions::wifiManagerConfigModeCallback (WiFiManager *myWiFiManager)
{
    dbgSerialPrintln("Entered config mode");
    dbgSerialPrint("Listen on accesspoint and IP: ");
    dbgSerialPrintln(WiFi.softAPIP());
}


