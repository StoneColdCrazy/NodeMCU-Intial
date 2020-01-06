#include <Arduino.h>
#include "wifi/wifiFunctions.h"
#include "web/webUpdater.h"

/**
 * Presetup
 */
void setup(void)
{
    Serial.begin(115200);
    WifiFunctions::initWifi();
    WebUpdater::initWebServer();
}

/**
 * Loop
 */
void loop(void)
{
    WifiFunctions::handleLoop();
    WebUpdater::handleLoop();
    delay(1);
}