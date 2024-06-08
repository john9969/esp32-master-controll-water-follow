#include "readAdc.h"
float getVol(){
    return 0;
    //return (analogRead(ADC_PIN)* DataConfig::getInstance()->_multiAdc + DataConfig::getInstance()->_offsetAdc);
}