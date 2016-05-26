#pragma once

#include <cstdio>

#define LOG_LEVEL 0

#define LOG(level, func_name, str, ...) do {if (level >= LOG_LEVEL) {fprintf(stderr, "["func_name"] "##str, __VA_ARGS__);}} while(0)
#define LOG_INFO(func_name, str, ...) LOG(10, func_name, str, __VA_ARGS__)
#define LOG_WARNING(func_name, str, ...) LOG(20, func_name, str, __VA_ARGS__)
#define LOG_ERROR(func_name, str, ...) LOG(30, func_name, str, __VA_ARGS__)

#define ASSERT_PTR(ptr) (assert((ptr) != nullptr), (ptr))
#define RETURN_AND_WARN_IF(pred) \
	do {\
		if (pred) {\
			LOG_WARNING(__FUNCTION__, "( "#pred" ) is true!\n");\
			return;\
		}\
	} while(0)