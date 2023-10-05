#ifndef _LOG_H_
#define _LOG_H_

#include <cstdio>

enum class LogLevel {
  TRACE = 1,
  DEBUG = 2,
  INFO = 3,
  WARNING = 4,
  ERROR = 5
};


void sec_log_setlevel(const char* level);
bool sec_log_benabled(LogLevel level);

#define sec_log(level, format, ...)        \
  if (sec_log_benabled(level)) { printf(format, ##__VA_ARGS__); }

/*template <typename... Args>
void sec_log(LogLevel level, const char *fmt, Args&&... args)
{
    if (level < def_level)
        return;
    
    std::printf(fmt, args...);
}*/

#endif  /* _LOG_H_ */