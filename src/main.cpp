// #include "board/rtc.h"
// #include "service/Connection.h"
// #include "service/Alarm.h"
#include "app/LogicControll.h"
//#include "app/ReadSensor.h"
Uart sensor = Uart(Uart::COM_PORT_SLAVE);
void setup() {
  UART_DEBUG.begin(115200);
  uartSlave.init();
  uartSensor.init();
 Alarm * alarm = Alarm::getInstance();
  Rtc* rtc = Rtc::getRtc();
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
