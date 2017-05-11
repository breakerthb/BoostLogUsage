#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <cassert>
#include <iostream>
#include <fstream>

int LogInit();
int LogInit(const char* strLogName);
int LogDeInit();
void LogStart();
void LogStop();
void LogEnableDebug();

int log_error(const char *function_name, const char *msg, ...);
int log_info(const char *function_name, const char *msg, ...);
int log_warning(const char *function_name, const char *msg, ...);
int log_debug(const char *function_name, const char *msg, ...);

#define LogError(msg, ...) do { log_error(__FUNCTION__, msg, ##__VA_ARGS__); } while (0)
#define LogInfo(msg, ...) do { log_info(__FUNCTION__, msg, ##__VA_ARGS__); } while (0)
//#define LogInfo(msg, ...) do { log_info("", msg, ##__VA_ARGS__); } while (0)
#define LogWarning(msg, ...) do { log_warning(__FUNCTION__, msg, ##__VA_ARGS__); } while (0)
#define LogDebug(msg, ...) do { log_debug(__FUNCTION__, msg , ##__VA_ARGS__); } while (0)
#define LogData(func_name, msg, ...) do { log_debug(func_name, msg , ##__VA_ARGS__); } while (0)

#endif
