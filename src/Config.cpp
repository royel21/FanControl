#include "Config.h"
#include <EEPROM.h>

Config::Config() {
    EEPROM.begin(256);
    if (EEPROM.read(0) != 1) {
        EEPROM.write(0, 1);
        save();
    }
    else {
        EEPROM.get(1, *this);
    }
}

void Config::save() {
    EEPROM.put(1, *this);
    EEPROM.commit();
}
void Config::save(const String& dvn, const String& n, const String& p, const String& srv) {
    strncpy(devname, dvn.c_str(), dvn.length());
    strncpy(wifiname, n.c_str(), n.length());
    strncpy(pass, p.c_str(), p.length());
    strncpy(server, srv.c_str(), srv.length());
    EEPROM.put(1, *this);
    EEPROM.commit();
}

String Config::toString() {
    String content = String(devname);
    content += ",";
    content += String(wifiname);
    content += ",";
    content += String(pass);
    content += ",";
    content += String(server);
    return content;
}