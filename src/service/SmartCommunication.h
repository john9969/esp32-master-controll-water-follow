#ifndef SMART_COMMUNICATION_H
#define SMART_COMMUNICATION_H

#include <iostream>
class Communication{
public:
    virtual void post(const std::string& data) = 0;
    virtual std::string get() = 0;
};
class Uart : public Communication
{
public:
    Uart();
    ~Uart();
    void post(const std::string& data);
    void init();

};


#endif
