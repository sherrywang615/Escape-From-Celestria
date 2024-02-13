// internal
#include "physics_system.hpp"
#include "world_init.hpp"

// Returns the local bounding coordinates scaled by the current size of the entity
vec2 get_bounding_box(const Motion& motion)
{
	// abs is to avoid negative scale due to the facing direction.
	return { abs(motion.scale.x), abs(motion.scale.y) };
}

// Helper function: return true if point is within the boundary
bool check_point_within_boundary(vec2 point, vec2 x_boundary, vec2 y_boundary)
{
	float x = point[0];
	float y = point[1];
	float left_b = x_boundary[0];
	float right_b = x_boundary[1];
	float top_b = y_boundary[0];
	float bot_b = y_boundary[1];

	return x >= left_b && x <= right_b && y >= top_b && y <= bot_b;
}

bool collides(const Motion& motion1, const Motion& motion2)
{
	vec2 pos1 = motion1.position;
	vec2 pos2 = motion2.position;
	vec2 scale1 = motion1.scale;
	vec2 scale2 = motion2.scale;

	float left_b1 = pos1.x - abs(scale1.x) / 2;
	float right_b1 = pos1.x + abs(scale1.x) / 2;
	float left_b2 = pos2.x - abs(scale2.x) / 2;
	float right_b2 = pos2.x + abs(scale2.x) / 2;

	float top_b1 = pos1.y - abs(scale1.y) / 2;
	float bot_b1 = pos1.y + abs(scale1.y) / 2;
	float top_b2 = pos2.y - abs(scale2.y) / 2;
	float bot_b2 = pos2.y + abs(scale2.y) / 2;

	vec2 top_left = { left_b1, top_b1 };
	vec2 top_right = { right_b1, top_b1 };
	vec2 bot_left = { left_b1, bot_b1 };
	vec2 bot_right = { right_b1, bot_b1 };
	vec2 x_boundary = { left_b2, right_b2 };
	vec2 y_boundary = { top_b2, bot_b2 };
	return check_point_within_boundary(top_left, x_boundary, y_boundary) ||
		check_point_within_boundary(top_right, x_boundary, y_boundary) ||
		check_point_within_boundary(bot_left, x_boundary, y_boundary) ||
		check_point_within_boundary(bot_right, x_boundary, y_boundary);
}

void PhysicsSystem::step(float elapsed_ms)
{
	// Move bug based on how much time has passed, this is to (partially) avoid
	// having entities move at different speed based on the machine.
	auto& motion_registry = registry.motions;
	for(uint i = 0; i< motion_registry.size(); i++)
	{
		// !!! TODO A1: update motion.position based on step_seconds and motion.velocity
		Motion& motion = motion_registry.components[i];
		Entity entity = motion_registry.entities[i];
		float step_seconds = elapsed_ms / 1000.f;
		//(void)elapsed_ms; // placeholder to silence unused warning until implemented
		motion.position[0] += motion.velocity[0] * step_seconds;
		motion.position[1] += motion.velocity[1] * step_seconds;

	}

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// TODO A2: HANDLE EGG UPDATES HERE
	// DON'T WORRY ABOUT THIS UNTIL ASSIGNMENT 2
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	// Check for collisions between all moving entities
    ComponentContainer<Motion> &motion_container = registry.motions;
	for(uint i = 0; i<motion_container.components.size(); i++)
	{
		Motion& motion_i = motion_container.components[i];
		Entity entity_i = motion_container.entities[i];
		
		// note starting j at i+1 to compare all (i,j) pairs only once (and to not compare with itself)
		for(uint j = i+1; j<motion_container.components.size(); j++)
		{
			Motion& motion_j = motion_container.components[j];
			if (collides(motion_i, motion_j))
			{
				Entity entity_j = motion_container.entities[j];
				// Create a collisions event
				// We are abusing the ECS system a bit in that we potentially insert muliple collisions for the same entity
				registry.collisions.emplace_with_duplicates(entity_i, entity_j);
				registry.collisions.emplace_with_duplicates(entity_j, entity_i);
			}
		}
	}
	

	// Momentum Implementation
	int friction = 100;   // The higher this value is, the longer the chicken can slide.
	ComponentContainer<Sliding>& sliding_container = registry.slidings;
	for (uint i = 0; i < sliding_container.size(); i++)
	{
		Entity entity = sliding_container.entities[i];
		Sliding sliding = sliding_container.components[i];
		if (!registry.deathTimers.has(entity)) {
			Motion& motion = registry.motions.get(entity);
			if (motion.velocity[0] < -10 || motion.velocity[0] > 10) {
				motion.velocity[0] -= sliding.velocity[0] / friction;
			}
			else {
				motion.velocity[0] = 0;
			}
			if (motion.velocity[1] < -10 || motion.velocity[1] > 10) {
				motion.velocity[1] -= sliding.velocity[1] / friction;
			}
			else {
				motion.velocity[1] = 0;
			}

		}
	}

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// TODO A2: HANDLE EGG collisions HERE
	// DON'T WORRY ABOUT THIS UNTIL ASSIGNMENT 2
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
}