#include <Arduino.h>
#include <iostream>
#include <memory>
#include "lib/lcd-iic/LiquidCrystal_I2C.h"

class Lcd: public LiquidCrystal_I2C
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
        TYPE_ANGLE      
    };
    Lcd(uint8_t address, uint8_t col, uint8_t row): LiquidCrystal_I2C(address, col, row){
    }
    void begin(){
        init();
        init();
        backlight();
    }
    static std::shared_ptr<Lcd> getInstance(){
        if(!lcd){
            lcd = std::shared_ptr<Lcd>(new Lcd(0x27,20,4));
        }
        return lcd;
    }
    void show(const String& data,const TYPE& type);
private:
    static std::shared_ptr<Lcd> lcd;
};

std::shared_ptr<Lcd> Lcd::lcd = nullptr;
void Lcd::show(const String& data,const TYPE& type){
    switch (type) {
        case TYPE_DATE:
            setCursor(0,0);
            print("        ");
            setCursor(0,0);
            break;
        case TYPE_TIME:
            setCursor(12,0);
            print("        ");
            setCursor(12,0);
            break;

        case TYPE_ERR:
            setCursor(0,1);
            print("    ");
            setCursor(0,1);
            print("E:" + data);
            break;
        case TYPE_REMAIN_TIME_ALARM:
            setCursor(6,1);
            print("    ");
            setCursor(5,1);
            print("R:" + data);
            break;
        case TYPE_TIMEOUT_SENSOR_4_WIRE:
            setCursor(12,1);
            print("    ");
            setCursor(0,1);
            print("S:" + data);
            break;

        case TYPE_UPDATE_COUNT_DOWN:
            setCursor(12,1);
            print("    ");
            setCursor(0,1);
            print("U:" + data);
            break;
            break;
        case TYPE_TIMEOUT_MEASURING:
            break;
        case TYPE_POS:
            setCursor(3,1);
            print("     ");
            setCursor(3,1);
            break;

        case TYPE_ROUND:
            setCursor(9,1);
            print("      ");
            setCursor(9,1);
            break;
        case TYPE_COUNT_UP:
            setCursor(16,1);
            print("     ");
            setCursor(16,1);
            break;
        case TYPE_ANGLE:
            break;
    }
}   