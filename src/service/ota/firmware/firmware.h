#ifndef FIRMWARE__H
#define FIRMWARE__H
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Update.h>
#include "../../../lib/tiny-json/tiny-json.h"
#include "../../../service/config/Config.h"
#include "../../../board/lcd/Lcd.h"



struct FwSession
{
  int32_t _size;
  uint8_t data[MAXIMUM_FW_LENGTH_SESSION];
  void operator = (const FwSession& fwSession){
    this->_size = fwSession._size;
    memcpy(this->data,fwSession.data,MAXIMUM_FW_LENGTH_SESSION);
  }
};
struct FwInfo{
  int _totalSize;
  uint32_t _index;
  uint32_t _numOfPart;
  String _fwVersion;
  FwSession _session;
  };
class Firmware
{
public:
  Firmware();
  ~Firmware();
  int32_t isNewFwAvailable(FwInfo& fw);
  uint32_t getFwSessionSize(FwInfo& fwInfo);
  int32_t downloadFw(const String& url, uint8_t *image ,const uint32_t& index ,const uint32_t& length = MAXIMUM_FW_INFO);
  int32_t beginWriteImage(const uint32_t & totalSize);
  int32_t writeImage(uint8_t * image, const uint32_t & size);
  int32_t otaFinish(FwInfo& fwInfo);
  int32_t getValueInt(String data, String key, int & value);
  int32_t checkFwSize();
  int32_t getValueText(String data, String key, String & value);

};


#endif // !FIRMWARE__H
