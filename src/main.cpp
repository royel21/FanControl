#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "WManager.h"
#include "WifiControl.h"

uint32_t blinkSpeed = 500;

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  WiFi.begin(WRC::config.name, WRC::config.pass);

  WiFi.waitForConnectResult();

  if (WiFi.status() != WL_CONNECTED) {
    WRC::setupManager();
    blinkSpeed = 500;
  }
  else {
    blinkSpeed = 1000;
    WRCC::setupControls();
  }


}

uint32_t start = 0;
uint8_t state = 0;

void loop()
{
  if (millis() - start > blinkSpeed) {
    state = !state;
    digitalWrite(LED_BUILTIN, state);
    start = millis();
  }

  WRC::loop();
}