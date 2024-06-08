
#ifndef SERVICE_HTTP_REQUEST__H
#define SERVICE_HTTP_REQUEST__H
#include <Arduino.h>
#include <HTTPClient.h>
#include <iostream> 
#include <vector>    
#include <memory>
#include "../connection/Connection.h"
#include "../config/Config.h"
#define API_GET_CONFIG "https://donuoctrieuduong.xyz/dev_test/get_config.php?serial_number="

#if DEBUG
#define API_GET_TIME "https://donuoctrieuduong.xyz/dev_test/get_time_test.php"
#else
#define API_GET_TIME "https://donuoctrieuduong.xyz/dev_test/get_time.php"
#endif // DEBUG

#define API_POST "https://donuoctrieuduong.xyz/test/index.php/Sql/getdatafromesp8266"
class HttpRequest {
public:
    enum Type{
        TYPE_DEBUG  = 0,
        TYPE_CRITICAL
    };
    String post(const std::vector<std::vector<String>>& data, const String& api);
    String post(const String& dataDebug, const String& api, const Type& type);
    String get(const String& API);
    static std::shared_ptr<HttpRequest> getInstance();
private:
    HttpRequest();
    static std::shared_ptr<HttpRequest> _httpRequest;
};
#endif
