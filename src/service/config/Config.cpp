#include "config.h"
DataConfig* DataConfig::_instance = nullptr;
void DataConfig::append(ConfigObserver* configObserver) { 
    _configObserverList.push_back(configObserver);
}
void DataConfig::remove(ConfigObserver* configObserver) {
    auto it = std::find(this->_configObserverList.begin(), this->_configObserverList.end(), configObserver);
    if( it != this->_configObserverList.end()){
        Serial.println("earse element success");
        this->_configObserverList.erase(it);
    }
}
void DataConfig::notifyAll() {
    for(auto elements: this->_configObserverList){
        elements->hasChanged();
    }
}
DataConfig::DataConfig(String serialNumber,uint8_t freDay,uint8_t freNight, uint8_t dayTime, uint8_t nightTime, uint8_t minuteStart, float multiAdc, float offsetAdc, bool isOnled, float lowerVolThreashold){
    this->_sn = std::string(serialNumber.c_str());
    this->_freDay = freDay;
    this->_freNight = freNight;
    this->_dayTime = dayTime;
    this->_nightTime = nightTime;
    this->_minuteStart = minuteStart;
    this->_multiAdc = multiAdc;
    this->_offsetAdc = offsetAdc;
    this->_isOnLed = isOnled;
    this->_lowerVolThreashold = lowerVolThreashold;
}
DataConfig* DataConfig::getInstance(String serialNumber,uint8_t freDay, uint8_t freNight, uint8_t dayTime, uint8_t nightTime, uint8_t minuteStart, float multiAdc, float offsetAdc, bool isOnLed, float lowerVolThreashold){
    if(! _instance){
        _instance = new DataConfig(serialNumber, freDay,freNight,dayTime,nightTime, minuteStart,multiAdc,offsetAdc,isOnLed,lowerVolThreashold);
    }
    return _instance;
}
String DataConfig::getSerialNumber(){
    return String(this->_sn.c_str());
}
DataConfig* DataConfig::fromString(String input){
    std::string data = std::string(input.c_str());
    size_t pos = 0;
    bool hasChange = false;
    while (pos < data.size()) {
        if(data.back() == '}') data.pop_back();
        size_t keyStart = data.find('"', pos);
        if (keyStart == std::string::npos) break;
        size_t keyEnd = data.find('"', keyStart + 1);
        if (keyEnd == std::string::npos) break;
        std::string key = data.substr(keyStart + 1, keyEnd - keyStart - 1);
        key.erase(0, key.find_first_not_of(" \t\n\r\f\v\""));
        key.erase(key.find_last_not_of(" \t\n\r\f\v\"") + 1);
        size_t valueStart = data.find(':', keyEnd);
        if (valueStart == std::string::npos) break;
        size_t valueEnd = data.find(',', valueStart);
        if (valueEnd == std::string::npos) valueEnd = data.size();
        std::string value = data.substr(valueStart + 1, valueEnd - valueStart - 1);
        value.erase(0, value.find_first_not_of(" \t\n\r\f\v\""));
        value.erase(value.find_last_not_of(" \t\n\r\f\v\"") + 1);
        if(key == "frequency_day"){
            uint8_t freDay =  std::stoi(value); 
            if(this->_freDay != freDay){
                this->_freDay = freDay;
                hasChange = true;
            }
        }
        else if(key == "frequency_night"){
            uint8_t freNight =  std::stoi(value); 
            if(this->_freNight != freNight){
                this->_freNight = freNight;
                hasChange = true;
            }
        }
        else if(key == "minute_start"){
            uint8_t minuteStart = std::stoi(value);
            if(this->_minuteStart != minuteStart){
                this->_minuteStart = minuteStart;
                hasChange = true;
            }
        }
        else if (key == "day_time")
        {
            uint8_t dayTime  = std::stoi(value);
            if(this->_dayTime != dayTime){
                this->_dayTime = dayTime;
                hasChange = true;
            }
        }
        else if (key == "night_time"){
            uint8_t nightTime = std::stoi(value);
            if(this->_nightTime != nightTime){
                this->_nightTime = nightTime;
                hasChange = true;
            }
        }
        else if(key == "multi_adc"){
            float multiAdc = std::stof(value);
            if(!IS_EQUAL(this->_multiAdc,multiAdc)){
                this->_multiAdc = multiAdc;
                hasChange = true;
            }
        }
        else if( key == "offset_adc"){
            float offsetAdc = std::stof(value);
            if(!IS_EQUAL(this->_offsetAdc, offsetAdc)){
                this->_offsetAdc = offsetAdc;
                hasChange = true;
            }
            
        }
        else if( key == "is_on_led"){
            bool isOnLed;
            if(value == "true" || value == "1")
            isOnLed = true;
            else if(value == "false" || value == "0")
            isOnLed = false;
            if(this->_isOnLed != isOnLed){
                this->_isOnLed = isOnLed;
                hasChange = true;
            }
        }
        else if( key == "low_vol_threashold"){
            float lowerVolThreashold = std::stof(value);
            if(!IS_EQUAL(this->_lowerVolThreashold,lowerVolThreashold)){
                this->_lowerVolThreashold = lowerVolThreashold;
                hasChange = true;
            }
        }
        pos = valueEnd + 1;
    }
    if(hasChange){
        notifyAll();
    }
    return this;
}
DataConfig* DataConfig::operator=(const DataConfig& obj){
    this->_sn = obj._sn;
    this->_freDay = obj._freDay;
    this->_freNight = obj._freNight;
    this->_dayTime = obj._dayTime;
    this->_nightTime = obj._nightTime;
    this->_minuteStart = obj._minuteStart;
    this->_multiAdc = obj._multiAdc;
    this->_offsetAdc = obj._offsetAdc;
    this->_isOnLed = obj._isOnLed;
    this->_lowerVolThreashold = obj._lowerVolThreashold;
    return this;
};

void DataConfig::setSn(const std::string& sn) {
    _sn = sn;
}

void DataConfig::setFreDay(uint8_t freDay) {
    _freDay = freDay;
}

void DataConfig::setFreNight(uint8_t freNight) {
    _freNight = freNight;
}

void DataConfig::setDayTime(uint8_t dayTime) {
    _dayTime = dayTime;
}

void DataConfig::setNightTime(uint8_t nightTime) {
    _nightTime = nightTime;
}

void DataConfig::setMinuteStart(uint8_t minuteStart) {
    _minuteStart = minuteStart;
}

void DataConfig::setMultiAdc(float multiAdc) {
    _multiAdc = multiAdc;
}

void DataConfig::setOffsetAdc(float offsetAdc) {
    _offsetAdc = offsetAdc;
}

void DataConfig::setIsOnLed(bool isOnLed) {
    _isOnLed = isOnLed;
}

void DataConfig::setLowerVolThreashold(float lowerVolThreashold) {
    _lowerVolThreashold = lowerVolThreashold;
}
std::string DataConfig::getSn() const {
    return _sn;
}

uint8_t DataConfig::getFreDay() const {
    return _freDay;
}

uint8_t DataConfig::getFreNight() const {
    return _freNight;
}

uint8_t DataConfig::getDayTime() const {
    return _dayTime;
}

uint8_t DataConfig::getNightTime() const {
    return _nightTime;
}

uint8_t DataConfig::getMinuteStart() const {
    return _minuteStart;
}

float DataConfig::getMultiAdc() const {
    return _multiAdc;
}

float DataConfig::getOffsetAdc() const {
    return _offsetAdc;
}

bool DataConfig::getIsOnLed() const {
    return _isOnLed;
}

float DataConfig::getLowerVolThreashold() const {
    return _lowerVolThreashold;
}
