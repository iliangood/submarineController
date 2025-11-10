#if !defined PACKETS_H
#define PACKETS_H

#include <inttypes.h>

#include "axis.h"

struct ControllerPacket
{
	uint64_t sendTime_ms;
	uint64_t last_packet_rx_time_message;
	Axises speedTarget;
};

struct SubmarinePacket
{
	uint64_t sendTime_ms;
	uint64_t last_packet_rx_time_message;
	Axises currentSpeed;
	Axises currentPos;
};


#endif