#include <iostream>
#include <SDL2/SDL.h>
#include <udptransmitter.h>


int main() {
    if(SDL_Init(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER) < 0)
	{
		std::cerr << "SDL init failed: " << SDL_GetError();
		return 0;
	}
	
	
    return 0;
}