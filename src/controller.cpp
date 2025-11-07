#include "controller.h"

#include <iostream>

#include <SDL2/SDL.h>


Controller::Controller()
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
	for()
}