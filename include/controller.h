#if !defined CONTROLLER_H
#define CONTROLLER_H

#include <inttypes.h>

#include <SDL2/SDL.h>

#include "axis.h"
class Joystick
{
	SDL_GameController* joystick_;
	Axises axises_;
public:
	SDL_GameController*& joystick()
	{
		return joystick_;
	}

};


class Controller
{
	Axises mainAxises_;
	
	Controller();
	Controller(const Controller&) = delete;
	Controller& operator=(const Controller&) = delete;
	void handleEvent(SDL_ControllerAxisEvent event);
	void handleEvent(SDL_ControllerButtonEvent);

public:
	static Controller& getInstance()
	{
		static Controller controller;
		return controller;
	}

	

};

#endif