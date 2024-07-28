#ifndef OTA__H
#define OTA__H

#include "firmware/firmware.h"
#include "../../board/lcd/Lcd.h"

class OtaController
{
public:
  enum STATE{
    STATE_INIT = 0,
    STATE_DOWNLOAD = 1,
    STATE_UPLOAD  =  2,
    STATE_FINISH  =  3,
    STATE_FAIL    =  4,
    STATE_CHECK_AVAILABLE,
    STATE_RES 
  };
  OtaController();
  ~OtaController();
  int32_t isAvailable();
  void run();
  STATE getState() const;
  void setState(const STATE& state);
  static OtaController* getInstance();
private:
    static OtaController* _instance;
    uint32_t _heartBeat;
    int32_t _timeoutCheckNewFw;
    STATE _state;
    Firmware _firmware;
    FwInfo _fwInfo;
};

#endif // OTA__H