
#include <Arduino.h>
#include "io.h"

#define SLOOP 1.00

float getVol(){
    return analogRead(ADC_PIN)* SLOOP;
}