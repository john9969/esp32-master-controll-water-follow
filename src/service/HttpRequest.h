
#ifndef SERVICE_HTTPREQUEST_H
#define SERVICE_HTTPREQUEST_H
#include <Arduino.h>
#include <HTTPClient.h>
#include <iostream> 
#include "Connection.h"
#include <vector>    
#include <memory>
#define WORKSTATION_ID "010"
#define API_GET_TIME "https://donuoctrieuduong.xyz/dev_test/get_time.php"
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
    static std::shared_ptr<HttpRequest> getInstance(){
        if(!httpRequest){
            httpRequest = std::shared_ptr<HttpRequest>(new HttpRequest());
        }
        return httpRequest;
    }
private:
    HttpRequest(){

    }
    static std::shared_ptr<HttpRequest> httpRequest;
};
#endif
std::shared_ptr<HttpRequest> HttpRequest::httpRequest = nullptr;

String HttpRequest::post(const std::vector<std::vector<String>>& data, const String& api){
    Connection* connection = Connection::getInstance();
    if(!connection->isConnected()){
        setErrCode(ERR_WIFI_LOST_CONNECT);
        return "";
    }
    std::unique_ptr<HTTPClient> http(new HTTPClient());
    http->begin(api.c_str());    
    http->addHeader("Content-Type", "application/x-www-form-urlencoded");
    //String httpRequestData = "apikey=senddata&keyword=TA&workstationid=010&data=100'120'180' 060'090'090'";
    String dataPost ="apikey=senddata"
                 "&keyword=";   
    dataPost += "TA";
    dataPost += "&workstationid=" WORKSTATION_ID;
    dataPost += "&data=";
    for(std::vector<String> value:data){
        for(String p_value : value){
            dataPost += p_value;
        }
    }
    Serial.println("data post: "  + dataPost);  
    
    int httpResponseCode = http->POST(dataPost);
    String payload="";
    if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        payload = http->getString();
        Serial.println(payload);
      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
    }
    http->end();
    return payload;
}
String HttpRequest::post(const String& data, const String& api, const Type& type){
    Connection* connection = Connection::getInstance();
    // Connection * connection = Connection::getInstance();
    if(!connection->isConnected()){
        setErrCode(ERR_WIFI_LOST_CONNECT);
        return "";
    }
    std::unique_ptr<HTTPClient> http(new HTTPClient());
    http->begin(api.c_str());    
    http->addHeader("Content-Type", "application/x-www-form-urlencoded");
    //String httpRequestData = "apikey=senddata&keyword=TA&workstationid=010&data=100'120'180' 060'090'090'";
    String dataPost ="apikey=senddata"
                 "&keyword=";   
    if(type == TYPE_CRITICAL) dataPost += "TC";
    else if( type == TYPE_DEBUG) dataPost += "TB";
    dataPost += "&workstationid=" WORKSTATION_ID;
    dataPost += "&data=";
    dataPost += data;
    Serial.println(dataPost);  
    int httpResponseCode = http->POST(dataPost);
    String payload="";
    if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        payload = http->getString();
        Serial.println(payload);
      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
    }
    http->end();
    return payload;
}
String HttpRequest::get(const String& api){
    std::unique_ptr<HTTPClient> http(new HTTPClient());
    http->begin(api.c_str());    
    int httpResponseCode = http->GET();

  String payload = ""; 

  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http->getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  http->end();
  return payload;      
}

