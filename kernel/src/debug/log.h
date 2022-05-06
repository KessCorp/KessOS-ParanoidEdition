#ifndef LOG_H
#define LOG_H

#include <stdarg.h>

typedef enum {
   S_WARNING,
   S_INFO,
   S_CRITICAL,
   S_PANIC,
} STATUS;


void log(const char* format, STATUS status, ...);

#endif
