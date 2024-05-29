#ifndef BOARD_IO_H
#define BOARD_IO_H
#include <iostream>
#include <vector>
#include <Arduino.h>
#define LED                     LED_BUILTIN
#define LED_SIGNAL              33
#define SENSOR_INPUT_PIN        39
#define START_BTN               35
#define HAS_4_WIRE_SENSOR_PIN   34
#define SPEAKER_PIN             27
#define ADC_PIN                 36
extern "C"{
    #include <stdbool.h>
    enum ERR_CODE_t{
        
        ERR_WIFI_LOST_CONNECT=0,
        ERR_TIMEOUT_RUNING = 1,
        ERR_SENSOR_FAIL = 2,
        ERR_NON_WATER_SIGNAL = 3,
        ERR_GET_TIME_FAIL = 4,
        ERR_GET_DATACONFIG_FAIL =5,
        ERR_MOVING_FROM_BOT_TO_DAY =6,
        ERR_MOVING_FROM_DAY_TO_08,
        ERR_MOVING_FROM_DAY_TO_06,
        ERR_MOVING_FROM_08_TO_06,
        ERR_MOVING_FROM_06_TO_04,
        ERR_MOVING_FROM_04_TO_02,
        ERR_MOVING_FROM_02_TO_MAT,
        ERR_MOVING_FROM_MAT_TO_RES,
        ERR_NON_RESPONSE_AT_STEP_TO_BOT=6,
        ERR_NON_RESPONSE_AT_STEP_TO_08=7,
        ERR_NON_RESPONSE_AT_STEP_TO_06=8,
        ERR_NON_RESPONSE_AT_STEP_TO_04=9,
        ERR_NON_RESPONSE_AT_STEP_TO_02=10,
        ERR_NON_RESPONSE_AT_STEP_TO_MAT=11,
        ERR_NON_RESPONSE_AT_STEP_TO_RES=12,
        ERR_MOVING_FROM_RES_TO_BOTTOM = 14,
        ERR_TIMEOUT_AT_BOT= 15,
        ERR_TIMEOUT_AT_08 =16,
        ERR_TIMEOUT_AT_06 =17,
        ERR_TIMEOUT_AT_04 =18,
        ERR_TIMEOUT_AT_02 = 19,
        ERR_TIMEOUT_AT_MAT =20,
        ERR_TIMEOUT_TOTAL = 13,
        ERR_NUM_OF_LIST_DATA_OUT_OF_RANGE,
        NO_ERR

    };
    typedef struct Speaker{
        uint16_t timeOnspeaker = 0;
        uint64_t _timeRetainSpeaker = 0;
        bool isSpeakerActive = false;
        bool isEnable = false;
    } Speaker;
    Speaker speaker;
    typedef enum ERR_CODE_t ErrCode;
    static bool signalSensor3Wire;
    static bool hasStartBtn = false;
    static bool hasErrChanged = true;
    
    static std::vector<ErrCode> err;
    static int getErrCode(std::vector<ErrCode>& _errCode);
    static void setErrCode(ErrCode _errCode);
    static void removeErrCode(ErrCode _errCode);
    static void IRAM_ATTR callback3wire();
    static void ioInit();
    static void blink_led();
    static void setHigh(uint8_t pin);
    static void setLow(uint8_t pin);
    static bool getDigitalState(uint8_t pin);
    static uint16_t getAnalog(uint8_t pin);

    static void setOnSpeaker(uint16_t timeOn);
    static void speakerLoop();
    void checkBtn(){
        #if 0
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
            //Serial.println("btn release");
            oldState = currentState;
        }
        #endif
    }
    int getErrCode(std::vector<ErrCode>& _errCode){
        _errCode = err;
        return _errCode.size();
    }
    void setErrCode(ErrCode _errCode){
        int length = err.size();
        if(length <= 0){
            err.push_back(_errCode);
            hasErrChanged =true;
        }
        else{
            for(int i = 0; i <length; i++){
                if(err.at(i) == _errCode) return;
            }
            hasErrChanged = true;
            err.push_back(_errCode);
        }
    }
    void removeErrCode(ErrCode _errCode){
        int length = err.size();
        if(length <=0) return;
        for(int i = 0; i< length; i++){
            if(err.at(i) == _errCode){
                err.erase(err.begin()+i);
                hasErrChanged = true;
                break;
            }
        }
    }
    bool oldStateSensor = false;
    inline void callback3wire(){
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
    void ioInit(){
        pinMode(SPEAKER_PIN,OUTPUT);
        pinMode(SENSOR_INPUT_PIN,INPUT);
        pinMode(LED_SIGNAL,OUTPUT);
        pinMode(LED,OUTPUT);
        pinMode(HAS_4_WIRE_SENSOR_PIN,INPUT);
        pinMode(START_BTN,INPUT);
        signalSensor3Wire = false;
    }
    void blink_led(){
        digitalWrite(LED,!digitalRead(LED));
    }

    void setHigh(uint8_t pin){
        digitalWrite(pin,HIGH);
    }
    void setLow(uint8_t pin){
        digitalWrite(pin,LOW);
    }
    bool getDigitalState(uint8_t pin){
        return digitalRead(pin);
    } 
    uint16_t getAnalog(uint8_t pin){
        return analogRead(pin);
    }
    void setOnSpeaker(uint16_t timeOn){
        speaker.timeOnspeaker = timeOn;
        speaker._timeRetainSpeaker= millis();
        speaker.isEnable = true;
        // Serial.println("time ON" + String(speaker.timeOnspeaker));
        // Serial.println("time retain" + String(speaker._timeRetainSpeaker));
        
    }
    void speakerLoop(){
        if(!speaker.isEnable) return;
        // Serial.println("mili: " + String(millis()));
        // Serial.println("retain: " +String(speaker._timeRetainSpeaker));
        // Serial.println("timeOn: " + String(speaker.timeOnspeaker));
        if((millis() - speaker._timeRetainSpeaker) < speaker.timeOnspeaker){
            if(!speaker.isSpeakerActive) {
                // Serial.println("speaker enable");
                #if 0
                setHigh(SPEAKER_PIN);
                #endif
                speaker.isSpeakerActive = true;
            }
        }
        else {
            // Serial.println("speaker off");
            speaker.isEnable = false;
            speaker.isSpeakerActive = false;
            setLow(SPEAKER_PIN);
        }
    }
}
#endif