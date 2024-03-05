
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

VecVertice createVerticesForAPlatform(vec2 range, float y, int head, int tail) {
	Vertex* latest;
	Vertex* vHead;
	Vertex* vTail;
	for (int i = range[0]; i <= range[1]; i += 50) {
		Vertex* v = new Vertex(i, y);
		graph.addVertex(v);
		if (i != range[0]) {
			graph.addEdge(v, latest, ACTION::WALK);
			graph.addEdge(latest, v, ACTION::WALK);
		}
		if (i == head) {
			vHead = v;
		}
		if (i == tail) {
			vTail = v;
		}
		latest = v;
	}
	VecVertice result = { vHead, vTail };
	return result;
}


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

	VecVertice vv;
	vv = createVerticesForAPlatform({ 0, 1050 }, 644, 900, 100);
	Vertex* first_mid_plat = new Vertex(950, 544);
	graph.addVertex(first_mid_plat);
	graph.addEdge(vv.head, first_mid_plat, ACTION::JUMP);
	graph.addEdge(first_mid_plat, vv.head, ACTION::WALK);

	Vertex* second_plat = new Vertex(830, 444);
	graph.addVertex(second_plat);
	graph.addEdge(second_plat, first_mid_plat, ACTION::JUMP);
	graph.addEdge(first_mid_plat, second_plat, ACTION::JUMP);
	vv = createVerticesForAPlatform({ 0, 800 }, 444, 100, 800);
	Vertex* second_mid_plat = new Vertex(60, 344);
	graph.addVertex(second_mid_plat);
	graph.addEdge(second_mid_plat, vv.head, ACTION::WALK);
	graph.addEdge(vv.head, second_mid_plat, ACTION::JUMP);
	vv = createVerticesForAPlatform({ 210, 1060 }, 244, 210, 1060);
	graph.addEdge(vv.head, second_mid_plat, ACTION::WALK);
	graph.addEdge(second_mid_plat, vv.head, ACTION::JUMP);


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
