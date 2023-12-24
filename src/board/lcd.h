#include <Arduino.h>
#include <iostream>
#include <memory>
#include <mutex>
#include <variant>
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
        thread = std::thread(&Lcd::run,this);
    }
    void threadJoin(){
        if(thread.joinable()) thread.join();
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
    void show(String data,const TYPE& type, const uint8_t& length = 0);
private:
    void run();
    std::mutex mtx;
    std::thread thread;
    std::vector<DataElement> listData;
    static std::shared_ptr<Lcd> lcd;
    void padLeft(String& data,const uint8_t & length);
};

std::shared_ptr<Lcd> Lcd::lcd = nullptr;
void Lcd::padLeft(String& data, const uint8_t& length){
    if(data.length() >= length) return;
    String append = "";
    for(uint8_t i =0; i < (length - data.length()); i++){
        append+="0";
    }
    data = append + data;
}
void Lcd::run(){
    while (1)
    {
    if(this->listData.size() <=0){
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        continue;
    } 
    std::unique_lock<std::mutex> lock(this->mtx);
    DataElement dataElement = listData.back();
    listData.pop_back();
    lock.unlock();
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
            print("    ");
            setCursor(0,1);
            print("E:" + data);
            break;
        case TYPE_REMAIN_TIME_ALARM:
            setCursor(6,1);
            print("    ");
            setCursor(5,1);
            if(dataElement.length >0){
                padLeft(data,dataElement.length);
            }
            else{
                padLeft(data,3);
            }
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
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
void Lcd::show(String data,const TYPE& type, const uint8_t & length){
    Serial.println(data + String(type));
    if(listData.size() > MAXIMUM_DATA){
        Serial.println("lcd too much data show");
        return;
    } 
    std::lock_guard<std::mutex> lock (this->mtx);
    DataElement dataElement = {data,type,length};
    listData.push_back(dataElement);
}   