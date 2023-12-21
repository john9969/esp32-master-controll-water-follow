#include <Arduino.h>
#include <iostream>
#include <thread>
#include "io.h"
#define RESET_DCOM_PIN 25
static void dcomInit();
static void dcomReset();
//to reset add it in thread
void dcomReset(){
    digitalWrite(RESET_DCOM_PIN,HIGH);
    int cout =0;
    while (1)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        cout++;
        digitalWrite(LED, !digitalRead(LED));
        if(cout>100) break;
    }
    digitalWrite(LED,HIGH);
    digitalWrite(RESET_DCOM_PIN,LOW);
}
void dcomInit(){
pinMode(RESET_DCOM_PIN,OUTPUT);
digitalWrite(RESET_DCOM_PIN,LOW);
}

