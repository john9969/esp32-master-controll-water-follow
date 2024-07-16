#ifndef LOG__H
#define LOG__H
#include <Arduino.h>
#include "../../board/rtc/Rtc.h"
#define MAXINUM_LOG_LENGTH 1024
class Log{
public:
    enum TYPE_LOG{
        TYPE_LOG_DEBUG  =    0,
        TYPE_LOG_ERR,
        TYPE_LOG_WARNING
    };
    Log();
    ~Log();
    int32_t addLog(const String& data, TYPE_LOG typeLog);
    int32_t getLengthLogData() const;
    String getLogData() const;
    int32_t clearLogData();

private:
    String _logData;

};

#endif // !LOG__H
