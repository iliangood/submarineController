#include "controller.h"

#include <iostream>

#include <SDL2/SDL.h>


Controller::Controller() : deadzone_(2000)
{
	if(SDL_Init(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER) < 0)
	{
		std::cerr << "SDL init failed: " << SDL_GetError();
		return;
	}
	if(SDL_GameControllerAddMappingsFromFile("gamecontrollerdb.txt") < 0)
	{
		std::cerr << "SDL add mapping failed: " << SDL_GetError();
		return;
	}
	int numJoysticks = SDL_NumJoysticks();
	if(numJoysticks < 0)
	{
		std::cerr << "num joystics failed: " << SDL_GetError();
		return;
	}
	joysticks_.reserve(numJoysticks);
	for(int i = 0; i < numJoysticks; ++i)
	{
		SDL_GameController* controller = SDL_GameControllerOpen(i);
		if(controller != nullptr)
		{
			joysticks_.emplace_back(controller);
		}
	}
	SDL_GameControllerEventState(SDL_ENABLE);

}

int Controller::watcherEventAxis(void*, SDL_Event* event)
{
	if(event->type != SDL_CONTROLLERAXISMOTION)
		return 0;
	Controller& crt = getInstance();
	crt.handleEventAxis(&event->caxis);
	return 0;
}
int Controller::watcherEventButton(void*, SDL_Event* event)
{
	if(event->type != SDL_CONTROLLERBUTTONDOWN && event->type != SDL_CONTROLLERBUTTONUP)
		return 0;
	Controller& crt = getInstance();
	crt.handleEventButton(&event->cbutton);
	return 0;
}

void Controller::handleEventAxis(SDL_ControllerAxisEvent* event)
{
	int16_t value = event->value;
	if(abs(value) < deadzone_)
		value = 0;
	
	if (event->axis == SDL_CONTROLLER_AXIS_LEFTX)
	{
		axises
	}
	
}
void Controller::handleEventButton(SDL_ControllerButtonEvent* event)
{

}