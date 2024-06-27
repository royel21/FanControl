#pragma once
#include <Arduino.h>
#include <LittleFS.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "Config.h"

namespace WRC
{
    extern Config config;
    extern AsyncWebServer server;
    extern std::vector<uint8_t> outs;
    extern uint8_t LED_STATUS;

    //Setup server and controls
    void setupManager();

    //keep server going
    void loopManager();

    void startManager();
} // namespace RC