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
			SDL_Joystick* joystick = SDL_GameControllerGetJoystick(controller);
			SDL_JoystickID instance_id = SDL_JoystickInstanceID(joystick);
			joysticks_.insert({instance_id, controller});
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
	std::unordered_map<uint32_t, Joystick>::iterator jit = joysticks_.find(event->which);
	if(jit == joysticks_.end())
		return;
	Joystick& joystick = jit->second;
	if (event->axis == SDL_CONTROLLER_AXIS_LEFTX) // стрейф вправо/влево
	{
		joystick.axises()[AxisesNames::Vy] = value;
		return;
	}
	if (event->axis == SDL_CONTROLLER_AXIS_LEFTY)
	{
		joystick.axises()[AxisesNames::] = -value;
		return;
	}
	if (event->axis == SDL_CONTROLLER_AXIS_RIGHTX) // поворот вправо влево
	{
		joystick.axises()[AxisesNames::Wx] = value;
		return;
	}
	if (event->axis == SDL_CONTROLLER_AXIS_RIGHTY) // поворот нос вверх вниз
	{
		joystick.axises()[AxisesNames::Wy] = value;
		return;
	}
	if (event->axis == SDL_CONTROLLER_AXIS_TRIGGERRIGHT) // вперед назад
	{
		joystick.axises()[AxisesNames::Vx] = joystick.rTrigger() ? -value : value;
		return;
	}
	if (event->axis == SDL_CONTROLLER_AXIS_TRIGGERLEFT) // вверх вниз
	{
		joystick.axises()[AxisesNames::Vz] = joystick.lTrigger() ? -value : value;
		return;
	}
}
void Controller::handleEventButton(SDL_ControllerButtonEvent* event)
{

}