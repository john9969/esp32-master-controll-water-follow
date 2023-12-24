#include "uart.h"
ConfigUart uartSensor(COM_PORT_SENSOR);
ConfigUart uartSlave(COM_PORT_SLAVE);
void ConfigUart::begin(){
    this->_serial->begin(baud);
}
void ConfigUart::removeLast(String &  data){
    int length = data.length();
    if(length >1){
        data.remove(length-1);
    }
    else {
        data="";
    }
}
bool ConfigUart::read(String& data){
    if(this->_serial->available()){
        data = this->_serial->readString();
        Serial.println("sensor:"+data);
    }
    data.trim();
    if(data.endsWith(")"));
    removeLast(data);
    if(data != "")
        return true;
    else 
        return false;
}
ConfigUart::ConfigUart(COM_PORT comPort, int _baud):comport(comPort), baud(_baud){
    switch (this->comport)
    {
    case COM_PORT_SLAVE:
        this->_serial = &UART_SLAVE;
        break;
    case COM_PORT_SENSOR:
        this->_serial = &UART_SENSOR;
        break;
    default:
        this->_serial = &UART_DEBUG;
        break;
    }
    this->baud = _baud;
}
void ConfigUart::send(const std::string& data){
    char _data[100];
    int length = data.length();
    if(length > 100) length = 100;
    for(int i =0 ; i< length; i++){
        _data[i] = data.at(i);
    }
    _data[length] = '\0';
    this->_serial->print(_data);
}