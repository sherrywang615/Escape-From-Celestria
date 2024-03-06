
#define GL3W_IMPLEMENTATION
#include <gl3w.h>

// stlib
#include <chrono>

// internal
#include "physics_system.hpp"
#include "render_system.hpp"
#include "world_system.hpp"

#include "ai_system.hpp"
#include <windows.h>
#include <iostream>

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


void createVerticesForLevel1() {
	VecVertice vv;
	vv = createVerticesForAPlatform({ 0, 1050 }, 644, 900, 100);
	Vertex* first_mid_plat = new Vertex(950, 544);
	graph.addVertex(first_mid_plat);
	graph.addEdge(vv.head, first_mid_plat, ACTION::JUMP);
	graph.addEdge(first_mid_plat, vv.head, ACTION::WALK);

	Vertex* second_plat = new Vertex(830, 444);
	graph.addVertex(second_plat);
	graph.addEdge(second_plat, first_mid_plat, ACTION::WALK);
	graph.addEdge(first_mid_plat, second_plat, ACTION::JUMP);
	vv = createVerticesForAPlatform({ 0, 800 }, 444, 100, 800);
	graph.addEdge(vv.tail, second_plat, ACTION::WALK);
	graph.addEdge(second_plat, vv.tail, ACTION::WALK);
	Vertex* second_mid_plat = new Vertex(60, 344);
	graph.addVertex(second_mid_plat);
	graph.addEdge(second_mid_plat, vv.head, ACTION::WALK);
	graph.addEdge(vv.head, second_mid_plat, ACTION::JUMP);
	vv = createVerticesForAPlatform({ 210, 1060 }, 244, 210, 1060);
	graph.addEdge(vv.head, second_mid_plat, ACTION::WALK);
	graph.addEdge(second_mid_plat, vv.head, ACTION::JUMP);
}

void createVerticesForLevel2() {
	VecVertice vv;
	vv = createVerticesForAPlatform({ 0, 1050 }, 644, 900, 100);
	Vertex* first_mid_plat = new Vertex(50, 544);
	graph.addVertex(first_mid_plat);
	graph.addEdge(first_mid_plat, vv.tail, ACTION::WALK);
	graph.addEdge(vv.tail, first_mid_plat, ACTION::JUMP);
	vv = createVerticesForAPlatform({ 170, 520 }, 444, 170, 520);
	graph.addEdge(first_mid_plat, vv.head, ACTION::JUMP);
	graph.addEdge(vv.head, first_mid_plat, ACTION::WALK);
	VecVertice vv2 = createVerticesForAPlatform({ 550, 650 }, 444, 550, 650);
	graph.addEdge(vv2.head, vv.tail, ACTION::WALK);
	graph.addEdge(vv.tail, vv.head, ACTION::WALK);
	VecVertice vvRight = createVerticesForAPlatform({ 720, 970 }, 444, 720, 970);
	graph.addEdge(vv2.tail, vvRight.head, ACTION::WALK);
	graph.addEdge(vvRight.head, vv2.tail, ACTION::WALK);
	VecVertice vvUp = createVerticesForAPlatform({ 550, 650 }, 344, 550, 650);
	graph.addEdge(vv.tail, vvUp.head, ACTION::JUMP);
	graph.addEdge(vvUp.head, vv.tail, ACTION::WALK);
	graph.addEdge(vvRight.head, vvUp.tail, ACTION::JUMP);
	graph.addEdge(vvUp.tail, vvRight.head, ACTION::WALK);
	Vertex* second_mid_plat = new Vertex(70, 344);
	VecVertice vvLeft3 = createVerticesForAPlatform({ 170, 420 }, 244, 170, 420);
	graph.addEdge(vvUp.head, vvLeft3.tail, ACTION::JUMP);
	graph.addEdge(vvLeft3.tail, vvUp.head, ACTION::WALK);
	graph.addEdge(second_mid_plat, vvLeft3.head, ACTION::JUMP);
	graph.addEdge(vvLeft3.head, second_mid_plat, ACTION::WALK);
	VecVertice vvRight3 = createVerticesForAPlatform({ 770, 970 }, 244, 770, 970);
	graph.addEdge(vvUp.tail, vvRight3.head, ACTION::JUMP);
	graph.addEdge(vvRight3.head, vvUp.tail, ACTION::WALK);
	Vertex* third_mid_plat = new Vertex(70, 144);
	VecVertice vv3 = createVerticesForAPlatform({ 170, 970 }, 44, 170, 970);
	graph.addEdge(vvLeft3.head, third_mid_plat, ACTION::JUMP);
	graph.addEdge(third_mid_plat, vvLeft3.head, ACTION::WALK);
	graph.addEdge(third_mid_plat, vv3.head, ACTION::JUMP);
	graph.addEdge(vv3.head, third_mid_plat, ACTION::WALK);


	
}

void createGraph(int level) {
	graph = Graph();
	if (level == 1) {
		createVerticesForLevel1();
	}
	else if (level == 2) {
		createVerticesForLevel2();
	}
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

	int level = world.currentLevel;
	createVerticesForLevel1();


	//graph.saveGraph(GRAPH_PATH + "level1.txt");
	//graph.loadFromFile(GRAPH_PATH + "level1.txt");
	

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
		//printf("%d\n", world.currentLevel);
		if (world.currentLevel != level) {
			level = world.currentLevel;
			createGraph(level);
		}

		world.step(elapsed_ms);
		physics.step(elapsed_ms);
		ai.step(elapsed_ms);
		world.handle_collisions();
		
		renderer.draw();
		float ms_to_sleep = 1000 /60 - elapsed_ms;
		//std::cout << ms_to_sleep << std::endl;
		if (ms_to_sleep > 0) {
			Sleep((ms_to_sleep));
		}
		
		// renderer.renderText("test", -0.6f, 0.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f), trans);

	}

	return EXIT_SUCCESS;
}
