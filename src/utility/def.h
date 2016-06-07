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

#define RETURN_STH_AND_WARN_IF(pred, ret) \
	do {\
		if (pred) {\
			LOG_WARNING(__FUNCTION__, "( "#pred" ) is true!\n");\
			return (ret);\
		}\
	} while(0)

#define RETURN_IF(pred) if (pred) return

#define RETURN_IF_NULL(obj) if ((obj) == nullptr) return
#define RETURN_STH_IF_NULL(obj, ret) if ((obj) == nullptr) return (ret)

#ifndef _DEBUG
	#define ASSERT_CAST static_cast
#else
	#define ASSERT_CAST dynamic_cast
#endif