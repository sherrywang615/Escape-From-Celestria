// internal
#include "ai_system.hpp"

int X_frame = 1;


float findDistanceBetween(Motion motion_1, Motion motion_2) {
	vec2 pos1 = motion_1.position;
	vec2 pos2 = motion_2.position;
	float dist = pow((pos1.x - pos2.x), 2) + pow((pos1.y - pos2.y), 2);
	return dist;
}

void findPath(Motion motion_1, Motion motion_2) {

}

void updateZombiePath(float elapsed_ms) 
{
	float memory = 3000.f;
	for (Entity entity_z : registry.zombies.entities) {
		NormalZombie& zombie = registry.zombies.get(entity_z);
		Motion& motion_z = registry.motions.get(entity_z);
		for (Entity entity_p : registry.players.entities) {
			Motion& motion_p = registry.motions.get(entity_p);
			float dist = findDistanceBetween(motion_z, motion_p);
			// chase player if player is within sensing range and zombie is facing the plaeyr
			if (dist <= zombie.sensing_range) {
				if (zombie.face == DIRECTION::RIGHT && (motion_p.position.x > motion_z.position.x)) {
					zombie.is_alerted = true;
					zombie.memory = memory;
				}
				if (zombie.face == DIRECTION::LEFT && (motion_p.position.x < motion_z.position.x)) {
					zombie.is_alerted = true;
					zombie.memory = memory;
				}
			}
			else if (zombie.is_alerted) {
				zombie.memory -= elapsed_ms;
				if (zombie.memory < 0) {
					zombie.is_alerted = false;
				}
			}
			double xPosition = motion_z.position.x;
			// if zombie state == unalert (0), then check if it has reached the edge of its walking range and switch direction if so
			if (!zombie.is_alerted && (xPosition <= zombie.walking_range[0] || xPosition >= zombie.walking_range[1])) {
				motion_z.velocity.x *= -1;
				motion_z.scale[0] *= -1;
			}

			// change zombie's facing direction
			if (motion_z.velocity.x > 0) {
				zombie.face = DIRECTION::RIGHT;
			}
			else {
				zombie.face = DIRECTION::LEFT;
			}
		}
	}
}

void AISystem::step(float elapsed_ms)
{
	int target_ms = X_frame * 1000 / 60;
	if (elapsed_ms >= target_ms) {
		updateZombiePath(elapsed_ms);
	}
}