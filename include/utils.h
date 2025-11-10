#if !defined UTILS_H
#define UTILS_H

#include <inttypes.h>
#include <chrono>

inline uint64_t millis()
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
}
#endif