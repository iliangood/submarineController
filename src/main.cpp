#include <iostream>
#include <chrono>
#include <thread>

#include <SDL2/SDL.h>



#include <udptransmitter.h>

#include "controller.h"
#include "packets.h"

#include "utils.h"

int main()
{
    debug_init();

	spdlog::set_level(spdlog::level::debug);

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
			std::cout << "Received packet from " << rci.remoteIP.value_or(IPAddress(0,0,0,0)) << " size=" << rci.dataSize << " bytes\n";
			SubmarinePacket inPacket = msg.read<SubmarinePacket>();
			last_packet_rx_time_message = inPacket.sendTime_ms;
			ping_ms = millis() - inPacket.last_packet_rx_time_message;
		}
		msg.clear();
		if(std::chrono::steady_clock::now() - lastSend > std::chrono::milliseconds(100))
		{
			msg.push(ControllerPacket
				{
					static_cast<uint64_t>(millis()),
					last_packet_rx_time_message,
					Controller::getInstance().axises()
				});
			transmitter.sendData(msg);
			lastSend = std::chrono::steady_clock::now();
		}
		msg.clear();
		const Axises& axises = Controller::getInstance().axises();
		//std::cout << axises[0] << ' ' << axises[1] << ' ' << axises[2] << ' ' << axises[3] << ' ' << axises[4] << ' ' << axises[5] << '\n';
		//std::cout << "ping:" << ping_ms << "ms" << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	
    return 0;
}