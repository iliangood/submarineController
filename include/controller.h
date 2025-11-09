#if !defined CONTROLLER_H
#define CONTROLLER_H

#include <inttypes.h>
#include <vector>
#include <unordered_map>
#include <mutex>

#include <SDL2/SDL.h>

#include "axis.h"

struct JoystickState
{
    int16_t leftStickX  = 0;
    int16_t leftStickY  = 0;
    int16_t rightStickX = 0;
    int16_t rightStickY = 0;

    int16_t leftTrigger  = 0;
    int16_t rightTrigger = 0;

    bool leftShoulder  = false;
    bool rightShoulder = false;
};

class Joystick
{
	SDL_GameController* joystick_;
	Axises axises_;
	JoystickState joystickState_;

public:
	Joystick(SDL_GameController* joystick) : joystick_(joystick) {}
	Joystick(const Joystick& other) : joystick_(other.joystick_), axises_(other.axises_), joystickState_(other.joystickState_) {}
	~Joystick() {}
	Joystick& operator=(const Joystick& other)
	{
		joystick_ = other.joystick_;
		axises_ = other.axises_;
		joystickState_ = other.joystickState_;
		return *this;
	}
	SDL_GameController*& joystick()
	{
		return joystick_;
	}
	Axises& axises()
	{
		return axises_;
	}
	const Axises& axises() const
	{
		return axises_;
	}

	void handleEventAxis(SDL_ControllerAxisEvent* event, int16_t deadzone);

	void handleEventButton(SDL_ControllerButtonEvent* event, int16_t rollSpeed);
};


class Controller
{
	Axises mainAxises_;
	std::unordered_map<int32_t, Joystick> joysticks_;
	int16_t deadzone_;

	int16_t rollSpeed_;

	std::mutex axisMutex_;
	std::mutex buttonMutex_;
	std::mutex getMutex_;

	Controller();
	Controller(const Controller&) = delete;
	Controller& operator=(const Controller&) = delete;
	~Controller();

	static int watcherEventAxis(void*, SDL_Event* event);
	static int watcherEventButton(void*, SDL_Event* event);
	static int watcherEventDevicesUpdate(void*, SDL_Event* event);

	void handleEventAxis(SDL_ControllerAxisEvent* event);
	void handleEventButton(SDL_ControllerButtonEvent* event);
	void handleEventDevicesUpdate(SDL_ControllerDeviceEvent* event);

	bool axisesUpdated_;
	void updateAxises();

public:
	static Controller& getInstance()
	{
		static Controller controller;
		return controller;
	}

	
	const Axises& axises()
	{
		std::lock_guard lock(getMutex_);
		updateAxises();
		return mainAxises_;
	}
	
	
	void setDeadZone(int deadzone) { deadzone_ = deadzone; }
	int getDeadZone() const { return deadzone_; }
	void setRollSpeed(int32_t speed);
	int32_t getRollSpeed();
};

#endif