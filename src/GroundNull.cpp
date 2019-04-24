// internal
#include "GameObject.hpp"
#include <windows.h>
#include "world.hpp"
#include "resource_manager.hpp"


#define GL3W_IMPLEMENTATION
#include <gl3w.h>


// stlib
#include <chrono>
#include <iostream>
#include <algorithm>

using Clock = std::chrono::high_resolution_clock;

// Global 
World world;
const int width = 1920;
const int height = 1080;
const char* title = "Your Title Here";
static resource_manager ResourceManager;

// Entry point
int main(int argc, char* argv[])
{
	
	// Initializing world (after renderer.init().. sorry)
	if (!world.init({ (float)width, (float)height }, &ResourceManager))
	{
		// Time to read the error message
		std::cout << "Press any key to exit" << std::endl;
		std::cin.get();
		return EXIT_FAILURE;
	}

	auto t = Clock::now();
	srand(time(NULL));
	
	float fps_sec_countdown = 1000.f;
	float dt = 17.f;
	int fps = 0;
	int total_fps = 0;
	float accumulator = 0;

	
	// variable timestep loop.. can be improved (:
	while (!world.is_over())
	{
		// Processes system messages, if this wasn't present the window would become unresponsive
		glfwPollEvents();

		// Calculating elapsed times in milliseconds from the previous iteration
		auto now = Clock::now();
		float elapsed_sec = (float)(std::chrono::duration_cast<std::chrono::microseconds>(now - t)).count() / 1000;
		fps_sec_countdown -= elapsed_sec;
		t = now;
		accumulator += elapsed_sec;

		while (accumulator > dt)
		{
			world.update(dt, total_fps);
			world.update_allphysics(dt);
			accumulator -= dt;
		}

		world.draw();
		if (fps_sec_countdown > 0) {
			fps++;
		}
		else {
			total_fps = fps;
			fps = 0;
			fps_sec_countdown = 1000.f;
		}
	}

	world.destroy();

	return EXIT_SUCCESS;
}