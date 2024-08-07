#ifndef SERVICE_HTTPREQUEST__H
#define SERVICE_HTTPREQUEST__H
#include <Arduino.h>
#include <iostream>
#include <list>
#include <string>
#include <EEPROM.h>
#define DEBUG 0
#define SPEAKER_ENABLE 1
#define BTN_ENABLE 1
#define ENABLE_READ_SLAVE 1
#define SYNC_TIME_ONLINE 1
#define SYNC_BUG_ONLINE 1
#define ENABLE_OTA 1

#define ADDRESS_SERIAL_NUMBER 0
#define EEPROM_WRITE_LENGTH 10
#define WIFI_ID_1     "Thuyen2"
#define WIFI_PASS_1   "0363862128"

#define WIFI_ID_2     "Minh Tuan"
#define WIFI_PASS_2   "j12345678"
#define TIME_MAIN_WIFI_RECONNECT 2
#define TIME_OUT_MULTI_WIFI  12

#define EPSILON 0.00001f
#define IS_EQUAL(a,b) ((std::fabs(a-b) < EPSILON) ? true: false)
#define SERIAL_NUMBER "000"
#define MAXIMUM_FW_LENGTH_SESSION 1024
#define MAXIMUM_FW_INFO 20

#define DEFAULT_MULTI_ADC 0.008057f
#define DEFAULT_OFFSET_ADC 0.001f
#define LOWER_VOL_THRESHOLD 0.00f
#define DEFAULT_IS_ON_LED true
#define DEFAULT_DAY_TIME 6
#define DEFAULT_NIGHT_TIME 18
#define DEFAULT_MINUTE_START 55
#define DEFAULT_FRE_DAY 1
#define DEFAULT_FRE_NIGHT 1
/**
 * OTA
 * ----------------------------------------------------------------------------------
 */
#define VERSION "ver1.1.5"
#define TEST_NEW_FW 1
#define FRE_SYNC_OTA 10
#define DEFAULT_HEART_BEAT 100
#define TIMEOUT_CHECK_NEW_FW 3000
#if !TEST_NEW_FW 
#define API_GET_FW_IMAGE  "https://raw.githubusercontent.com/john9969/ota_server/master/firmware.bin"
#define API_GET_FW_INFO   "https://raw.githubusercontent.com/john9969/ota_server/master/fw_status.txt"
#else
#define API_GET_FW_IMAGE  "https://raw.githubusercontent.com/john9969/ota_server/dev/firmware.bin"
#define API_GET_FW_INFO   "https://raw.githubusercontent.com/john9969/ota_server/dev/fw_status.txt"
#endif

#define log(data) Serial.print(data)
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
    std::string _sn;
    uint8_t _freDay;
    uint8_t _freNight;
    uint8_t _dayTime;
    uint8_t _nightTime;
    uint8_t _minuteStart;
    float   _multiAdc;
    float   _offsetAdc;
    bool    _isOnLed;
    float   _lowerVolThreashold;
public:

    void setSn(const std::string& sn);
    void setFreDay(uint8_t freDay);
    void setFreNight(uint8_t freNight);
    void setDayTime(uint8_t dayTime);
    void setNightTime(uint8_t nightTime);
    void setMinuteStart(uint8_t minuteStart);
    void setMultiAdc(float multiAdc);
    void setOffsetAdc(float offsetAdc);
    void setIsOnLed(bool isOnLed);
    void setLowerVolThreashold(float lowerVolThreashold);

    std::string getSn() const;
    uint8_t getFreDay() const;
    uint8_t getFreNight() const;
    uint8_t getDayTime() const;
    uint8_t getNightTime() const;
    uint8_t getMinuteStart() const;
    float getMultiAdc() const;
    float getOffsetAdc() const;
    bool getIsOnLed() const;
    float getLowerVolThreashold() const;

    void append(ConfigObserver* configObserver) override;
    void remove(ConfigObserver* configObserver) override;
    void notifyAll() override;
    DataConfig(String serialNumber,uint8_t freDay,uint8_t freNight, uint8_t dayTime, uint8_t nightTime, uint8_t minuteStart, float multiAdc, float offsetAdc, bool isOnled, float lowerVolThreashold);
    static DataConfig* getInstance(String serialNumber = SERIAL_NUMBER,uint8_t freDay = DEFAULT_FRE_DAY, uint8_t freNight = DEFAULT_FRE_NIGHT, uint8_t dayTime = DEFAULT_DAY_TIME, uint8_t nightTime = DEFAULT_NIGHT_TIME, uint8_t minuteStart = DEFAULT_MINUTE_START, float multiAdc = DEFAULT_MULTI_ADC, float offsetAdc = DEFAULT_OFFSET_ADC, bool isOnLed = true, float lowerVolThreashold = LOWER_VOL_THRESHOLD);
    String getSerialNumber();
    void setSerialNumber(char* serial);
    DataConfig* fromString(String input);
    DataConfig* operator=(const DataConfig& obj);
};
#endif