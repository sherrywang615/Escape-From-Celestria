
#define GL3W_IMPLEMENTATION
#include <gl3w.h>

// stlib
#include <chrono>

// internal
#include "physics_system.hpp"
#include "render_system.hpp"
#include "world_system.hpp"
#include "dialog_system.hpp"

#include "ai_system.hpp"
#include <iostream>

using Clock = std::chrono::high_resolution_clock;



// Entry point
int main()
{
	// Global systems
	WorldSystem world;
	RenderSystem renderer;
	PhysicsSystem physics;
	DialogSystem dialog;
	AISystem ai;

	// Initializing window
	GLFWwindow* window = world.create_window();
	if (!window) {
		// Time to read the error message
		printf("Press any key to exit");
		getchar();
		return EXIT_FAILURE;
	}

	// initialize the main systems
	renderer.init(window);

	// initialize font
	std::string font_filename = font_path("Kenney_Future.ttf");
	unsigned int font_default_size = 48;
	renderer.fontInit(window, font_filename, font_default_size);
	world.init(&renderer,&dialog);
	

	// variable timestep loop
	auto t = Clock::now();
	while (!world.is_over()) {
		// Processes system messages, if this wasn't present the window would become unresponsive
		glfwPollEvents();

		// Calculating elapsed times in milliseconds from the previous iteration
		auto now = Clock::now();
		float elapsed_ms =
			(float)(std::chrono::duration_cast<std::chrono::microseconds>(now - t)).count() / 1000;
		t = now;


		world.step(elapsed_ms);
		if (!world.is_paused()) {
			physics.step(elapsed_ms);
			ai.step(elapsed_ms);
		}
		world.handle_collisions();
		
		renderer.draw();
		float ms_to_sleep = 1000 /60 - elapsed_ms;
	}

	return EXIT_SUCCESS;
}
