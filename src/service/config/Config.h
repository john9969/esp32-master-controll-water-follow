#ifndef SERVICE_HTTPREQUEST_H
#define SERVICE_HTTPREQUEST_H
#include <Arduino.h>
#include <iostream>
#include <list>
#include <string>
#define EPSILON 0.00001f
#define IS_EQUAL(a,b) ((std::fabs(a-b) < EPSILON) ? true: false)
#define SERIAL_NUMBER "113"
#define DEFAULT_MULTI_ADC 0.008057f
#define DEFAULT_OFFSET_ADC 0.001f
#define LOWER_VOL_THRESHOLD 0.00f
#define DEFAULT_IS_ON_LED true
#define DEFAULT_DAY_TIME 6
#define DEFAULT_NIGHT_TIME 18
#define DEFAULT_MINUTE_START 55
#define DEFAULT_FRE_DAY 1
#define DEFAULT_FRE_NIGHT 1

class ConfigObserver{
public:
    virtual ~ConfigObserver(){};
    virtual void hasChanged() =0;
};
class ISubject{
public:
    virtual void notifyAll() = 0;
    virtual void append(ConfigObserver* configObserver) = 0;
    virtual void remove(ConfigObserver* ConfigObserver) = 0;
};
class DataConfig : public ISubject
{
private:
    static DataConfig * _instance;
    std::list<ConfigObserver*> _configObserverList;
public:
    std::string _sn;
    uint8_t _freDay;
    uint8_t _freNight;
    uint8_t _dayTime;
    uint8_t _nightTime;
    uint8_t _minuteStart;
    float _multiAdc;
    float _offsetAdc;
    bool _isOnLed;
    float  _lowerVolThreashold;
    void append(ConfigObserver* configObserver) override{ 
        _configObserverList.push_back(configObserver);
    }
    void remove(ConfigObserver* configObserver) override{
        auto it = std::find(this->_configObserverList.begin(), this->_configObserverList.end(), configObserver);
        if( it != this->_configObserverList.end()){
            Serial.println("earse element success");
            this->_configObserverList.erase(it);
        }
        else {
            Serial.println("could not find element from list");
        }
    }
    void notifyAll() override{
        for(auto elements: this->_configObserverList){
            elements->hasChanged();
        }
    }
    DataConfig(String serialNumber,uint8_t freDay,uint8_t freNight, uint8_t dayTime, uint8_t nightTime, uint8_t minuteStart, float multiAdc, float offsetAdc, bool isOnled, float lowerVolThreashold){
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
    static DataConfig* getInstance(String serialNumber = SERIAL_NUMBER,uint8_t freDay = DEFAULT_FRE_DAY, uint8_t freNight = DEFAULT_FRE_NIGHT, uint8_t dayTime = DEFAULT_DAY_TIME, uint8_t nightTime = DEFAULT_NIGHT_TIME, uint8_t minuteStart = DEFAULT_MINUTE_START, float multiAdc = DEFAULT_MULTI_ADC, float offsetAdc = DEFAULT_OFFSET_ADC, bool isOnLed = true, float lowerVolThreashold = LOWER_VOL_THRESHOLD){
        if(! _instance){
            _instance = new DataConfig(serialNumber, freDay,freNight,dayTime,nightTime, minuteStart,multiAdc,offsetAdc,isOnLed,lowerVolThreashold);
        }
        return _instance;
    }
    String getSerialNumber(){
        return String(this->_sn.c_str());
    }
    DataConfig* fromString(String input){
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
            Serial.println("has change config data");
            notifyAll();
        }
        return this;
    }
    DataConfig* operator=(const DataConfig& obj){
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
};
DataConfig* DataConfig::_instance = nullptr;
#endif