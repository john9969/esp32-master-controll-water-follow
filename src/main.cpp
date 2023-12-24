#include "app/logic_controll.h"

void setup() {
    
  UART_DEBUG.begin(115200);
  std::shared_ptr<ReadSensor> readSensor = ReadSensor::getInstance();
  std::shared_ptr<LogicControl> logicControl = LogicControl::getInstance();
  std::shared_ptr<Lcd> lcd = Lcd::getInstance();
  Alarm * alarm = Alarm::getInstance();
  Rtc* rtc = Rtc::getRtc();
  
  Connection* connection = Connection::getInstance();
  dcomInit();
  ioInit();
  lcd->begin();
  logicControl->init();
  readSensor->init();
  connection->init();
  lcd->threadJoin();
  alarm->joinThread();
  rtc->joinThread();
  connection->threadjoin();
  readSensor->threadJoin();
}
void loop() {
}

