
#include <iostream>
#include <vector>
#include <Arduino.h>
#include "../../service/config/Config.h"
#ifndef IO__H
#define IO__H
#define LED                     LED_BUILTIN
#define LED_SIGNAL              33
#define SENSOR_INPUT_PIN        39
#define START_BTN               35
#define HAS_4_WIRE_SENSOR_PIN   34
#define SPEAKER_PIN             27
#define ADC_PIN                 36
#ifdef __cplusplus
extern "C" {
#endif
    #include <stdbool.h>
    
    enum ERR_CODE_t{
        ERR_WIFI_LOST_CONNECT       = 0,
        ERR_TIMEOUT_RUNING          = 1,
        ERR_SENSOR_FAIL             = 2,
        ERR_NON_WATER_SIGNAL        = 3,
        ERR_GET_TIME_FAIL           = 4,
        ERR_GET_DATACONFIG_FAIL     = 5,
        ERR_MOVING_FROM_BOT_TO_DAY  = 6,
        ERR_MOVING_FROM_DAY_TO_08   = 7,
        ERR_MOVING_FROM_DAY_TO_06   = 8,
        ERR_MOVING_FROM_08_TO_06    = 9,
        ERR_MOVING_FROM_06_TO_04    =10 ,
        ERR_MOVING_FROM_04_TO_02    =11,   
        ERR_MOVING_FROM_02_TO_MAT   =12,
        ERR_MOVING_FROM_MAT_TO_RES  =13,
        ERR_NON_RESPONSE_AT_STEP_TO_BOT =14,
        ERR_NON_RESPONSE_AT_STEP_TO_08  =15,
        ERR_NON_RESPONSE_AT_STEP_TO_06  =16,
        ERR_NON_RESPONSE_AT_STEP_TO_04  =17,
        ERR_NON_RESPONSE_AT_STEP_TO_02  =18,
        ERR_NON_RESPONSE_AT_STEP_TO_MAT =19,
        ERR_NON_RESPONSE_AT_STEP_TO_RES =20,
        ERR_MOVING_FROM_RES_TO_BOTTOM   =21,
        ERR_TIMEOUT_AT_BOT              =22,
        ERR_TIMEOUT_AT_08               =23,
        ERR_TIMEOUT_AT_06               =24,
        ERR_TIMEOUT_AT_04               =25,
        ERR_TIMEOUT_AT_02               =26,
        ERR_TIMEOUT_AT_MAT              =27,
        ERR_TIMEOUT_TOTAL               =28,
        ERR_NUM_OF_LIST_DATA_OUT_OF_RANGE = 29,
        NO_ERR
    };
    typedef enum ERR_CODE_t ErrCode;
    typedef struct Speaker_t Speaker;
    struct Speaker_t{
        uint16_t    _timeOnspeaker;
        uint64_t    _timeRetainSpeaker;
        bool        _isSpeakerActive;
        bool        _isEnable;
    };
    extern Speaker speaker;
    extern bool signalSensor3Wire;
    extern bool hasStartBtn;
    extern bool hasErrChanged;
    extern std::vector<ErrCode> err;
    
    int getErrCode(std::vector<ErrCode>& errCode);
    void setErrCode(ErrCode errCode);
    void removeErrCode(ErrCode errCode);
    void setOnSpeaker(uint16_t timeOn);
    void ioInit();
    void blink_led();
    void setHigh(uint8_t pin);
    void setLow(uint8_t pin);
    bool getDigitalState(uint8_t pin);
    uint16_t getAnalog(uint8_t pin);

    inline void speakerLoop(){
        if(!speaker._isEnable) return;
        if((millis() - speaker._timeRetainSpeaker) < speaker._timeOnspeaker){
            if(!speaker._isSpeakerActive) {
#if SPEAKER_ENABLE
                setHigh(SPEAKER_PIN);
#endif
                speaker._isSpeakerActive = true;
            }
        }
        else {
            speaker._isEnable = false;
            speaker._isSpeakerActive = false;
#if SPEAKER_ENABLE
            setLow(SPEAKER_PIN);
#endif
        }
    }
    inline void callback3wire(){
        static bool oldStateSensor = false;
        bool currentState = digitalRead(SENSOR_INPUT_PIN);
        if(currentState){
            if(oldStateSensor != currentState){
                oldStateSensor = currentState;
                return;
            }
        }
        else{
            if(oldStateSensor != currentState){
                oldStateSensor = currentState;
                if(!signalSensor3Wire){
                    signalSensor3Wire =true;
                }
                return;
            }
        }
    }
    inline void checkBtn(){
#if BTN_ENABLE
        static bool oldState = true;
        bool currentState = getDigitalState(START_BTN);
        if(!currentState){
            if(oldState != currentState){
                oldState = currentState;
                return;
            }
            Serial.println("btn press");
            hasStartBtn = true;
        }
        else{
            if(oldState == currentState) return;
            oldState = currentState;
        }
#endif
    }


#ifdef __cplusplus
}
#endif
#endif