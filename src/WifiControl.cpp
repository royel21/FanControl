#include "WifiControl.h"
#include "WManager.h"

namespace WRCC
{
    void setupControls() {
        WRC::server.on("/", [](AsyncWebServerRequest* request) {
            request->send(200, "text/plain", "ok");
            });

        WRC::server.begin();
    }
    void loop() {}
}