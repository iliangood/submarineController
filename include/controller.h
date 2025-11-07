#if !defined CONTROLLER_H
#define CONTROLLER_H

#include <inttypes.h>
#include <vector>

#include <SDL2/SDL.h>

#include "axis.h"
class Joystick
{
	SDL_GameController* joystick_;
	Axises axises_;
	bool lTrigger_;
	bool rTrigger_;
public:
	Joystick(SDL_GameController* joystick) : joystick_(joystick), lTrigger_(false), rTrigger_(false) {}
	SDL_GameController*& joystick()
	{
		return joystick_;
	}
	Axises& axises()
	{
		return axises_;
	}
	bool& lTrigger()
	{
		return lTrigger_;
	}
	bool& rTrigger()
	{
		return rTrigger_;
	}
};


class Controller
{
	Axises mainAxises_;
	std::vector<Joystick> joysticks_;
	int deadzone_;

	Controller();
	Controller(const Controller&) = delete;
	Controller& operator=(const Controller&) = delete;
	static int watcherEventAxis(void*, SDL_Event* event);
	static int watcherEventButton(void*, SDL_Event* event);
	void handleEventAxis(SDL_ControllerAxisEvent* event);
	void handleEventButton(SDL_ControllerButtonEvent* event);


public:
	static Controller& getInstance()
	{
		static Controller controller;
		return controller;
	}

	void setDeadZone(int deadzone)
	{ 
		deadzone_ = deadzone; 
		return; 
	}

	int getDeadZone() const { return deadzone_; }

	const Axises& axises() const
	{
		return mainAxises_;
	}
};

#endif