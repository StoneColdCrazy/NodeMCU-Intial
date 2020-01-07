#include "webUpdater.h"
#include "../params.h"
#include <Updater.h>
#include <WiFiClientSecure.h>
#include "uploadPage.h"

ESP8266WebServer WebUpdater::webServer(WEBPORT);

/**
 * Intialize webserver
 */
void WebUpdater::initWebServer()
{
    webServer.on("/", HTTP_GET, showUploader);
    webServer.on("/update", HTTP_POST, postUpdate, onRecvUploadFile);
    webServer.onNotFound(redirectHome);
    webServer.begin();
}

/**
 * Intialize webserver calls
 */
void WebUpdater::handleLoop()
{
    webServer.handleClient();
}

/**
 * Redirect response to web client
 */
void WebUpdater::redirectHome() 
{
    dbgSerialPrintln("Redirect home");
    webServer.sendHeader("Location", String("/"), true);
    webServer.sendHeader("Cache-Control", "no-cache, no-store");
    webServer.sendHeader("Pragma", "no-cache");
    webServer.sendHeader("Expires", "-1");
    webServer.send(302, "text/plain", "");
    webServer.client().stop();
}

/**
 * Show bin uploader
 */
void WebUpdater::showUploader()
{
    String hostname(HOSTNAME);
    hostname += String(ESP.getChipId(), HEX);

    String targetOta = "";
    targetOta += "HostName: " + hostname + "<br>";
    targetOta += "IP: " + WiFi.localIP().toString() + "<br>";
    targetOta += "Port: " + String(ARDUINO_OTA_PORT) + "<br>";

    String webPage = WEB_UPLOAD_PAGE;
    webPage.replace("%target%", targetOta);

    webServer.sendHeader("Connection", "close");
    webServer.send(200, "text/html", webPage);
}

/**
 * When posting new binary
 */
void WebUpdater::postUpdate()
{
    webServer.sendHeader("Connection", "close");
    webServer.send(200, "text/plain", ( Update.hasError() ) ? "FAIL" : "OK");
    //esp_wifi_wps_disable();
    ESP.restart();
}

/**
 * When recving uploaded binary
 */
void WebUpdater::onRecvUploadFile()
{
    HTTPUpload& upload = webServer.upload();
    if(upload.status == UPLOAD_FILE_START) {
        dbgSerialPrintln("Update: " + String(upload.filename.c_str()));
        if(!Update.begin(-1)) { //start with max available size
            Update.printError(dbgSerial);
        }
    }
    else if(upload.status == UPLOAD_FILE_WRITE) {
        /* flashing firmware to ESP*/
        if(Update.write(upload.buf, upload.currentSize) != upload.currentSize){
            Update.printError(dbgSerial);
        }
    }
    else if(upload.status == UPLOAD_FILE_END) {
        if(Update.end(true)){ //true to set the size to the current progress
            dbgSerialPrintln("Update Success: " + String(upload.totalSize));
            dbgSerialPrintln("Rebooting...");
        } else {
            Update.printError(dbgSerial);
        }
    }
}