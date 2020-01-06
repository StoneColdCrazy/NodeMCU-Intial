#include "webUpdater.h"
#include "../params.h"
#include <Updater.h>
#include <WiFiClientSecure.h>

ESP8266WebServer WebUpdater::webServer(WEBPORT);

String WEB_UPLOAD = "<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>"
"<form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>"
    "<input type='file' name='update'>"
    "<input type='submit' value='Update'>"
"</form>"
"<div id='prg'>progress: 0%</div>"
"<script>"
"$('form').submit(function(e){"
    "e.preventDefault();"
      "var form = $('#upload_form')[0];"
      "var data = new FormData(form);"
      " $.ajax({"
            "url: '/update',"
            "type: 'POST',"               
            "data: data,"
            "contentType: false,"                  
            "processData:false,"  
            "xhr: function() {"
                "var xhr = new window.XMLHttpRequest();"
                "xhr.upload.addEventListener('progress', function(evt) {"
                    "if (evt.lengthComputable) {"
                        "var per = evt.loaded / evt.total;"
                        "$('#prg').html('progress: ' + Math.round(per*100) + '%');"
                    "}"
               "}, false);"
               "return xhr;"
            "},"                                
            "success:function(d, s) {"    
                "console.log('success!')"
           "},"
            "error: function (a, b, c) {"
            "}"
          "});"
"});"
"</script>";

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
    webServer.sendHeader("Connection", "close");
    webServer.send(200, "text/html", WEB_UPLOAD);
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