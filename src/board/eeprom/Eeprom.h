#include <Arduino.h>
#include <EEPROM.h>
#include "../../service/config/Config.h"
class EEPROMStringHandler {
public:
    EEPROMStringHandler(int eepromSize) {
        EEPROM.begin(eepromSize);
    }

    void writeString(int address, const String &data) {
        int len = data.length();
        EEPROM.write(address, len);
        for (int i = 0; i < len; i++) {
            EEPROM.write(address + 1 + i, data[i]);
        }
        EEPROM.commit();
    }

    String readString(int address) {
        int len = EEPROM.read(address);
        char data[len + 1];
        for (int i = 0; i < len; i++) {
            data[i] = EEPROM.read(address + 1 + i);
        }
        data[len] = '\0'; // Null-terminate the string
        return String(data);
    }
};

//EEPROMStringHandler eepromHandler(8);