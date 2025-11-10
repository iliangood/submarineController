#if !define PACKETS_H
#define PACKETS_H

#include <inttypes.h>

#include "axis.h"

struct ControllerPacket
{
	uint64_t sendTime_ms;
	uint64_t receiveTime_ms;
	Axises speedTarget;
};

struct SubmarinePacket
{
	uint64_t sendTime_ms;
	uint64_t receiveTime_ms;
	Axises currentSpeed;
	Axises currentPos;
};


#endif