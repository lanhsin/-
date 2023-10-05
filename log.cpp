#include "log.h"
#include <string.h>


LogLevel def_level = LogLevel::ERROR;

void sec_log_setlevel(const char* level)
{
    if (strcmp(level, "TRACE") == 0)
        def_level = LogLevel::TRACE;
    else if (strcmp(level, "INFO") == 0)
        def_level = LogLevel::INFO;
    else if (strcmp(level, "WARNING") == 0)
        def_level = LogLevel::WARNING;
    else
        def_level = LogLevel::ERROR;
}


bool sec_log_benabled(LogLevel level)
{
    if (level >= def_level)
        return true;
    
    return false;
}