#pragma once

#include <vector>

#include "tiny_ecs_registry.hpp"
#include "common.hpp"

class AISystem
{
private:
	std::chrono::system_clock::time_point start;
public:
	AISystem();
	void step(float elapsed_ms);
};