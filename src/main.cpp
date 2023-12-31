#include "app/SyncData.h"
#include "lib/thread/Thread.h"
#include "lib/thread/ThreadController.h"

void callback_logic_control();
void callback_read_sensor();
void callback_sync_sensor();
void callback_uart();
void callback_io();
void callback_lcd();
void callback_rtc();

void callback_alarm();
void callback_connection();
void callback_http();
void callback_sync_data();
ThreadController controller = ThreadController();
LogicControl* logicControl = LogicControl::getInstance();
ReadSensor* readSensor = ReadSensor::getInstance();

Lcd* lcd = Lcd::getInstance();
Rtc* rtc = Rtc::getInstance();

SyncData syncData;

Alarm* ringAlarm = Alarm::getInstance();
Connection * connection = Connection::getInstance();

Thread threadLogicControl = Thread();
Thread threadReadSensor = Thread();
Thread threadSyncServer = Thread();

Thread threadIO = Thread();
Thread threadLcd = Thread();
Thread threadRtc = Thread();

Thread threadAlarm = Thread();
Thread threadConnection = Thread();
Thread threadHttpRequest = Thread();
Thread threadSyncData = Thread();
Thread threadUart = Thread();
void setup() {
  delay(2000);
  threadConnection.onRun(&callback_connection);
  threadConnection.setInterval(1000);
  threadIO.onRun(&callback_io);
  threadIO.setInterval(100);
  threadLcd.onRun(&callback_lcd);
  threadLcd.setInterval(100);
  threadUart.onRun(&callback_uart);
  threadUart.setInterval(5);
  threadRtc.onRun(&callback_rtc);
  threadRtc.setInterval(100);
  threadAlarm.onRun(&callback_alarm);
  threadAlarm.setInterval(100);
  threadReadSensor.onRun(&callback_read_sensor);
  threadReadSensor.setInterval(10);
  threadSyncData.onRun(&callback_sync_data);
  threadSyncData.setInterval(5000);
  threadLogicControl.onRun(&callback_logic_control);
  threadLogicControl.setInterval(10);
  dcomInit();
  ioInit();
  lcd->begin();
  Serial.begin(115200);
  connection->init();
  logicControl->init();
  readSensor->init();
  uartSlave.begin();
  // pinMode(9,INPUT_PULLUP);
  // pinMode(10,OUTPUT);
  // Serial1.begin(115200);
  controller.add(&threadUart);
  controller.add(&threadRtc);
  controller.add(&threadLcd);
  controller.add(&threadConnection);
  
  controller.add(&threadAlarm);
  controller.add(&threadIO);
  controller.add(&threadReadSensor);
  controller.add(&threadSyncData);
  controller.add(&threadLogicControl);
}
void loop() {
  controller.run();
}


void callback_logic_control(){
  logicControl->run();
}
void callback_read_sensor(){
  readSensor->process4wire();
  readSensor->process3Wire();
}
void callback_sync_sensor(){

}

void callback_io(){
  checkBtn();
  speakerLoop();
}

void callback_lcd(){
  lcd->run();
}
void callback_rtc(){
  rtc->run();
}

void callback_uart(){
  String data = "";

  if(Serial.available()){
    String data = Serial.readString();
    if(data.startsWith("start")){
      hasStartBtn =true;
      Serial.println("start ok");
    }
    else if(data.startsWith("reset")){
      ESP.restart();
    }
    else if(data.startsWith("slave")){
      // uartSlave.send("A");
    }
  }
}

void callback_alarm(){
  ringAlarm->runAlarm();
}
void callback_connection(){
  connection->checkingConnection();
}

void callback_sync_data(){
  syncData.syncErr();
  syncData.syncTime();
}
