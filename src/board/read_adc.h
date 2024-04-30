
#include <Arduino.h>
#include "io.h"
#include "service/config/Config.h"
float getVol(){
    return (analogRead(ADC_PIN)* DataConfig::getInstance()->_offsetAdc + DataConfig::getInstance()->_offsetAdc);
}