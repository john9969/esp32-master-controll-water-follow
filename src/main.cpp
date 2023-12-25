#include "app/logic_controll.h"
#include "lib/thread/Thread.h"
#include "lib/thread/ThreadController.h"
void callback_logic_control();
void callback_read_sensor();
void callback_sync_sensor();

void callback_dcom();
void callback_lcd();
void callback_threadrtc();
void callback_uart();

void callback_alarm();
void callback_connection();
void callback_http();
ThreadController controller = ThreadController();
LogicControl* logicControl = LogicControl::getInstance();
ReadSensor* readSensor = ReadSensor::getInstance();
Lcd* lcd = Lcd::getInstance();
Rtc* rtc = Rtc::getInstance();

Alarm* ringAlarm = Alarm::getInstance();
Connection * connection = Connection::getInstance();
HttpRequest* httpRequest = HttpRequest::getInstance();

Thread threadLogicControl = Thread();
Thread threadReadSensor = Thread();
Thread threadSyncServer = Thread();

Thread threadDcom = Thread();
Thread threadLcd = Thread();
Thread threadRtc = Thread();
Thread threadUart = Thread();

Thread threadAlarm = Thread();
Thread threadConnection = Thread();
Thread threadHttpRequest = Thread();
void setup() {
  UART_DEBUG.begin(115200);
  threadConnection.onRun(&callback_connection);
  threadConnection.setInterval(1000);
  threadLcd.onRun(&callback_lcd);
  threadLcd.setInterval(100);

  dcomInit();
  ioInit();
  lcd->begin();
  connection->init();
  logicControl->init();
  readSensor->init();

  controller.add(&threadLcd);
  controller.add(&threadConnection);

}
void loop() {
  controller.run();
}


void callback_logic_control(){

}
void callback_read_sensor(){

}
void callback_sync_sensor(){

}

void callback_dcom(){

}
void callback_lcd(){
  lcd->run();
}
void callback_threadrtc(){
  rtc->run();
}
void callback_uart(){
  readSensor->process4wire();

}

void callback_alarm(){
  ringAlarm->runAlarm();
}
void callback_connection(){
  connection->checkingConnection();
}
void callback_http(){

}
