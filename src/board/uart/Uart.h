#ifndef BOARD_UART__H
#define BOARD_UART__H

#include <Arduino.h>
#include <HardwareSerial.h>
#include <iostream>
#define UART_SLAVE Serial1
#define UART_SENSOR Serial2
#define UART_DEBUG Serial
enum COM_PORT{
    COM_PORT_SLAVE = 0,
    COM_PORT_SENSOR,
    COM_PORT_DEBUG
};
class ConfigUart{
public:

    ConfigUart(COM_PORT comPort, int _baud = 115200);
    void begin();
    void send(const String& data);
    bool read(String& data);
    void clearBuffer();
private:
    COM_PORT comport;
    HardwareSerial* _serial;
    void removeLast(String &data);
    int baud;
};
extern ConfigUart uartSensor;
extern ConfigUart uartSlave;
extern ConfigUart uartDebug;
#endif // BOART_UART_H
