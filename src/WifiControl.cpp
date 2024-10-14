#include "WManager.h"
#include <ESPAsyncWebServer.h>
#include <HCSR04.h>

UltraSonicDistanceSensor distanceSensor(D8, D7, 170);

double distance = 0;

namespace WRC
{
    uint32_t lastTime = 0;

    uint8_t currentState = 255;

    void indexControl(AsyncWebServerRequest* request)
    {
        request->send(LittleFS, "/control.html", "text/html");
    }

    void rebootOnAP(AsyncWebServerRequest* request) {
        config.bootAp = true;
        config.save();
        request->send(200, "text/plain", "Ok");
    }

    void getDevName(AsyncWebServerRequest* request) {

        distance = distanceSensor.measureDistanceCm();
        String content = String(config.devname);
        content += ",";
        content += String(config.min);
        content += ",";
        content += String(config.max);
        request->send(200, "text/plain", content);
    }

    void getState(AsyncWebServerRequest* request) {
        distance = distanceSensor.measureDistanceCm();
        request->send(200, "text/plain", String(distance));
    }

    void saveDisc(AsyncWebServerRequest* request) {
        String val = request->getParam("min")->value();
        String val2 = request->getParam("max")->value();
        config.min = val.toInt();
        config.max = val2.toInt();
        config.save();

        String content = "save: ";
        content += ",";
        content += val;
        content += ",";
        content += val2;
        content += ",";
        content += String(config.min);
        content += ",";
        content += String(config.max);
        request->send(200, content);
    }

    void setupControl() {
        if (config.bootAp == false) {
            server.on("/", HTTP_GET, indexControl);
            server.on("/reboot-ap", HTTP_GET, rebootOnAP);
            server.on("/get-name", HTTP_GET, getDevName);
            server.on("/get-state", HTTP_GET, getState);
            server.on("/save-disc", HTTP_GET, saveDisc);
        }
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

    void loopControl() {
    }
}
