
#define GL3W_IMPLEMENTATION
#include <gl3w.h>

// stlib
#include <chrono>

// internal
#include "physics_system.hpp"
#include "render_system.hpp"
#include "world_system.hpp"

#include "ai_system.hpp"


using Clock = std::chrono::high_resolution_clock;

// Entry point
int main()
{
	// Global systems
	WorldSystem world;
	RenderSystem renderer;
	PhysicsSystem physics;
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
	world.init(&renderer);


	Vertex* latest;
	Vertex* v900;
	// Generate vertices for first row
	for (int i = 0; i < 1051; i += 50) {
		Vertex* v = new Vertex(i, 622);
		graph.addVertex(v);
		if (i != 0) {
			graph.addEdge(v, latest, ACTION::WALK);
			graph.addEdge(latest, v, ACTION::WALK);
		}
		if (i == 900) {
			v900 = v;
		}
		latest = v;
	}
	Vertex* first_mid_plat = new Vertex(950, 522);
	graph.addVertex(first_mid_plat);
	graph.addEdge(v900, first_mid_plat, ACTION::JUMP);
	graph.addEdge(first_mid_plat, v900, ACTION::WALK);
	latest = first_mid_plat;

	Vertex* second_plat = new Vertex(830, 422);
	graph.addVertex(second_plat);
	graph.addEdge(second_plat, first_mid_plat, ACTION::JUMP);
	graph.addEdge(latest, second_plat, ACTION::JUMP);
	latest = second_plat;

	for (int i = 800; i >= 0; i -= 50) {
		Vertex* v = new Vertex(i, 422);
		graph.addVertex(v);
		graph.addEdge(v, latest, ACTION::WALK);
		graph.addEdge(latest, v, ACTION::WALK);
		latest = v;
	}
	std::string font_filename = "../data/fonts/Kenney_Pixel_Square.ttf";
	unsigned int font_default_size = 48;
	// renderer.fontInit(*window, font_filename, font_default_size);

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
		physics.step(elapsed_ms);
		ai.step(elapsed_ms);
		world.handle_collisions();
		
		renderer.draw();

		// renderer.renderText("test", -0.6f, 0.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f), trans);

	}

	return EXIT_SUCCESS;
}
