#ifndef BOARD_LCD_H
#define BOARD_LCD_H

#include <Arduino.h>
#include <iostream>
#include <memory>
#include <vector>
#include "service/config/Config.h"
#include "lib/lcd-iic/LiquidCrystal_I2C.h"
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
    Lcd(uint8_t address, uint8_t col, uint8_t row): LiquidCrystal_I2C(address, col, row), _isOnled(true){
        this->freLooping = 100;
        DataConfig::getInstance()->append(this);
    }
    void hasChanged() override {
        if (DataConfig::getInstance()->_isOnLed == this->_isOnled) return;
        if (DataConfig::getInstance()->_isOnLed)
        {
            backlight();
        }
        else {
            noBacklight();
        }
        _isOnled = DataConfig::getInstance()->_isOnLed;
    }
    void begin(){
        init();
        init();
        backlight();
    }
    static Lcd* getInstance(){
        if(!_lcd){
            _lcd = new Lcd(0x27,20,4);
        }
        return _lcd;
    }
    void show(String data,const TYPE& type, const uint8_t& length = 0);
    uint32_t getFreLooping(){ return this->freLooping;}
    void run();
private:
    uint32_t freLooping;
    bool _isOnled;
    std::vector<DataElement> listData;
    static Lcd* _lcd;
    void padLeft(String& data,const uint8_t & length);
};
#endif // DEBUG
Lcd* Lcd::_lcd = nullptr;
void Lcd::padLeft(String& data, const uint8_t& length){
    if(data.length() >= length) return;
    String append = "";
    for(uint8_t i =0; i < (length - data.length()); i++){
        append+="0";
    }
    data = append + data;
}
void Lcd::run(){

    if(this->listData.size() <= 0){
        return;
    }
    DataElement dataElement = listData.back();
    listData.pop_back();
    TYPE type = dataElement.type;
    String data = dataElement.data;
    switch (type) {
        case TYPE_DATE:
            setCursor(0,0);
            print(data);
            break;
        case TYPE_TIME:
            setCursor(12,0);
            print(data);
            break;

        case TYPE_ERR:
            setCursor(0,1);
            print("          ");
            setCursor(0,1);
            print(data);
            break;
        case TYPE_REMAIN_TIME_ALARM:
            setCursor(15,1);
            print("R:" + data);
            break;
        case TYPE_TIMEOUT_SENSOR_4_WIRE:
            setCursor(12,1);
            print("    ");
            setCursor(0,1);
            print("S:" + data);
            break;

        case TYPE_UPDATE_COUNT_DOWN:
            
            setCursor(0,1);
            print("U:" + data);
            break;
        case TYPE_TIMEOUT_MEASURING:
            break;
        case TYPE_POS:
            setCursor(0,2);
            print(data);
            break;
        case TYPE_ROUND:
            setCursor(5,2);
            print("R:"+ data);
            break;
        case TYPE_COUNT_UP:
            
            setCursor(15,2);
            print("T:"+data);
            break;
        case TYPE_ANGLE:
            setCursor(10,2);
            print("-"+data);
            break;
        case TYPE_PRIVIOUS_DATA_MEASURING:
            setCursor(0,3);
            print("                 ");
            setCursor(0,3);
            print(data);
            break;
    }

}

void Lcd::show(String data,const TYPE& type, const uint8_t & length){
    //Serial.println("Data show lcd " + data + String(type));
    if(listData.size() > MAXIMUM_DATA){
        Serial.println("lcd too much data show");
        return;
    } 
    if(length >0 )padLeft(data,length);
    DataElement dataElement = {data,type,length};
    listData.push_back(dataElement);
}   