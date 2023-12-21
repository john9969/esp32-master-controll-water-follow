#include <iostream>

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
        NO_ERR = 0,
        ERR_WIFI_LOST_CONNECT,
        ERR_TIMEOUT_RUNING,
        ERR_SENSOR_FAIL,
        ERR_NON_WATER_SIGNAL
    };
    typedef enum ERR_CODE_t ErrCode;
    typedef struct Err_t Err;
    struct Err_t{
        bool has_err = false;
        ErrCode errCode = NO_ERR;
    };
    Err err;
    void setErrCode(ErrCode errCode){
        if(errCode != err.errCode){
            err.has_err = true;
            err.errCode = errCode;
        }
    }
    void setHigh(uint8_t pin);
    void setLow(uint8_t pin);
    bool getDigitalState(uint8_t pin);
    uint16_t getAnalog(uint8_t pin);
    void ioInit(){
        pinMode(SPEAKER_PIN,OUTPUT);
        pinMode(SENSOR_INPUT_PIN,INPUT);
        pinMode(LED_SIGNAL,OUTPUT);
        pinMode(LED,OUTPUT);
        pinMode(HAS_4_WIRE_SENSOR_PIN,INPUT);
        pinMode(START_BTN,INPUT);
    }
    void blink_led();
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