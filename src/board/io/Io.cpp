#include "Io.h"
Speaker speaker = {0};
bool signalSensor3Wire  = false;
bool hasStartBtn        = false;
bool hasErrChanged      = true;
std::vector<ErrCode> err;

int getErrCode(std::vector<ErrCode>& errCode){
    errCode = err;
    return errCode.size();
}
void setErrCode(ErrCode errCode){
    int length = err.size();
    if(length <= 0){
        err.push_back(errCode);
        hasErrChanged =true;
    }
    else{
        for(int i = 0; i <length; i++){
            if(err.at(i) == errCode) return;
        }
        hasErrChanged = true;
        err.push_back(errCode);
    }
}
void removeErrCode(ErrCode errCode){
    int length = err.size();
    if(length <=0) return;
    for(int i = 0; i< length; i++){
        if(err.at(i) == errCode){
            err.erase(err.begin()+i);
            hasErrChanged = true;
            break;
        }
    }
}

void ioInit(){
    pinMode(SPEAKER_PIN,            OUTPUT);
    pinMode(SENSOR_INPUT_PIN,       INPUT);
    pinMode(LED_SIGNAL,             OUTPUT);
    pinMode(LED,                    OUTPUT);
    pinMode(HAS_4_WIRE_SENSOR_PIN,  INPUT);
    pinMode(START_BTN,              INPUT);
}
void blink_led(){
    digitalWrite(LED,               !digitalRead(LED));
}

void setHigh(uint8_t pin){
    digitalWrite(pin,   HIGH);
}
void setLow(uint8_t pin){
    digitalWrite(pin,   LOW);
}
bool getDigitalState(uint8_t pin){
    return digitalRead(pin);
} 
uint16_t getAnalog(uint8_t pin){
    return analogRead(pin);
}
void setOnSpeaker(uint16_t timeOn){
    speaker._timeOnspeaker      = timeOn;
    speaker._timeRetainSpeaker  = millis();
    speaker._isEnable           = true;
}
