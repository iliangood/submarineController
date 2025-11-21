#if !defined PACKETS_H
#define PACKETS_H

#include <inttypes.h>

#include "axis.h"
#include <array>

struct ControllerPacket
{
	uint64_t sendTime_ms;
	uint64_t last_packet_rx_time_message;
	Axises speedTarget;

	std::array<uint8_t, 28> serialize()
	{
		std::array<uint8_t, 28> data{};
		*reinterpret_cast<uint64_t*>(&data[0]) = sendTime_ms;
		*reinterpret_cast<uint64_t*>(&data[8]) = last_packet_rx_time_message;
		for(size_t i = 0; i < 6; ++i)
		{
			*reinterpret_cast<int16_t*>(&data[16 + i * 2]) = speedTarget[i];
		}
		return data;
	}
	static ControllerPacket deserialize(const std::array<uint8_t, 28>& data)
	{
		ControllerPacket packet;
		packet.sendTime_ms = *reinterpret_cast<const uint64_t*>(&data[0]);
		packet.last_packet_rx_time_message = *reinterpret_cast<const uint64_t*>(&data[8]);
		for(size_t i = 0; i < 6; ++i)
		{
			packet.speedTarget[i] = *reinterpret_cast<const int16_t*>(&data[16 + i * 2]);
		}
		return packet;
	}
	static constexpr size_t serializedSize()
	{
		return 28;
	}
};

struct SubmarinePacket
{
	uint64_t sendTime_ms;
	uint64_t last_packet_rx_time_message;
	Axises currentSpeed;
	Axises currentPos;

	std::array<uint8_t, 40> serialize()
	{
		std::array<uint8_t, 40> data{};
		*reinterpret_cast<uint64_t*>(&data[0]) = sendTime_ms;
		*reinterpret_cast<uint64_t*>(&data[8]) = last_packet_rx_time_message;
		for(size_t i = 0; i < 6; ++i)
		{
			*reinterpret_cast<int16_t*>(&data[16 + i * 2]) = currentSpeed[i];
			*reinterpret_cast<int16_t*>(&data[28 + i * 2]) = currentPos[i];
		}
		return data;
	}
	static SubmarinePacket deserialize(const std::array<uint8_t, 40>& data)
	{
		SubmarinePacket packet;
		packet.sendTime_ms = *reinterpret_cast<const uint64_t*>(&data[0]);
		packet.last_packet_rx_time_message = *reinterpret_cast<const uint64_t*>(&data[8]);
		for(size_t i = 0; i < 6; ++i)
		{
			packet.currentSpeed[i] = *reinterpret_cast<const int16_t*>(&data[16 + i * 2]);
			packet.currentPos[i] = *reinterpret_cast<const int16_t*>(&data[28 + i * 2]);
		}
		return packet;
	}
	static constexpr size_t serializedSize()
	{
		return 40;
	}
};


#endif