#include <iostream>
#include <chrono>
#include <thread>

#include <SDL2/SDL.h>
#include <filesystem>

#include <udptransmitter.h>

#include "attitudeIndicator.h"

#include "controller.h"
#include "packets.h"

#include "utils.h"

int main()
{
    debug_init();

	spdlog::set_level(spdlog::level::trace);

    Controller::getInstance();
	UDPTransmitter transmitter(56728, "submarine");

	Message<256> msg;
	std::chrono::steady_clock::time_point lastSend = std::chrono::steady_clock::now();
	uint64_t last_packet_rx_time_message = 0;
	uint64_t ping_ms = 0;

	AttitudeIndicator horizon;

	SDL_Event event;

	while(true)
	{

		while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            return 0; // Завершить программу при Ctrl+C или закрытии окна
        }
    }

		ReceiveInfo rci = transmitter.receiveData(&msg);
		if(recieved(rci) && rci.dataSize == SubmarinePacket::serializedSize())
		{
			/*if(rci.dataSize != SubmarinePacket::serializedSize())
			{
				spdlog::warn("incorrect packet size from {}", rci.remoteIP.value_or(IPAddress(0,0,0,0)).toString());
			}*/
			SubmarinePacket inPacket = SubmarinePacket::deserialize(msg.read<std::array<uint8_t,40>>());
			last_packet_rx_time_message = inPacket.sendTime_ms;
			ping_ms = millis() - inPacket.last_packet_rx_time_message;
			spdlog::info("received packet from {}", rci.remoteIP.value_or(IPAddress(0,0,0,0)).toString());
			//uint64_t curCount = msg.read<uint64_t>();
			Axises Saxises = inPacket.currentPos;
			std::cout << Saxises[3] << ' ' << Saxises[4] << ' ' << Saxises[5] << std::endl;
			horizon.sendAngles(-static_cast<double>(Saxises[4])/std::numeric_limits<int16_t>::max()*180, static_cast<double>(Saxises[3])/std::numeric_limits<int16_t>::max()*180);
		}
		msg.clear();
		if(std::chrono::steady_clock::now() - lastSend > std::chrono::milliseconds(100))
		{
			msg.push(ControllerPacket
				{
					millis(),
					last_packet_rx_time_message,
					Controller::getInstance().axises()
				}.serialize());
			transmitter.sendData(msg);
			//std::cout << "send time:" << millis() << '\n';
		}
		msg.clear();
		const Axises& axises = Controller::getInstance().axises();
		//std::cout << axises[0] << ' ' << axises[1] << ' ' << axises[2] << ' ' << axises[3] << ' ' << axises[4] << ' ' << axises[5] << '\n';
		//std::cout << "ping:" << ping_ms << "ms" << std::endl;
		//std::cout << "target IP:" << transmitter.targetIP() << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	
    return 0;
}