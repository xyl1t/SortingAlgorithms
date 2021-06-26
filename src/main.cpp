#include <iostream>
#if __has_include("SDL2/SDL.h")
# include <SDL2/SDL.h>
#else
# include <SDL.h>
#endif
#define CIDR_IMPLEMENTATION
#include "cidr.hpp"
#include "eventHandler.hpp"
#include "display.hpp"
using namespace cdr;

int main(int argc, char** argv) {
	SDL_Init(SDL_INIT_EVERYTHING);
	
	constexpr float zoom = 1.f;
	constexpr int DISPLAY_WIDTH = 256*zoom;
	constexpr int DISPLAY_HEIGHT = 240*zoom;
	Display display{DISPLAY_WIDTH, DISPLAY_HEIGHT, "Sorting Algorithm Visualisation", true, zoom, zoom};
	Renderer renderer {display.GetPixels(), DISPLAY_WIDTH, DISPLAY_HEIGHT};
	EventHandler& eh = EventHandler::GetInstance();
	float currentTime = 0.f, previousTime = 0.f, delta = 0.f;
	int timer = 0;
	
	while(!display.IsClosed()) {
		currentTime = SDL_GetTicks(); delta = currentTime - previousTime; previousTime = currentTime;
		if(currentTime / 1000 > timer) { std::clog << "FPS: " << 1000.f / delta  << std::endl; timer++; }
		delta /= 1000;
		
		
		
		display.Update();
	}
	
	SDL_Quit();
	
	return 0;
}
