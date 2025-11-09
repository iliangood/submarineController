#include "controller.h"

#include <iostream>
#include <algorithm>

#include <SDL2/SDL.h>


void Controller::Joystick::handleEventAxis(SDL_ControllerAxisEvent* event, int16_t deadzone)
{
	int16_t value = event->value;
	if(abs(value) < deadzone)
		value = 0;
	if (event->axis == SDL_CONTROLLER_AXIS_LEFTX) // стрейф вправо/влево
	{
		joystickState_.leftStickX = value;
		axises_[AxisesNames::Vy] = value;
		return;
	}
	if (event->axis == SDL_CONTROLLER_AXIS_LEFTY) // вперед назад
	{
		joystickState_.leftStickY = value;
		axises_[AxisesNames::Vx] = -value;
		return;
	}
	if (event->axis == SDL_CONTROLLER_AXIS_RIGHTX) // поворот вправо влево
	{
		joystickState_.rightStickX = value;
		axises_[AxisesNames::Wz] = value;
		return;
	}
	if (event->axis == SDL_CONTROLLER_AXIS_RIGHTY) // поворот нос вверх вниз
	{
		joystickState_.rightStickY = value;
		axises_[AxisesNames::Wy] = value;
		return;
	}
	if (event->axis == SDL_CONTROLLER_AXIS_TRIGGERRIGHT) // вперед назад
	{
		joystickState_.rightTrigger = value;
		axises_[AxisesNames::Vz] = joystickState_.leftTrigger - joystickState_.rightTrigger;
		return;
	}
	if (event->axis == SDL_CONTROLLER_AXIS_TRIGGERLEFT) // вверх вниз
	{
		joystickState_.leftTrigger = value;
		axises_[AxisesNames::Vz] = joystickState_.leftTrigger - joystickState_.rightTrigger;
		return;
	}
}

void Controller::Joystick::handleEventButton(SDL_ControllerButtonEvent* event, int16_t rollSpeed)
{
	bool state = event->state == SDL_PRESSED;

	if(event->button == SDL_CONTROLLER_BUTTON_RIGHTSHOULDER)
	{
		joystickState_.rightShoulder = state;
		axises_[AxisesNames::Wx] = (joystickState_.rightShoulder - joystickState_.leftShoulder) * rollSpeed;
		return;
	}
	if(event->button == SDL_CONTROLLER_BUTTON_LEFTSHOULDER)
	{
		joystickState_.leftShoulder = state;
		axises_[AxisesNames::Wx] = (joystickState_.rightShoulder - joystickState_.leftShoulder) * rollSpeed;
		return;
	}
}



Controller::Controller() : deadzone_(2000)
{
	if(SDL_Init(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER) < 0)
	{
		std::cerr << "SDL init failed: " << SDL_GetError();
		throw std::runtime_error("SDL init failed");
		return;
	}
	if(SDL_GameControllerAddMappingsFromFile("gamecontrollerdb.txt") < 0)
	{
		std::cerr << "SDL add mapping failed: " << SDL_GetError();
		throw std::runtime_error("SDL add mapping failed");
		return;
	}
	int numJoysticks = SDL_NumJoysticks();
	if(numJoysticks < 0)
	{
		std::cerr << "num joystics failed: " << SDL_GetError();
		throw std::runtime_error("num joystics failed");
		return;
	}
	for(int i = 0; i < numJoysticks; ++i)
	{
		SDL_GameController* controller = SDL_GameControllerOpen(i);
		if(controller != nullptr)
		{
			SDL_Joystick* joystick = SDL_GameControllerGetJoystick(controller);
			SDL_JoystickID instance_id = SDL_JoystickInstanceID(joystick);
			joysticks_.insert({instance_id, controller});
		}
		else
		{
			std::cerr << "SDL_GameControllerOpen failed" << std::endl;
		}
	}
	rollSpeed_ = INT16_MAX;
	axisesUpdated_ = false;
	SDL_GameControllerEventState(SDL_ENABLE);
	SDL_AddEventWatch(watcherEventAxis, nullptr);
	SDL_AddEventWatch(watcherEventButton, nullptr);
	SDL_AddEventWatch(watcherEventDevicesUpdate, nullptr);
}

Controller::~Controller()
{
	for(std::pair<int32_t, Joystick> joystick : joysticks_)
	{
		if(joystick.second.joystick() != nullptr)
		{
			SDL_GameControllerClose(joystick.second.joystick());
			joystick.second.joystick() = nullptr;
		}
	}
	joysticks_.clear();
	SDL_DelEventWatch(watcherEventAxis, nullptr);
	SDL_DelEventWatch(watcherEventButton, nullptr);
	SDL_DelEventWatch(watcherEventDevicesUpdate, nullptr);
	SDL_QuitSubSystem(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER);
}

void Controller::updateAxises()
{
	std::lock_guard lock(mutex_);
	if(axisesUpdated_)
		return;
	mainAxises_ = Axises();
	for(std::pair<int32_t, Joystick> joystick : joysticks_)
	{
		for(int i = 0; i < 6; ++i)
		{
			mainAxises_[i] = static_cast<int16_t>(
				std::clamp(
					static_cast<int32_t>(joystick.second.axises()[i])
					 + static_cast<int32_t>(mainAxises_[i]),
					static_cast<int32_t>(INT16_MIN),
					static_cast<int32_t>(INT16_MAX)));
		}
	}
	axisesUpdated_ = true;
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

int Controller::watcherEventDevicesUpdate(void*, SDL_Event* event)
{
	if(event->type != SDL_CONTROLLERDEVICEADDED && event->type != SDL_CONTROLLERDEVICEREMOVED && event->type != SDL_CONTROLLERDEVICEREMAPPED)
		return 0;
	Controller& crt = getInstance();
	crt.handleEventDevicesUpdate(&event->cdevice);
	return 0;
}

void Controller::handleEventAxis(SDL_ControllerAxisEvent* event)
{
	std::lock_guard lock(mutex_);
	SDL_JoystickID instance_id = event->which;
	if(instance_id < 0)
		return;		
	
	std::map<int32_t, Joystick>::iterator jit = joysticks_.find(instance_id);
	if(jit == joysticks_.end())
		return;
	axisesUpdated_ = false;
	jit->second.handleEventAxis(event, deadzone_);
}
void Controller::handleEventButton(SDL_ControllerButtonEvent* event)
{
	std::lock_guard lock(mutex_);
	SDL_JoystickID instance_id = event->which;
	if(instance_id < 0)
		return;
	
	std::map<int32_t, Joystick>::iterator jit = joysticks_.find(instance_id);
	if(jit == joysticks_.end())
		return;
	axisesUpdated_ = false;
	jit->second.handleEventButton(event, rollSpeed_);
}

void Controller::handleEventDevicesUpdate(SDL_ControllerDeviceEvent* event)
{	
	if(event->type == SDL_CONTROLLERDEVICEADDED)
	{
		int32_t device_index = event->which;
		SDL_GameController* controller = SDL_GameControllerOpen(device_index);
		SDL_Joystick* joystick = SDL_GameControllerGetJoystick(controller);
		SDL_JoystickID instance_id = SDL_JoystickInstanceID(joystick);
		if(instance_id < 0)
		{
			std::cerr << "Controller::handleEventDevicesUpdate: SDL_CONTROLLERDEVICEADDED: incorrect instance_id" << std::endl;
			return;
		}
		joysticks_.insert({instance_id, controller});
		return;
	}
	if(event->type == SDL_CONTROLLERDEVICEREMOVED)
	{
		SDL_JoystickID instance_id = event->which;
		if(instance_id < 0)
		{
			std::cerr << "Controller::handleEventDevicesUpdate: SDL_CONTROLLERDEVICEREMOVED: incorrect instance_id" << std::endl;
			return;
		}
		joysticks_.erase(instance_id);
		return;
	}
	if(event->type == SDL_CONTROLLERDEVICEREMAPPED)
	{
		axisesUpdated_ = false;
		return;
	}
}

void Controller::setRollSpeed(int16_t speed)
{
	rollSpeed_ = speed;
}
int16_t Controller::getRollSpeed()
{
	return rollSpeed_;
}