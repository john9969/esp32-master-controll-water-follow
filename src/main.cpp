#include "app/syncData/SyncData.h"
#include "lib/thread/Thread.h"
#include "lib/thread/ThreadController.h"
#include "board/eeprom/Eeprom.h"
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
void callback_ota();
void getSerialNumber();
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

Thread threadIO = Thread();
Thread threadLcd = Thread();
Thread threadRtc = Thread();
Thread threadOta = Thread();

Thread threadAlarm = Thread();
Thread *threadResetAlarm = new Thread();
Thread threadConnection = Thread();
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
  threadOta.setInterval(FRE_SYNC_OTA);
  threadOta.onRun(callback_ota);
  threadResetAlarm->setInterval(10000);
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
  getSerialNumber();
  
  connection->init();
  logicControl->init();
  readSensor->init();
  uartSlave.begin();

  controller.add(&threadUart);
  controller.add(&threadRtc);
  controller.add(&threadLcd);
  controller.add(&threadConnection);
  controller.add(&threadOta);

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
    data.trim();
    if(data.startsWith("start")){
      hasStartBtn =true;
      Serial.println("start ok");
    }
    else if(data.startsWith("reset")){
      ESP.restart();
    }
    else if (data.startsWith("done measuring")){
      
    }
    else if(data.startsWith("get remain")){
      Serial.printf("remain: %d minute\n", Alarm::getInstance()->getRemainMinute());
    }
    else if(data.startsWith("setTime:")){
      data.remove(0,8);
      std::vector<String> time;
      for(int i =0;i <6;i++){
        time.push_back(data.substring(0,2));
        data.remove(0, 2);
      }
      Time _tm = Time{uint8_t(time.at(0).toInt()),uint8_t(time.at(1).toInt())
                    ,uint8_t(time.at(2).toInt()),1,uint8_t(time.at(3).toInt()),
                    uint8_t(time.at(4).toInt()) ,uint8_t(time.at(5).toInt())};
      Rtc::getInstance()->setTime(_tm);
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
    else if(data.startsWith("serial:")){
      data.remove(0,7);
      data.replace("\n","");
      data.replace("\r","");
      data.trim();
      if(data.length() <3) {
        Serial.printf("Serial wrong: %s\n",data);
        return;
      }
        EEPROM.write(0, data[0]);
        EEPROM.write(1, data[1]);
        EEPROM.write(2, data[2]);
        EEPROM.write(3, '\0'); // Null-terminate the string
        EEPROM.commit();
        Serial.println("write success");
    }
    else if(data.startsWith("read_serial_eeprom")){
      char serial[4];
      for (int i = 0; i < 3; i++)
      {
        serial[i]= (EEPROM.read(i));
      }
      serial[3] = '\0';
      Serial.printf("serial: %s", serial); 
    }
    else if(data.startsWith("read_serial_config")){
      Serial.printf("serial: %s\n",DataConfig::getInstance()->getSerialNumber());
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
void callback_ota(){
  syncData.syncOta();
}
void getSerialNumber(){
  if(!EEPROM.begin(4)){
    Serial.println("eeprom can not open");
  }
  else {
    char serial[4] ={0};
    for (int i = 0; i < 3; i++)
      {
        serial[i]= (EEPROM.read(i));
      }
      serial[3] = '\0';
      Serial.printf("set seial: %s",serial);
      DataConfig::getInstance()->setSerialNumber(serial);
  }
}