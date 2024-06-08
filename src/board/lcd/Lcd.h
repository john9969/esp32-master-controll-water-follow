#ifndef LCD__H
#define LCD__H

#include <Arduino.h>
#include <iostream>
#include <memory>
#include <vector>

#include "../../service/config/Config.h"
#include "../../lib/lcd-iic/LiquidCrystal_I2C.h"

#define LCD_ADDRESS 0x27
#define LCD_COL_NUM 20
#define LCD_ROW_NUM 4

#define MAXIMUM_DATA 10
class Lcd: public LiquidCrystal_I2C, public ConfigObserver
{
public:
    enum TYPE{
        TYPE_DATE =0,
        TYPE_TIME,

        TYPE_ERR,
        TYPE_REMAIN_TIME_ALARM,
        TYPE_TIMEOUT_SENSOR_4_WIRE,
        
        TYPE_UPDATE_COUNT_DOWN,
        TYPE_TIMEOUT_MEASURING,
        TYPE_POS,
        
        TYPE_ROUND,
        TYPE_COUNT_UP,
        TYPE_ANGLE,

        TYPE_PRIVIOUS_DATA_MEASURING,     

    };
    struct DataElement{
        String data;
        TYPE type;
        int length;
    };
    Lcd(uint8_t address, uint8_t col, uint8_t row);
    void hasChanged() override;
    void begin();
    static Lcd* getInstance();
    void show(String data,const TYPE& type, const uint8_t& length = 0);
    uint32_t getFreLooping();
    void run();
private:
    bool _isOnled;
    std::vector<DataElement> _listData;
    static Lcd* _lcd;
    void padLeft(String& data,const uint8_t & length);
};
#endif // DEBUG