#ifndef OTA__H
#define OTA__H

#include "firmware/firmware.h"

class OtaController
{
public:
  OtaController();
  ~OtaController();
  int32_t isAvailable();
  void run();
  static OtaController* getInstance();
private:
    static OtaController* _instance;
    Firmware _firmware;
    FwInfo _fwInfo;
};

#endif // OTA__H