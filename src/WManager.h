#pragma once
#include <Arduino.h>
#include <EEPROM.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

namespace WRC
{
    constexpr uint8_t NAME_SIZE = 32;
    constexpr uint8_t PASSWORD_SIZE = 64;
    constexpr uint8_t SERVER_SIZE = 20;

    struct Config {
        char name[NAME_SIZE] = { 0 };
        char pass[PASSWORD_SIZE] = { 0 };
        char server[SERVER_SIZE] = { 0 };
        Config();
        void save(const String& n, const String& p, const String& srv);
        String toString();
    };

    extern Config config;
    extern AsyncWebServer server;

    void setupManager();
    void loop();
} // namespace RC