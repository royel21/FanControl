#include "WManager.h"
#include <ESP8266WiFi.h>
#include <LittleFS.h>

namespace WRC
{
    // Webserver
    AsyncWebServer server(80);
    uint32_t scan = true;
    String list = "";
    String name = "";
    String passwd = "";
    Config config;
    uint8_t restart = false;

    void scanNet() {
        list = "";
        int n = WiFi.scanNetworks();
        for (int i = 0; i < n; i++)
        {
            list += WiFi.SSID(i);
            if (i < n - 1) {
                list += ",";
            }
        }
        scan = false;
    }
    void notFound(AsyncWebServerRequest* request) {
        request->send(200, "text/plain", "not found");
    }
    void indexFile(AsyncWebServerRequest* request)
    {
        request->send(LittleFS, "/index.html", "text/html");
    }

    void scanNetwork(AsyncWebServerRequest* request)
    {
        scan = true;
        request->send(200, "text/plain", list);
    }

    void saveNetwork(AsyncWebServerRequest* request) {
        String name = request->getParam("name")->value();
        String pass = request->getParam("pass")->value();
        String server = request->getParam("server")->value();
        config.save(name, pass, server);
        request->send(200, "text/plain", "Restarting...");
        restart = true;
    }

    void readConfig(AsyncWebServerRequest* request) {
        request->send(200, "text/plain", config.toString());
    }

    void setupManager() {
        WiFi.disconnect();
        WiFi.mode(WIFI_AP_STA);
        WiFi.softAP("ESP-config");
        LittleFS.begin();
        // Route for root index.html
        server.on("/", HTTP_GET, indexFile);
        server.on("/list", HTTP_GET, scanNetwork);
        server.on("/save", HTTP_GET, saveNetwork);
        server.on("/cfg", HTTP_GET, readConfig);
        server.onNotFound(notFound);
        scanNet();
        server.begin();
    }

    void loop()
    {
        if (scan) scanNet();
        if (restart) ESP.restart();
    }

    Config::Config() {
        EEPROM.begin(128);
        if (EEPROM.read(0)) {
            EEPROM.get(0, config);
        }
    }
    void Config::save(const String& n, const String& p, const String& srv) {
        // config.name
        strncpy(config.name, n.c_str(), 32);
        strncpy(config.pass, p.c_str(), 64);
        strncpy(config.server, srv.c_str(), 20);
        EEPROM.put(0, config);
        EEPROM.commit();
    }

    String Config::toString() {
        String content = String(config.name);
        content += ",";
        content += String(config.pass);
        content += ",";
        content += String(config.server);
        return content;
    }
} // namespace WRC