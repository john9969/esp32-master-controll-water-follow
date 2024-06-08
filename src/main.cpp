#include "app/syncData/SyncData.h"
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
void callback_resetAlarm();
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
Thread *threadResetAlarm = new Thread();
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
  threadResetAlarm->setInterval(3000);
  threadResetAlarm->onRun(&callback_resetAlarm);
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

  controller.add(&threadUart);
  controller.add(&threadRtc);
  controller.add(&threadLcd);
  controller.add(&threadConnection);
  
  controller.add(&threadAlarm);
  controller.add(threadResetAlarm);
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
    else if (data.startsWith("done measuring")){
      
    }
    else if(data.startsWith("reset alarm")){
      Alarm::getInstance()->resetAlarm();
    }
    else if(data.startsWith("slave1")){
      uartSlave.send("A1");
    }
    else if(data.startsWith("slave2")){
      uartSlave.send("A2");
    }
    else if(data.startsWith("slave3")){
      uartSlave.send("A3");
    }
    else if(data.startsWith("slave4")){
      uartSlave.send("A4");
    }
    else if(data.startsWith("slave5")){
      uartSlave.send("A5");
    }
    else if(data.startsWith("slave6")){
      uartSlave.send("A6");
    }
    else if(data.startsWith("slave7")){
      uartSlave.send("A7");
    }
    else if(data.startsWith("slave8")){
      uartSlave.send("A8");
    }
  }
}

void callback_alarm(){
  ringAlarm->runAlarm();
}
void callback_resetAlarm(){
  ringAlarm->resetAlarm();
  controller.remove(threadResetAlarm);
  delete threadResetAlarm;
  threadResetAlarm = nullptr;
}

void callback_connection(){
  connection->checkingConnection();
}

void callback_sync_data(){
  syncData.syncErr();
  syncData.syncTime();
  syncData.syncConfig();
}
