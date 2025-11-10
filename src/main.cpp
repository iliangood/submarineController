#include <iostream>
#include <chrono>

#include <SDL2/SDL.h>
#include <udptransmitter.h>

#include "controller.h"
#include "packets.h"

#include "utils.h"

int main()
{
    Controller::getInstance();
	UDPTransmitter transmitter(56728, "submarine");

	Message<256> msg;
	std::chrono::steady_clock::time_point lastSend = std::chrono::steady_clock::now();
	uint64_t last_packet_rx_time_message = 0;
	uint64_t ping_ms = 0;
	while(true)
	{
		ReceiveInfo rci = transmitter.receiveData(&msg);
		if(recieved(rci))
		{
			SubmarinePacket inPacket = msg.read<SubmarinePacket>();
			last_packet_rx_time_message = inPacket.sendTime_ms;
			ping_ms = millis() - inPacket.last_packet_rx_time_message;
		}
		msg.clear();
		if(std::chrono::steady_clock::now() - lastSend > std::chrono::milliseconds(10))
		{
			msg.push(ControllerPacket
				{
					static_cast<uint64_t>(millis()),
					last_packet_rx_time_message,
					Controller::getInstance().axises()
				});
		}
		msg.clear();
		std::cout << "ping:" << ping_ms << "ms\n";
	}
	
    return 0;
}