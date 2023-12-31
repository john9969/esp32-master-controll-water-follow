#include "uart.h"
ConfigUart uartSensor(COM_PORT_SENSOR);
ConfigUart uartSlave(COM_PORT_SLAVE,9600);
ConfigUart uartDebug(COM_PORT_DEBUG);
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
        data = this->_serial->readStringUntil(')');
        data.trim();
        if(data != ""){
            return true;
        }
        else 
            return false;
    }
    return false;
}
void ConfigUart::clearBuffer(){
    if(this->_serial->available()){
        this->_serial->readString();
    }
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
void ConfigUart::send(const String& data){
    String _data = "";
    _data = data+ ")";
    Serial.println("data send: " + _data); 
    this->_serial->print(_data);
}