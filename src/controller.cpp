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
	if (event->axis == SDL_CONTROLLER_AXIS_LEFTY) // вперед назад
	{
		joystick.axises()[AxisesNames::Vx] = -value;
		return;
	}
	if (event->axis == SDL_CONTROLLER_AXIS_RIGHTX) // поворот вправо влево
	{
		joystick.axises()[AxisesNames::Wz] = value;
		return;
	}
	if (event->axis == SDL_CONTROLLER_AXIS_RIGHTY) // поворот нос вверх вниз
	{
		joystick.axises()[AxisesNames::Wy] = value;
		return;
	}
	if (event->axis == SDL_CONTROLLER_AXIS_TRIGGERRIGHT) // вперед назад
	{
		if(joystick.axises()[AxisesNames::Vz] > 0 && abs(value) < abs(joystick.axises()[AxisesNames::Vz]))
			return;
		joystick.axises()[AxisesNames::Vz] = -value;
		return;
	}
	if (event->axis == SDL_CONTROLLER_AXIS_TRIGGERLEFT) // вверх вниз
	{
		if(joystick.axises()[AxisesNames::Vz] < 0 && abs(value) < abs(joystick.axises()[AxisesNames::Vz]))
			return;
		joystick.axises()[AxisesNames::Vz] = value;
		return;
	}
}
void Controller::handleEventButton(SDL_ControllerButtonEvent* event)
{
	bool state = event->button;
	std::unordered_map<uint32_t, Joystick>::iterator jit = joysticks_.find(event->which);
	if(jit == joysticks_.end())
		return;
	Joystick& joystick = jit->second;
	if(event->button == SDL_CONTROLLER_BUTTON_RIGHTSHOULDER)
	{
		if(joystick.axises()[AxisesNames::Wx] < 0 && !state)
			return;
		joystick.axises()[AxisesNames::Wx] = state ? INT32_MAX : 0;
		return;
	}
	if(event->button == SDL_CONTROLLER_BUTTON_LEFTSHOULDER)
	{
		if(joystick.axises()[AxisesNames::Wx] > 0 && !state)
			return;
		joystick.axises()[AxisesNames::Wx] = state ? -INT32_MAX : 0;
		return;
	}
}