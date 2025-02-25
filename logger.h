#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "ini.h"

void log_message(LogConfig *logconfig, const char *format, ...);

#endif // LOGGER_H
