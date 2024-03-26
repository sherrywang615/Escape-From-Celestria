// internal
#include "physics_system.hpp"
#include "world_init.hpp"
#include <iostream>

vec2 previous_position = {};

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

bool check_in_range(vec2 point1, vec2 point2, vec2 target)
{
	if ((target.x <= max(point1.x, point2.x) && target.x >= min(point1.x, point2.x)) && (target.y <= max(point1.y, point2.y) && target.y >= min(point1.y, point2.y))) 
	{
		return true;
	}
	return false;
}


// Helper function to check if a line intersects another line
bool check_line_intersects(vec2 point1, vec2 point2, vec2 point3, vec2 point4)
{
	float intersectx = NULL;
	float intersecty = NULL;

	if ((point2.x - point1.x) == 0) {
		intersectx = point2.x;
	}
	else if ((point4.x - point3.x) == 0) {
		intersectx = point4.x;
	}
	if ((point2.y - point1.y) == 0) {
		intersecty = point2.y;
	}
	else if ((point4.y - point3.y) == 0) {
		intersecty = point2.y;
	}

	if (intersectx != NULL && intersecty != NULL) {
		if (check_in_range(point1, point2, {intersectx, intersecty}) && check_in_range(point3, point4, { intersectx, intersecty })) {
			return true;
		}
	}

	if (intersectx != NULL) {
		if (intersectx == point4.x) {
			float slope1 = (point2.y - point1.y) / (point2.x - point1.x);
			float intercept1 = point2.y - (slope1 * point2.x);
			intersecty = slope1 * point4.x + intercept1;
		} 
		else 
		{
			float slope2 = (point4.y - point3.y) / (point4.x - point3.x);
			float intercept2 = point4.y - (slope2 * point4.x);
			intersecty = slope2 * point2.x + intercept2;
		}
		if (check_in_range(point1, point2, { intersectx, intersecty }) && check_in_range(point3, point4, { intersectx, intersecty })) {
			return true;
		}
		else 
		{
			return false;
		}
	}

	if (intersecty != NULL) {
		if (intersecty == point4.y) {
			float slope1 = (point2.y - point1.y) / (point2.x - point1.x);
			float intercept1 = point2.y - (slope1 * point2.x);
			intersectx =  (point4.y - intercept1)/slope1;
		}
		else
		{
			float slope2 = (point4.y - point3.y) / (point4.x - point3.x);
			float intercept2 = point4.y - (slope2 * point4.x);
			intersectx = (point2.y - intercept2)/ slope2;
		}
		if (check_in_range(point1, point2, { intersectx, intersecty }) && check_in_range(point3, point4, { intersectx, intersecty })) {
			return true;
		}
		else
		{
			return false;
		}
	}


	float slope1 = (point2.y - point1.y) / (point2.x - point1.x);
	float slope2 = (point4.y - point3.y) / (point4.x - point3.x);
	float intercept1 = point2.y - (slope1 * point2.x);
	float intercept2 = point4.y - (slope2 * point4.x);
	
	if ((point2.y - point1.y == 0) || (point2.x - point1.x) == 0 || slope2 != slope1)
	{
		intersectx = (intercept2 - intercept1) / (slope1 - slope2);
		intersecty = slope2 * intersectx + intercept2;
		if (check_in_range(point1, point2, { intersectx, intersecty }) && check_in_range(point3, point4, { intersectx, intersecty })) {
			return true;
		}
	}
	else {
		if (intercept1 == intercept2) {
			return true;
		}
	}
	return false;
}

void collision_resolve(Motion& motion, vec2 prev_pos, std::vector<int> dir, Motion& motion2)
{

	if (dir[0] == 1) {
		motion.velocity.y = 0;
		motion.position.y = prev_pos.y;
	}
	if (dir[1] == 1) {
		motion.velocity.y = 0;
		motion.position.y = prev_pos.y;
	}

	if ((dir[2] == 1 || dir[3] == 1)) {
		motion.velocity.x = 0;
		motion.position.x = prev_pos.x;
	}

	
}
// checks mesh based collision between player mesh and other objects
std::vector<int> collides_with_mesh(const Motion& motion, const Motion& mesh_motion, float step_secs, const Mesh& meshPtrs) {
	vec2 pos1 = motion.position;
	vec2 scale1 = motion.scale;
	vec2 vel1 = motion.velocity;
	
	//player velocity and scale
	vec2 pscale = mesh_motion.scale;
	vec2 pvel = mesh_motion.velocity;
	

	float left_b1 = pos1.x - abs(scale1.x) / 2 ;
	float right_b1 = pos1.x + abs(scale1.x) / 2;
	float top_b1 = pos1.y - abs(scale1.y) / 2 ;
	float bot_b1 = pos1.y + abs(scale1.y) / 2 ;
	
	vec2 top_left = { left_b1, top_b1};
	vec2 top_right = { right_b1, top_b1};
	vec2 bot_left = { left_b1, bot_b1 };
	vec2 bot_right = { right_b1, bot_b1 };

	double meshPosX = mesh_motion.position.x;
	double meshPosY = mesh_motion.position.y;

	auto& vertices = meshPtrs.vertices;
	std::vector<int> collision_dirs = {0, 0, 0, 0, 0};
	for (uint i = 0; i < vertices.size() - 1; i++)
	{
		double currX = meshPosX - vertices[i].position.x * (pscale.x) ;
		double currY = meshPosY - vertices[i].position.y * (pscale.y) ;
		double nextX = meshPosX - vertices[i + 1].position.x * (pscale.x) ;
		double nextY = meshPosY - vertices[i + 1].position.y * (pscale.y) ;
		
		if (check_point_within_boundary({ currX, currY }, { left_b1, right_b1 }, { top_b1, bot_b1 }) ||
			check_point_within_boundary({ nextX, nextY }, { left_b1, right_b1 }, { top_b1, bot_b1 })) {
			//bot
			if (check_line_intersects({top_left.x, top_left.y}, { top_right.x, top_right.y }, { currX, currY }, { nextX, nextY }))
			{
				collision_dirs[0] = 1;
				collision_dirs[4] = 1;
			}
			//top
			if (check_line_intersects(bot_left, bot_right, { currX, currY }, { nextX, nextY }))
			{
				collision_dirs[1] = 1;
				collision_dirs[4] = 1;
			}
			//right
			if (check_line_intersects({ top_right.x, top_right.y +1}, { bot_right.x, bot_right.y - 1 }, { currX, currY }, { nextX, nextY }))
			{
				collision_dirs[2] = 1;
				collision_dirs[4] = 1;
			}
			//left
			if (check_line_intersects({ top_left.x, top_right.y +1}, { bot_left.x, bot_left.y - 1 }, { currX, currY }, { nextX, nextY }))
			{
				collision_dirs[3] = 1;
				collision_dirs[4] = 1;
			}
		}

	}
	
	return collision_dirs;
}

bool collides(const Motion& motion1, const Motion& motion2, float step_secs, DIRECTION dire = DIRECTION::ALL)
{
	vec2 pos1 = motion1.position;
	vec2 pos2 = motion2.position;
	vec2 scale1 = motion1.scale;
	vec2 scale2 = motion2.scale;
	vec2 vel1 = motion1.velocity;
	vec2 vel2 = motion2.velocity;

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

	vec2 left = { left_b1, (top_b1 + bot_b1) / 2 };
	vec2 right = { right_b1, (top_b1 + bot_b1) / 2 };
	vec2 top = {(left_b1 + right_b1)/2, top_b1};
	vec2 bot = {(left_b1 + right_b1)/2, bot_b1};

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
		return	check_point_within_boundary(left, x_boundary, y_boundary) ||
				check_point_within_boundary(top, x_boundary, y_boundary) ||
				check_point_within_boundary(right, x_boundary, y_boundary) ||
				check_point_within_boundary(bot, x_boundary, y_boundary);
	}



}


void PhysicsSystem::step(float elapsed_ms)
{
	//previous position before moving
	auto& players = registry.players;
	for (uint i = 0; i < players.size(); i++) {
		previous_position = registry.motions.get(players.entities[i]).position;
	}

	// ------------------- Gravity system -------------------------------------
	// Check gravity first so we can finalize yspeed
	float gravity = 30;
	float step_seconds = elapsed_ms / 1000.f;
	ComponentContainer<Gravity>& gravity_container = registry.gravities;
	for (uint i = 0; i < gravity_container.size(); i++)
	{
		Entity entity = gravity_container.entities[i];
		if (!registry.players.has(entity)) {
			if (registry.motions.has(entity)) {
				Motion& motion = registry.motions.get(entity);
				motion.velocity[1] += gravity;
			}
		}
		else {
			if (registry.players.get(entity).standing == false) {
				Motion& motion = registry.motions.get(entity);
				motion.velocity[1] += gravity;
			}
		}

	}

	
	ComponentContainer<Motion>& motion_container = registry.motions;
	ComponentContainer<Platform>& plat_container = registry.platforms;

	// -------------------------- Step motion objects --------------------------
	// having entities move at different speed based on the machine.
	for (uint i = 0; i < motion_container.size(); i++)
	{
		Motion& motion = motion_container.components[i];
		Entity entity = motion_container.entities[i];
		motion.position[0] += motion.velocity[0] * step_seconds;
		motion.position[1] += motion.velocity[1] * step_seconds;


		//remove bullet
		if (registry.bullets.has(entity) && !registry.eatables.has(entity)) {

			//registry.collisions.emplace_with_duplicates(entity, entity);
		}

	}

	// ------------------------------- Debugging ---------------------------------
	
	if (debugging.in_debug_mode == true) {
		vec2 player_pos = {};
		// debugging box for player
		for (int i = 0; i < registry.players.size(); i++) {
			player_pos = registry.motions.get(registry.players.entities[i]).position;

			//testing for mesh collision detection
			auto& vertices = registry.meshPtrs.get(registry.players.entities[i])->vertices;
			for (uint j = 0; j < vertices.size() - 3; j++)
			{
				double currX = player_pos.x - vertices[j].position.x * (registry.motions.get(registry.players.entities[i]).scale.x);
				double currY = player_pos.y - vertices[j].position.y * (registry.motions.get(registry.players.entities[i]).scale.y);
				double nextX = player_pos.x - vertices[j + 1].position.x * (registry.motions.get(registry.players.entities[i]).scale.x);
				double nextY = player_pos.y - vertices[j + 1].position.y * (registry.motions.get(registry.players.entities[i]).scale.y);

				auto newline = createLine({ currX , currY }, { 3, 3 });

			}

		}
		// debugging box for zombies
		auto& zombies = registry.zombies;
		for (int i = 0; i < zombies.size(); i++)
		{
			if (registry.motions.has(zombies.entities[i])) {
				Motion& motion = registry.motions.get(zombies.entities[i]);
				float xScale1 = abs(motion.scale.x);
				float yScale1 = abs(motion.scale.y);

				vec2 topmid1 = { motion.position.x, motion.position.y - yScale1 / 2 };
				vec2 leftmid1 = { motion.position.x - xScale1 / 2 , motion.position.y };
				vec2 rightmid1 = { motion.position.x + xScale1 / 2 ,motion.position.y };
				vec2 botmid1 = { motion.position.x, motion.position.y + yScale1 / 2 };

				auto lineTop = createLine(topmid1, { xScale1, 2 });
				auto lineLeft = createLine(leftmid1, { 2, yScale1 });
				auto lineRight = createLine(rightmid1, { 2,yScale1 });
				auto lineBottom = createLine(botmid1, { xScale1, 2 });
			}
		}

	}

	// ---------------------------------- Collision checking ----------------------------------
	// Check for collisions between all moving entities and platforms
	for (uint i = 0; i < motion_container.size(); i++)
	{
		Motion& motion = motion_container.components[i];
		Entity& entity = motion_container.entities[i];
		// only check platform collision if current motion is not a platform
		if (!registry.platforms.has(registry.motions.entities[i])) {
			bool collide = false;
			for (uint p = 0; p < plat_container.size(); p++)
			{
				Platform& plat = plat_container.components[p];
				Motion motion_p = { plat.position, 0, {0,0}, plat.scale };
				if (abs(motion_p.position.x - motion.position.x) < 200 && abs(motion_p.position.y - motion.position.y) < 200 && !registry.golds.has(registry.motions.entities[i])) {
					// make collision checking more efficient, only check close platforms
					// mesh collision				
					if (registry.players.has(entity)) {
						Player& player = registry.players.get(entity);
						std::vector<int> collide_dir = collides_with_mesh(motion_p, motion, step_seconds, *registry.meshPtrs.get(motion_container.entities[i]));
						if (collide_dir[4] == 1) {
							collide = true;
							collision_resolve(motion, previous_position, collide_dir, motion_p);

							if (collide_dir[0] == 1) {
								player.standing = 1;
							}
							else {
								player.standing = 0;
							}
							if (collide_dir[2] || collide_dir[3]) {
								player.against_wall = 1;
							}
							else {
								player.against_wall = 0;
							}

						}
					} // non mesh collisions
					else
					{
						if (collides(motion, motion_p, step_seconds, DIRECTION::TOP)) {
							motion.velocity.y = 0;
							motion.position.y = motion_p.position.y + abs(motion_p.scale.y) / 2 + abs(motion.scale.y) / 2;
						}
						if (collides(motion, motion_p, step_seconds, DIRECTION::BOT)) {
							motion.velocity.y = 0;
							motion.position.y = motion_p.position.y - abs(motion_p.scale.y) / 2 - abs(motion.scale.y) / 2;
						}
						if (collides(motion, motion_p, step_seconds, DIRECTION::LEFT)) {

							motion.velocity.x = 0;
							motion.position.x = motion_p.position.x + abs(motion_p.scale.x) / 2 + abs(motion.scale.x) / 2;
						}
						if (collides(motion, motion_p, step_seconds, DIRECTION::RIGHT)) {
							motion.velocity.x = 0;
							motion.position.x = motion_p.position.x - abs(motion_p.scale.x) / 2 - abs(motion.scale.x) / 2;
						}
					}
				}
			}
			// ----------------------------- motion vs non platform collision checking ---------------------------------------
			// note starting j at i+1 to compare all (i,j) pairs only once (and to not compare with itself)
			for (uint j = i + 1; j < motion_container.components.size(); j++)
			{
				if (!registry.platforms.has(motion_container.entities[j])) {
					Motion& motion_j = motion_container.components[j];
					if (registry.players.has(entity))
					{
						if (collides_with_mesh(motion_j, motion, step_seconds, *registry.meshPtrs.get(motion_container.entities[i]))[4] == 1) {
							Entity entity_j = motion_container.entities[j];
							if ((!registry.collisions.has(entity) || !registry.collisions.has(entity_j))) {
								registry.collisions.emplace_with_duplicates(entity, entity_j);
								registry.collisions.emplace_with_duplicates(entity_j, entity);
							}
						}
					}
					else if (registry.players.has(motion_container.entities[j]))
					{
						if (collides_with_mesh(motion, motion_j, step_seconds, *registry.meshPtrs.get(motion_container.entities[j]))[4] == 1) {
							Entity entity_j = motion_container.entities[j];
							if ((!registry.collisions.has(entity) || !registry.collisions.has(entity_j))) {
								registry.collisions.emplace_with_duplicates(entity, entity_j);
								registry.collisions.emplace_with_duplicates(entity_j, entity);
							}
						}
					}
					else {
						if (collides(motion, motion_j, step_seconds))
						{
							Entity entity_j = motion_container.entities[j];
							// Create a collisions event
							// We are abusing the ECS system a bit in that we potentially insert muliple collisions for the same entity
							if ((!registry.collisions.has(entity) || !registry.collisions.has(entity_j))) {
								registry.collisions.emplace_with_duplicates(entity, entity_j);
								registry.collisions.emplace_with_duplicates(entity_j, entity);
							}
						}
					}
				}
			}

			if (registry.players.has(entity) && (collide == false)) {

				registry.players.get(entity).standing = 0;

			}

		}
	}

	// ------------------------------------- Boundary checking -------------------------------------
	// Check boundaries
	auto& motion_registry = registry.motions;
	for (uint i = 0; i < motion_registry.size(); i++) {
		Entity entity = motion_container.entities[i];
		Motion& motion = motion_registry.components[i];
		if ((motion.position.x - abs(motion.scale.x) / 2) < 0) {
			motion.velocity.x = 0;
			motion.position.x = abs(motion.scale.x) / 2;

			if (registry.shootBullets.has(entity) && !registry.eatables.has(entity)) {
				registry.meshPtrs.remove(entity);
				registry.hearts.remove(entity);
				registry.renderRequests.remove(entity);
			}
		}
		if ((motion.position.x + abs(motion.scale.x) / 2) > window_width_px) {
			motion.velocity.x = 0;
			motion.position.x = window_width_px - abs(motion.scale.x) / 2;

			if (registry.shootBullets.has(entity) && !registry.eatables.has(entity)) {
				registry.meshPtrs.remove(entity);
				registry.hearts.remove(entity);
				registry.renderRequests.remove(entity);
			}
		}
		if ((motion.position.y - abs(motion.scale.y) / 2) < 0) {
			motion.velocity.y = 0;
			motion.position.y = abs(motion.scale.y) / 2;
		}
		if ((motion.position.y + abs(motion.scale.y) / 2) > window_height_px) {
			motion.velocity.y = 0;
			motion.position.y = window_height_px - abs(motion.scale.y) / 2;
		}
	}
}

