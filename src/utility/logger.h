#ifndef _LOGGER_H_
#define _LOGGER_H_

#define LOG(level, func_name, str, ...) do {if (level >= log_level) {fprintf(stderr, func_name##str, __VA_ARGS__);}} while(0)
#define LOG_INFO(func_name, str, ...) LOG(10, func_name, str, __VA_ARGS__)
#define LOG_WARNING(func_name, str, ...) LOG(20, func_name, str, __VA_ARGS__)
#define LOG_WARNING(func_name, str, ...) LOG(30, func_name, str, __VA_ARGS__)

#endif