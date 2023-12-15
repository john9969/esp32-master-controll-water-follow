#ifndef BOARD_UART_H
#define BOARD_UART_H
#include <Arduino.h>
#include <HardwareSerial.h>
#include <iostream>
#define UART_SLAVE Serial1
#define UART_SENSOR Serial2
#define UART_DEBUG Serial
class Uart{
public:
    enum COM_PORT{
        COM_PORT_SLAVE = 0,
        COM_PORT_SENSOR,
        COM_PORT_DEBUG
    };
    Uart(COM_PORT comPort, bool _isUsingSmartSend = false, int _baud = 115200){
        switch (comPort)
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
        this->isUsingSmartSend = _isUsingSmartSend;
        
    }
    void init(){
        this->_serial->begin(baud);
    }
    void send(const std::string& data){
        char _data[100];
        int length = data.length();
        if(length > 100) length = 100;
        for(int i =0 ; i< length; i++){
            _data[i] = data.at(i);
        }
        _data[length] = '\0';
        this->_serial->print(_data);
    }
    int read(){
        
    }
private:
    HardwareSerial* _serial;
    bool isUsingSmartSend;
    int baud;
};
#endif // BOART_UART_H