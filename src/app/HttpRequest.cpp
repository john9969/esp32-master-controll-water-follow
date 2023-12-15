#include <Arduino.h>
#include <iostream>
#include "lib/json/cJSON.h"
class HttpRequest
{
private:
    /* data */
    
    public:
    HttpRequest(/* args */);
    virtual std::string get(const std::string & _url) = 0;
    virtual std::string post(const std::string & data) = 0;
    ~HttpRequest();
};
HttpRequest::HttpRequest(/* args */)

{
}

HttpRequest::~HttpRequest()
{
}
