
#ifndef SERVICE_HTTPREQUEST_H
#define SERVICE_HTTPREQUEST_H
#include <Arduino.h>
#include <HTTPClient.h>
#include <iostream> 
#include "Connection.h"
#include <vector>    
#include <memory>
#define WORKSTATION_ID "010"
class HttpRequest {
public:
    enum Type{
        TYPE_DEBUG,
        TYPE_DATA,
    };
    static String post(const std::vector<std::vector<String>>& data, const String& api);
    static String post(const String& dataDebug, const String& api);
    static String get(const String& API);
    static HttpRequest* getInstance(){
        if(!httpRequest){
            httpRequest = new HttpRequest();
        }
        return httpRequest;
    }
private:
    HttpRequest(){

    }
    static HttpRequest* httpRequest;
};
#endif
HttpRequest* HttpRequest::httpRequest = nullptr;
String HttpRequest::post(const std::vector<std::vector<String>>& data, const String& api){
    if(!httpRequest){
        httpRequest = new HttpRequest();
    }
    Connection * connection = Connection::getInstance();
    if(!connection->isConnected()){
        setErrCode(ERR_WIFI_LOST_CONNECT);
        return "";
    }
    std::unique_ptr<HTTPClient> http(new HTTPClient());
    http->begin(api.c_str());    
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
String HttpRequest::post(const String& data, const String& api){
    if(!httpRequest){
        httpRequest = new HttpRequest();
    }
    Connection * connection = Connection::getInstance();
    if(!connection->isConnected()){
        setErrCode(ERR_WIFI_LOST_CONNECT);
        return "";
    }
    std::unique_ptr<HTTPClient> http(new HTTPClient());
    http->begin(api.c_str());    
    //String httpRequestData = "apikey=senddata&keyword=TA&workstationid=010&data=100'120'180' 060'090'090'";
    String dataPost ="apikey=senddata"
                 "&keyword=";   
    dataPost += "TB";
    dataPost += "&workstationid=" WORKSTATION_ID;
    dataPost += "&data=";
    dataPost += data;
    Serial.println(dataPost);  
    return "";
}
String HttpRequest::get(const String& api){
    if(!httpRequest){
        httpRequest = new HttpRequest();
    }
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

