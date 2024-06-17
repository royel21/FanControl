#include "WManager.h"
#include <ESP8266WiFi.h>
#include <LittleFS.h>

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
    uint8_t state = 0;
    uint32_t blinkSpeed = 250;
    uint8_t currentState = 255;
    std::vector<uint8_t> outs = { 255, D7, D5, D6 };

    uint8_t StatusLed = D0;

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

    void indexControl(AsyncWebServerRequest* request)
    {
        request->send(LittleFS, "/control.html", "text/html");
    }

    void indexCSS(AsyncWebServerRequest* request)
    {
        request->send(LittleFS, "/index.css", "text/css");
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

    void changeState(AsyncWebServerRequest* request) {
        int speed = request->getParam("speed")->value().toInt();
        if (speed >= 0 && speed <= 3) {
            config.lastState = outs[speed];
        }

        request->send(200, "text/plain", "Ok");
    }

    void getState(AsyncWebServerRequest* request) {
        request->send(200, "text/plain", String(currentState));
    }

    void onStateChange() {
        if (config.lastState != currentState) {
            currentState = config.lastState;
            for (auto nstate : outs) {
                digitalWrite(nstate, LOW);
            }
            digitalWrite(currentState, HIGH);
            config.save();
        }
    }

    void rebootOnAP(AsyncWebServerRequest* request) {
        config.bootAp = true;
        config.save();
        request->send(200, "text/plain", "Ok");
        restart = true;
    }

    void getDevName(AsyncWebServerRequest* request) {
        request->send(200, "text/plain", config.devname);
    }

    void setupManager() {

        pinMode(StatusLed, OUTPUT);
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
        else {
            blinkSpeed = 1000;
            server.on("/", HTTP_GET, indexControl);
            server.on("/reboot-ap", HTTP_GET, rebootOnAP);
        }

        server.on("/get-name", HTTP_GET, getDevName);
        server.on("/index.css", HTTP_GET, indexCSS);
        server.on("/change-state", HTTP_GET, changeState);
        server.on("/get-state", HTTP_GET, getState);

        server.onNotFound(notFound);
        server.begin();

        configControl();
    }

    void loop()
    {
        if (millis() - start > blinkSpeed) {
            state = !state;
            digitalWrite(StatusLed, state);
            start = millis();
        }

        if (scan) scanNet();
        if (restart) ESP.restart();
        onStateChange();
    }
} // namespace WRC