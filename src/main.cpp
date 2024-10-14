#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "WManager.h"
#include "WifiControl.h"

void setup()
{
  WRC::setupManager();
  WRC::setupControl();

  WRC::startManager();
}

void loop()
{
  WRC::loopManager();
  WRC::loopControl();
}