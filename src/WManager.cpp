#include "WManager.h"
#include <ESP8266WiFi.h>

namespace WRC
{
    // Webserver
    AsyncWebServer server(80);
    Config config;

    uint32_t scan = true;
    String list = "";
    String tempList = "";
    String name = "";
    String passwd = "";
    uint8_t restart = false;
    uint32_t start = 0;
    uint32_t blinkSpeed = 1000;
    std::vector<uint8_t> outs = { 255, D7, D5, D6 };

    uint8_t LED_STATUS = D0;

    void scanNet() {
        tempList = "";
        int n = WiFi.scanNetworks();
        for (int i = 0; i < n; i++)
        {
            tempList += WiFi.SSID(i);
            if (i < n - 1) {
                tempList += ",";
            }
        }
        list = tempList;
        scan = false;
    }

    void notFound(AsyncWebServerRequest* request) {
        request->send(200, "text/plain", "not found");
    }
    void indexFile(AsyncWebServerRequest* request)
    {
        request->send(LittleFS, "/index.html", "text/html");
    }

    void indexCSS(AsyncWebServerRequest* request)
    {
        request->send(LittleFS, "/m-index.css", "text/css");
    }

    void scanNetwork(AsyncWebServerRequest* request)
    {
        scan = true;
        request->send(200, "text/plain", list);
    }

    void saveNetwork(AsyncWebServerRequest* request) {
        String devname = request->getParam("devname")->value();
        String wname = request->getParam("wname")->value();
        String pass = request->getParam("pass")->value();
        String server = request->getParam("server")->value();
        config.save(devname, wname, pass, server);
        request->send(200, "text/plain", "Config Save - Restarting Now...");
        restart = true;
    }

    void readConfig(AsyncWebServerRequest* request) {
        request->send(200, "text/plain", config.toString());
    }

    void setupManager() {

        pinMode(LED_STATUS, OUTPUT);
        WiFi.mode(WIFI_STA);

        WiFi.begin(WRC::config.wifiname, WRC::config.pass);
        WiFi.waitForConnectResult();
        LittleFS.begin();

        if (WiFi.status() != WL_CONNECTED || config.bootAp) {
            scanNet();
            WiFi.disconnect();
            WiFi.mode(WIFI_AP);
            WiFi.softAP("ESP-config");
            // Route for root index.html
            server.on("/", HTTP_GET, indexFile);
            server.on("/list", HTTP_GET, scanNetwork);
            server.on("/save", HTTP_GET, saveNetwork);
            server.on("/cfg", HTTP_GET, readConfig);
            blinkSpeed = 250;
            config.bootAp = false;
            config.save();
        }

        server.on("/m-index.css", HTTP_GET, indexCSS);
    }

    void startManager() {
        server.onNotFound(notFound);
        server.begin();
    }

    uint8_t ledState = 0;

    void loopManager()
    {
        if (millis() - start > blinkSpeed) {
            ledState = !ledState;
            digitalWrite(LED_STATUS, ledState);
            start = millis();
        }

        if (scan) scanNet();
        if (restart) ESP.restart();
    }
} // namespace WRC