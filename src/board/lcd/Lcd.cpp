
#include "Lcd.h"
void Lcd::hasChanged() {
    if (DataConfig::getInstance()->getIsOnLed() == this->_isOnled) return;
    if (DataConfig::getInstance()->getIsOnLed()) backlight();
    else noBacklight();
    _isOnled = DataConfig::getInstance()->getIsOnLed();
}
Lcd::Lcd(uint8_t address, uint8_t col, uint8_t row): LiquidCrystal_I2C(address, col, row), _isOnled(true){
    DataConfig::getInstance()->append(this);
}
void Lcd::begin(){
    init();
    init();
    backlight();
}
Lcd* Lcd::getInstance(){
    if(!_lcd){
        _lcd = new Lcd(LCD_ADDRESS,LCD_COL_NUM,LCD_ROW_NUM);
    }
    return _lcd;
}

void Lcd::padLeft(String& data, const uint8_t& length){
    if(data.length() >= length) return;
    String append = "";
    for(uint8_t i =0; i < (length - data.length()); i++){
        append+="0";
    }
    data = append + data;
}
void Lcd::run(){

    if(this->_listData.size() <= 0){
        return;
    }
    DataElement dataElement = _listData.back();
    _listData.pop_back();
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
    if(_listData.size() > MAXIMUM_DATA){
        Serial.println("lcd too much data to show");
        return;
    } 
    if(length >0 )padLeft(data,length);
    DataElement dataElement = {data,type,length};
    _listData.push_back(dataElement);
}   
Lcd* Lcd::_lcd = nullptr;