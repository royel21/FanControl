#include "WManager.h"

namespace WRC
{
    uint32_t lastTime = 0;
    struct Inputs {
        uint8_t pin;
        uint8_t out;
    };

    std::vector<Inputs> inputs = { {D4,  255}, {D3,  D7}, {D2,  D5}, {D1,  D6} };

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

    void configControl() {
        for (auto d : inputs) {
            pinMode(d.pin, INPUT_PULLUP);
            attachInterrupt(digitalPinToInterrupt(d.pin), isr, FALLING);
            if (d.out) {
                pinMode(d.out, OUTPUT);
            }
        }
    }
}
