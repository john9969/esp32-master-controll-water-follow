#include <Arduino.h>
#include <Thread>
#include "board/uart.h"
#include "board/rtc.h"
#include "service/Connection.h"
#include "service/Alarm.h"
#include "service/HttpRequest.h"
#include <thread>
Uart sensor = Uart(Uart::COM_PORT_SLAVE);
void setup() {
  UART_DEBUG.begin(115200);
  Alarm * alarm = Alarm::getInstance();

  Rtc* rtc = Rtc::getRtc();
  HttpRequest* httpRequest = HttpRequest::getInstance();
  Connection* connection = Connection::getInstance();
  dcomInit();
  ioInit();
  sensor.init();
  connection->init();
  alarm->joinThread();
  rtc->joinThread();
  connection->threadjoin();
}
void loop() {
}
