#pragma once
#include <Arduino.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "Config.h"

namespace WRC
{
    extern Config config;
    extern AsyncWebServer server;

    //Setup server and controls
    void setupManager();

    //keep server going
    void loop();

    //Don't call this function
    void configControl();
} // namespace RC