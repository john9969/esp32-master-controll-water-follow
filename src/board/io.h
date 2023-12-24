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

extern "C"{
    #include <stdbool.h>
    enum ERR_CODE_t{
        ERR_WIFI_LOST_CONNECT=0,
        ERR_TIMEOUT_RUNING,
        ERR_SENSOR_FAIL,
        ERR_NON_WATER_SIGNAL
    };
    typedef enum ERR_CODE_t ErrCode;
    static bool signalSensor3Wire;
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
    int getErrCode(std::vector<ErrCode>& _errCode){
        _errCode = err;
        return _errCode.size();
    }
    void setErrCode(ErrCode _errCode){
        int length = err.size();
        if(length <= 0){
            err.push_back(_errCode);
        }
        else{
            for(int i = 0; i <length; i++){
                if(err.at(i) == _errCode) return;
            }
            err.push_back(_errCode);
        }
    }
    void removeErrCode(ErrCode _errCode){
        int length = err.size();
        if(length <=0) return;
        for(int i = 0; i< length; i++){
            if(err.at(i) == _errCode){
                err.erase(err.begin()+i);
                break;
            }
        }
    }
    void IRAM_ATTR callback3wire(){
        if(!signalSensor3Wire)
            signalSensor3Wire =true;
    }
    void ioInit(){
        pinMode(SPEAKER_PIN,OUTPUT);
        attachInterrupt(SENSOR_INPUT_PIN,&callback3wire,RISING);
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
}
#endif