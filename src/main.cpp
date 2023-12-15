#include <Arduino.h>
#include "board/uart.h"
Uart sensor = Uart(Uart::COM_PORT_SLAVE);
// put function declarations here:
int myFunction(int, int);

void setup() {
  pinMode(LED_BUILTIN,OUTPUT);
  UART_DEBUG.begin(115200);
  // put your setup code here, to run once:
  sensor.init();
  //UART_SLAVE.begin(115200);
}
int i=0;
void loop() {
  i++;
  if(i!= 5)sensor.send("A3)");
  
  if (i == 5)
  {
    sensor.send(":RESET)");
    i=0;
    /* code */
  }
  
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  delay(2000);
  // put your main code here, to run repeatedly:
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}