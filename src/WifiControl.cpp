#include "WManager.h"
#include <ESPAsyncWebServer.h>

namespace WRC
{
    uint32_t lastTime = 0;
    struct Inputs {
        uint8_t pin;
        uint8_t out;
    };

    std::vector<Inputs> inputs = { {D4,  255}, {D3,  D7}, {D2,  D5}, {D1,  D6} };
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
        request->send(200, "text/plain", config.devname);
    }

    void getState(AsyncWebServerRequest* request) {
        request->send(200, "text/plain", String(currentState));
    }

    void changeState(AsyncWebServerRequest* request) {
        int speed = request->getParam("speed")->value().toInt();
        if (speed >= 0 && speed <= 3) {
            config.lastState = outs[speed];
        }

        request->send(200, "text/plain", "Ok");
    }

    void IRAM_ATTR isr() {
        uint32_t time = millis();
        if (time - lastTime > 100) {
            for (auto in : inputs) {
                if (!digitalRead(in.pin)) {
                    config.lastState = in.out;
                }
            }

            lastTime = time;
        }
    }

    void setupControl() {
        for (auto d : inputs) {
            pinMode(d.pin, INPUT_PULLUP);
            attachInterrupt(digitalPinToInterrupt(d.pin), isr, FALLING);
            if (d.out) {
                pinMode(d.out, OUTPUT);
            }
        }

        if (config.bootAp == false) {
            server.on("/", HTTP_GET, indexControl);
        }
        server.on("/reboot-ap", HTTP_GET, rebootOnAP);
        server.on("/change-state", HTTP_GET, changeState);
        server.on("/get-name", HTTP_GET, getDevName);
        server.on("/get-state", HTTP_GET, getState);
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
        onStateChange();
    }
}
