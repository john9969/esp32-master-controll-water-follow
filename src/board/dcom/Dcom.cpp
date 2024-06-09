#include "Dcom.h"
Dcom_State dcomState = Dcom_State_ON;
uint64_t timeoutReset =0;
void dcomReset(){
    switch (dcomState){
        case Dcom_State_DONE_RESET:
        case Dcom_State_ON:
        break;
        case Dcom_State_INIT_RESET:
            setHigh(RESET_DCOM_PIN);
            timeoutReset = millis();
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

