#ifndef DCOM__H
#define DCOM__H

#include <Arduino.h>
#include <iostream>
#include "../io/Io.h"
#define RESET_DCOM_PIN 25
#define TIMEOUT_BILNK_100

typedef enum Dcom_State{
    Dcom_State_ON = 0,
    Dcom_State_INIT_RESET,
    Dcom_State_RESET,
    Dcom_State_DONE_RESET
}Dcom_State;
extern Dcom_State dcomState;
extern uint64_t timeoutReset;
void dcomReset();
void dcomInit();
#endif // DCOM__H
