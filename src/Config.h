#pragma once
#include <Arduino.h>

constexpr uint8_t NAME_SIZE = 64;
constexpr uint8_t PASSWORD_SIZE = 64;
constexpr uint8_t SERVER_SIZE = 24;

struct Config {
    char devname[NAME_SIZE] = "";
    char wifiname[NAME_SIZE] = "";
    char pass[PASSWORD_SIZE] = "";
    char server[SERVER_SIZE] = "";
    uint8_t lastState = 255;
    uint8_t bootAp = false;

    Config();
    void save();
    void save(const String& dvn, const String& n, const String& p, const String& srv);
    String toString();
};