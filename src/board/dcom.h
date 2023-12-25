#include <Arduino.h>
#include <iostream>
#include "io.h"
#define RESET_DCOM_PIN 25
#define TIMEOUT_BILNK_100
static void dcomInit();
static void dcomReset();
uint64_t timeoutReset =0;
enum Dcom_State{
    Dcom_State_ON = 0,
    Dcom_State_INIT_RESET,
    Dcom_State_RESET,
    Dcom_State_DONE_RESET
};
Dcom_State dcomState = Dcom_State_ON;
void dcomReset(){
    switch (dcomState){
        case Dcom_State_DONE_RESET:
        case Dcom_State_ON:
        break;
        case Dcom_State_INIT_RESET:
            setHigh(RESET_DCOM_PIN);
            timeoutReset = millis();
            setErrCode(ErrCode::ERR_WIFI_LOST_CONNECT);
            dcomState = Dcom_State_RESET;
            break;
        case Dcom_State_RESET:
            if((millis()-timeoutReset) < 10000){
                blink_led();
            }
            else{
                setLow(RESET_DCOM_PIN);
                setLow(LED);
                dcomState = Dcom_State_DONE_RESET;
            }
            break;
    }
}

void dcomInit(){
    pinMode(RESET_DCOM_PIN,OUTPUT);
    digitalWrite(RESET_DCOM_PIN,LOW);
}

