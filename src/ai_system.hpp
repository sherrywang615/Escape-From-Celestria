#pragma once

#include <vector>

#include "tiny_ecs_registry.hpp"
#include "common.hpp"


class AISystem
{
private:
	std::chrono::system_clock::time_point start;
	std::queue<Vertex*> prev_path = {};

	void updateZombiePath(float elapsed_ms, int elapsed);

public:
	AISystem();
	~AISystem();
	void step(float elapsed_ms);
};