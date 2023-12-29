#ifndef BOARD_LCD_H
#define BOARD_LCD_H

#include <Arduino.h>
#include <iostream>
#include <memory>
#include <vector>
#include "lib/lcd-iic/LiquidCrystal_I2C.h"
#define MAXIMUM_DATA 10
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
    struct DataElement{
        String data;
        TYPE type;
        int length;
    };
    Lcd(uint8_t address, uint8_t col, uint8_t row): LiquidCrystal_I2C(address, col, row){
        this->freLooping = 100;
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
            print("        ");
            setCursor(0,0);
            print(data);
            break;
        case TYPE_TIME:
            setCursor(12,0);
            print("        ");
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
            print("    ");
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
            setCursor(12,1);
            print("    ");
            setCursor(0,1);
            print("U:" + data);
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
            setCursor(16,2);
            print("     ");
            setCursor(16,2);
            print("T:"+data);
            break;
        case TYPE_ANGLE:
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