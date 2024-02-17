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

	return x > left_b && x < right_b && y > top_b && y < bot_b;
}

bool collides(const Motion& motion1, const Motion& motion2, float step_secs, DIRECTION dire = DIRECTION::ALL)
{
	vec2 pos1 = motion1.position;
	vec2 pos2 = motion2.position;
	vec2 scale1 = motion1.scale;
	vec2 scale2 = motion2.scale;
	vec2 vel1 = motion1.velocity;
	vec2 vel2 = motion2.velocity;

	//float left_b1 = pos1.x - abs(scale1.x) / 2;
	//float right_b1 = pos1.x + abs(scale1.x) / 2;
	//float left_b2 = pos2.x - abs(scale2.x) / 2;
	//float right_b2 = pos2.x + abs(scale2.x) / 2;

	//float top_b1 = pos1.y - abs(scale1.y) / 2;
	//float bot_b1 = pos1.y + abs(scale1.y) / 2;
	//float top_b2 = pos2.y - abs(scale2.y) / 2;
	//float bot_b2 = pos2.y + abs(scale2.y) / 2;

	float left_b1	= pos1.x - abs(scale1.x) / 2 + vel1.x * step_secs;
	float right_b1	= pos1.x + abs(scale1.x) / 2 + vel1.x * step_secs;
	float left_b2	= pos2.x - abs(scale2.x) / 2 + vel2.x * step_secs;
	float right_b2	= pos2.x + abs(scale2.x) / 2 + vel2.x * step_secs;

	float top_b1	= pos1.y - abs(scale1.y) / 2 + vel1.y * step_secs;
	float bot_b1	= pos1.y + abs(scale1.y) / 2 + vel1.y * step_secs;
	float top_b2	= pos2.y - abs(scale2.y) / 2 + vel2.y * step_secs;
	float bot_b2	= pos2.y + abs(scale2.y) / 2 + vel2.y * step_secs;

	vec2 top_left = { left_b1, top_b1};
	vec2 top_right = { right_b1, top_b1 };
	vec2 bot_left = { left_b1, bot_b1};
	vec2 bot_right = { right_b1, bot_b1};
	vec2 x_boundary = { left_b2, right_b2 };
	vec2 y_boundary = { top_b2, bot_b2 };

	if (dire == DIRECTION::RIGHT) {
		return	check_point_within_boundary(top_right, x_boundary, y_boundary) ||
				check_point_within_boundary(bot_right, x_boundary, y_boundary);
	}
	else if (dire == DIRECTION::TOP) {
		return	check_point_within_boundary(top_left, x_boundary, y_boundary) ||
				check_point_within_boundary(top_right, x_boundary, y_boundary);
	}
	else if (dire == DIRECTION::LEFT) {
		return	check_point_within_boundary(top_left, x_boundary, y_boundary) ||
				check_point_within_boundary(bot_left, x_boundary, y_boundary);
	}
	else if (dire == DIRECTION::BOT) {
		return	check_point_within_boundary(bot_left, x_boundary, y_boundary) ||
				check_point_within_boundary(bot_right, x_boundary, y_boundary);
	}
	else {
		return	check_point_within_boundary(top_left, x_boundary, y_boundary) ||
				check_point_within_boundary(top_right, x_boundary, y_boundary) ||
				check_point_within_boundary(bot_left, x_boundary, y_boundary) ||
				check_point_within_boundary(bot_right, x_boundary, y_boundary);
	}

}


void PhysicsSystem::step(float elapsed_ms)
{
	// Check gravity first so we can finalize yspeed
	float gravity = 30;
	float step_seconds = elapsed_ms / 1000.f;
	
	ComponentContainer<Gravity>& gravity_container = registry.gravities;
	for (uint i = 0; i < gravity_container.size(); i++)
	{
		Entity entity = gravity_container.entities[i];
		if (registry.motions.has(entity)) {
			Motion& motion = registry.motions.get(entity);
				motion.velocity[1] += gravity;
		}
	}
	// zombie unalerted turning 
	auto& zombie_registry = registry.zombies;
	for (int i = (int)zombie_registry.components.size() - 1; i >= 0; --i)
	{
		NormalZombie& zombie = zombie_registry.components[i];
		double xPosition = registry.motions.get(zombie_registry.entities[i]).position.x;
		// if zombie state == unalert (0), then check if it has reached the edge of its walking range and switch direction if so
		if (zombie.state == 0 && (xPosition <= zombie.walking_range[0] || xPosition >= zombie.walking_range[1])) {
			registry.motions.get(zombie_registry.entities[i]).velocity.x *= -1;
			registry.motions.get(zombie_registry.entities[i]).scale[0] *= -1;
		}
	}

	// Check for collisions
	ComponentContainer<Motion>& motion_container = registry.motions;
	ComponentContainer<Platform>& plat_container = registry.platforms;
	// Check for collisions between all moving entities and platforms
	for (uint i = 0; i < motion_container.components.size(); i++)
	{
		Motion& motion = motion_container.components[i];

		for (uint p = 0; p < plat_container.components.size(); p++)
		{
			Platform& plat = plat_container.components[p];
			Motion motion_2 = { plat.position, 0, {0,0}, plat.scale };

			if (collides(motion, motion_2, step_seconds, DIRECTION::TOP)) {
				motion.velocity.y = 0;
				motion.position.y = motion_2.position.y + abs(motion_2.scale.y) / 2 + abs(motion.scale.y) / 2;
			}
			if (collides(motion, motion_2, step_seconds, DIRECTION::BOT)) {
				motion.velocity.y = 0;
				motion.position.y = motion_2.position.y - abs(motion_2.scale.y) / 2 - abs(motion.scale.y) / 2;
			}
			if (collides(motion, motion_2, step_seconds, DIRECTION::LEFT)) {
				printf("colliding with left\n");
				motion.velocity.x = 0;
				motion.position.x = motion_2.position.x + abs(motion_2.scale.x) / 2 + abs(motion.scale.x) / 2;
			}
			if (collides(motion, motion_2, step_seconds, DIRECTION::RIGHT)) {
				motion.velocity.x = 0;
				motion.position.x = motion_2.position.x - abs(motion_2.scale.x) / 2 - abs(motion.scale.x) / 2;
			}

		}
	}
	// Check for collisions between all moving entities

	for (uint i = 0; i < motion_container.components.size(); i++)
	{
		Motion& motion_i = motion_container.components[i];
		Entity entity_i = motion_container.entities[i];

		// note starting j at i+1 to compare all (i,j) pairs only once (and to not compare with itself)
		for (uint j = i + 1; j < motion_container.components.size(); j++)
		{
			Motion& motion_j = motion_container.components[j];
			if (collides(motion_i, motion_j, step_seconds))
			{
				Entity entity_j = motion_container.entities[j];
				//if ((registry.players.has(entity_i) && registry.platforms.has(entity_j))||
				//	(registry.players.has(entity_j) && registry.platforms.has(entity_i))) {
				//	motion_i.velocity[1] = 0;
				//}
				// Create a collisions event
				// We are abusing the ECS system a bit in that we potentially insert muliple collisions for the same entity
				registry.collisions.emplace_with_duplicates(entity_i, entity_j);
				registry.collisions.emplace_with_duplicates(entity_j, entity_i);
			}
		}
	}

	// Check boundaries
	auto& motion_registry = registry.motions;
	for (uint i = 0; i < motion_registry.size(); i++) {
		Motion& motion = motion_registry.components[i];
		if ((motion.position.x - abs(motion.scale.x) / 2) < 0) {
			//printf("Colliding with left boundary\n");
			motion.velocity.x = 0;
			motion.position.x = abs(motion.scale.x) / 2;
		}
		if ((motion.position.x + abs(motion.scale.x) / 2) > window_width_px) {
			motion.velocity.x = 0;
			motion.position.x = window_width_px - abs(motion.scale.x) / 2;
			//printf("Colliding with right boundary\n");
		}
		if ((motion.position.y - abs(motion.scale.y) / 2) < 0) {
			motion.velocity.y = 0;
			motion.position.y = abs(motion.scale.y) / 2;
			//printf("Colliding with top boundary\n");
		}
		if ((motion.position.y + abs(motion.scale.y) / 2) > window_height_px) {
			motion.velocity.y = 0;
			motion.position.y = window_height_px - abs(motion.scale.y) / 2;
			//printf("%f\n", motion.position.y + abs(motion.scale.y) / 2);
			//printf("Colliding with bot boundary\n");
		}
	}

	// Move bug based on how much time has passed, this is to (partially) avoid
	// having entities move at different speed based on the machine.
	for(uint i = 0; i< motion_registry.size(); i++)
	{
		// !!! TODO A1: update motion.position based on step_seconds and motion.velocity
		Motion& motion = motion_registry.components[i];
		Entity entity = motion_registry.entities[i];

		//(void)elapsed_ms; // placeholder to silence unused warning until implemented
		motion.position[0] += motion.velocity[0] * step_seconds;
		motion.position[1] += motion.velocity[1] * step_seconds;

	}






}