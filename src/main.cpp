#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "WManager.h"

uint32_t blinkSpeed = 250;

void setup()
{
  WRC::setupManager();
}

void loop()
{
  WRC::loop();
}