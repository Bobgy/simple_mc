#pragma once

#include "stdafx.h"

#include "utility/logger.h"

#define DEBUG_STRING_ID

#ifdef DEBUG_STRING_ID

#include <unordered_map>
#include <string>

#endif

typedef unsigned StringID;
const static uint32_t P1 = (uint64_t)(1e9 + 7), P2 = 9875321ULL;

#ifdef DEBUG_STRING_ID

extern std::unordered_map<StringID, string> g_string_id_to_string_map;

#endif

// unnamed namespace used to hide functions only to the current file
namespace
{
	inline StringID STRING_ID(const char str[])
	{
		uint64_t ret = 0;
		for (const char *p = str; *p; ++p) {
			ret = (ret * P2 + *p) % P1;
		}
#ifdef DEBUG_STRING_ID

		auto it = g_string_id_to_string_map.find(ret);
		if (it == g_string_id_to_string_map.end()) {
			g_string_id_to_string_map[ret] = str;
		}
		else if (it->second != str) {
			LOG_WARNING(
				__FUNCTION__,
				"StringID confliction on %u found! \"%s\" and \"%s\".\n",
				(uint32_t)ret,
				it->second.c_str(),
				str);
		}

#endif
		return (StringID)ret;
	}


	inline string GET_STRING(StringID string_id)
	{
#ifdef DEBUG_STRING_ID

		auto it = g_string_id_to_string_map.find(string_id);
		if (it != g_string_id_to_string_map.end()) {
			return it->second;
		}

		LOG_WARNING(__FUNCTION__, "No original string found in cache for string_id %u.\n", string_id);

#endif

		return string(to_string(string_id));
	}
}