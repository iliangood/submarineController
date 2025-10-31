#include <iostream>

#include <SDL.h>

#include <udptransmitter.h>


SDL_GameController* findController() 
{
	for (int i = 0; i < SDL_NumJoysticks(); ++i) 
	{
		if (SDL_IsGameController(i)) 
		{
			return SDL_GameControllerOpen(i);
		}
	}

	return nullptr;
}

int main()
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0) 
	{
		std::cerr << "SDL could not initialize! SDL Error: " << SDL_GetError() << std::endl;
		return 1;
	}

	SDL_GameController* controller = nullptr;
	while(controller == nullptr)
	{
		controller = findController();
	}

	
	return 0;
}