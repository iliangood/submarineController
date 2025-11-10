#include <iostream>
#include <chrono>

#include <SDL2/SDL.h>
#include <udptransmitter.h>

#include "controller.h"

int main()
{
    Controller::getInstance();
	UDPTransmitter transmitter(56728, "submarine");

	Message<256> msg;
	std::chrono::steady_clock::time_point lastSend = std::chrono::steady_clock::now();
	while(true)
	{
		ReceiveInfo rci = transmitter.receiveData(&msg);
		if(recieved(rci))
		{
			
		}
		msg.clear();
		if(std::chrono::steady_clock::now() - lastSend > std::chrono::milliseconds(10))
		{
			msg.push<Packe
		}
		msg.clear();
	}
	
    return 0;
}