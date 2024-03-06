// internal
#include "ai_system.hpp"

#include <queue>
#include <unordered_map>

int X_frame = 1;

Vertex* findNearestVertex(vec2 pos) {
	float nearest = 9999999.f;
	Vertex* point;
	for (Vertex* vertex : graph.getVertices()) {
		float dist = findDistanceBetween({ vertex->x, vertex->y }, pos);
		if (dist < nearest) {
			nearest = dist;
			point = vertex;
			point->id = vertex->id;
			point->x = vertex->x;
			point->y = vertex->y;
			point->adjs = vertex->adjs;
		}
	}
	return point;
}

// Helper function to reverse a queue
std::queue<Vertex*> reverseQueue(std::queue<Vertex*>& reversedQueue) {
	std::vector<Vertex*> temp;
	while (!reversedQueue.empty()) {
		temp.push_back(reversedQueue.front());
		reversedQueue.pop();
	}
	for (int i = temp.size() - 1; i >= 0; i --) {
		reversedQueue.push(temp[i]);
	}
	return reversedQueue;
}

// pathfinding using A*
std::queue<Vertex*> findPathAStar(Vertex* start, Vertex* end) {
	std::priority_queue<std::pair<Vertex*, float>> open;
	std::unordered_map<Vertex*, Vertex*> parent;
	std::unordered_map<Vertex*, float> g;
	std::unordered_map<Vertex*, float> h;
	std::unordered_map<Vertex*, float> f;
	g[start] = 0.0f;
	h[start] = findDistanceBetween({ start->x, start->y }, { end->x, end->y });
	f[start] = g[start] + h[start];
	open.push({ start, f[start]});

	while (!open.empty()) {
		auto best = open.top().first;
		open.pop();

		if (best == end) {
			std::queue<Vertex*> reversedPath;
			while (best->id != start->id) {
				reversedPath.push(best);
				best = parent[best];
			}
			reversedPath.push(start);
			return reverseQueue(reversedPath);
		}

		for (auto& adj : best->adjs) {
			Vertex* v = adj.first;
			float gScore = findDistanceBetween({ best->x, best->y }, { v->x, v->y });
			float hScore = findDistanceBetween({ v->x, v->y }, { end->x, end->y });
			float fScore = g[best] + gScore + hScore;
			if (!f.count(v) || fScore < f[v]) {
				parent[v] = best;
				g[v] = g[best] + gScore;
				f[v] = fScore;
				open.push({ v, fScore });
			}
		}
	}
	
	// return empty queue if cannot find a path
	printf("Cannot find a path from {%f, %f} to {%f, %f}\n", start->x, start->y, end->x, end->y);
	return std::queue<Vertex*>();
}


// Zombie will move according to the path
void followPath(Motion& motion, std::queue<Vertex*> path,ACTION action, Vertex* end, float speed, bool is_jumping) {
	// precision controls how close between the target point and where the zombie stops
	float precision = 20.f;
	if (!path.empty()) {
		//printf("Current location: {%f, %f}\n", motion.position.x, motion.position.y);
		Vertex* v = path.front();
		//printf("Target vertex {%f, %f} with id {%d}\n", v->x, v->y, v->id);
		//printf("Current Action {%d}\n", action);
		float current_h = findDistanceBetween(motion.position, { end->x, end->y });
		float potential_h = findDistanceBetween({ v->x, v->y }, { end->x, end->y });


		if (current_h <= precision) {
			motion.velocity = { 0, 0 };
			return;
		}
		path.pop();
		if (path.empty()) {
			return;
		}
		Vertex* next = path.front();
		float dist_to_next = findDistanceBetween(motion.position, { next->x, next->y });
		float curr_to_next = findDistanceBetween({ v->x, v->y }, { next->x, next->y });
		//printf("dist to next: %f\n", dist_to_next);
		// Go to next vertex if motion is between curr and next vertices
		if (dist_to_next <= curr_to_next) {
			action = v->adjs[next];
			v = next;
			//printf("Reached vertex {%f}\n", v->x);
			//printf("Go to next point\n");
		}
		float dp = v->x - motion.position.x;
		float dir = dp / abs(dp);
		// printf("%f\n", dir);
		if (action == ACTION::WALK) {
			motion.velocity.x = dir * speed;
		}
		else if (action == ACTION::JUMP || is_jumping) {
			is_jumping = true;
			motion.velocity.x = dir * speed;
			motion.velocity.y = -800;
		}
	}
}

void updateZombiePath(float elapsed_ms) 
{
	float memory = 3000.f;
	for (Entity entity_z : registry.zombies.entities) {
		NormalZombie& zombie = registry.zombies.get(entity_z);
		Motion& motion_z = registry.motions.get(entity_z);
		for (Entity entity_p : registry.players.entities) {
			Motion& motion_p = registry.motions.get(entity_p);
			float sense_y_min = motion_z.position.y + abs(motion_z.scale.y)/2;
			vec2 sense_min = motion_z.position - zombie.sensing_range;
			vec2 sense_max = motion_z.position + zombie.sensing_range;

			// chase player if player is within sensing range and zombie is facing the player
			float player_pos_x = motion_p.position.x;
			if (player_pos_x >= sense_min.x && player_pos_x <= sense_max.x && motion_p.position.y >= sense_min.y && motion_p.position.y <= sense_y_min) {
				// sense the player to the right
				if (zombie.face == DIRECTION::RIGHT && (motion_p.position.x > motion_z.position.x)) {
					printf("zombie is alerted\n");
					zombie.is_alerted = true;
					zombie.memory = memory;
					Vertex* start = findNearestVertex(motion_z.position);
					Vertex* end = findNearestVertex(motion_p.position);
					auto path = findPathAStar(start, end);
					followPath(motion_z, path, ACTION::WALK, end, zombie.alerted_speed, zombie.is_jumping);
				}
				// sense the player to the left
				if (zombie.face == DIRECTION::LEFT && (motion_p.position.x < motion_z.position.x)) {
					printf("zombie is alerted\n");
					zombie.is_alerted = true;
					zombie.memory = memory;
					Vertex* start = findNearestVertex(motion_z.position);
					Vertex* end = findNearestVertex(motion_p.position);
					auto path = findPathAStar(start, end);
					followPath(motion_z, path, ACTION::WALK, end, zombie.alerted_speed, zombie.is_jumping);
				}
			}
			// Zombie lose memory when it is not alerted
			if (zombie.is_alerted) {
				zombie.memory -= elapsed_ms;
				Vertex* start = findNearestVertex(motion_z.position);
				Vertex* end = findNearestVertex(motion_p.position);
				auto path = findPathAStar(start, end);
				followPath(motion_z, path, ACTION::WALK, end, zombie.alerted_speed, zombie.is_jumping);
				if (zombie.memory < 0) {
					printf("zombie is not alerted anymore\n");
					zombie.is_alerted = false;
					zombie.walking_bound[0] = motion_z.position.x - zombie.walking_range;
					zombie.walking_bound[1] = motion_z.position.x + zombie.walking_range;
					zombie.memory = 0;
					printf("zombie walking_bound: {%f, %f}", zombie.walking_bound[0], zombie.walking_bound[1]);
				}
			}
			// if zombie is unalerted, begin wander around
			if (!zombie.is_alerted) {
				// if zombie touches the boundaries of its wandering range, turn around
				if (motion_z.position.x <= zombie.walking_bound[0] || motion_z.position.x >= zombie.walking_bound[1]) {
					motion_z.velocity.x *= -1;
					motion_z.scale.x *= -1;
				}
				// also turn around if the zombie touches the boundaries
				if (motion_z.position.x <= 0 + (motion_z.scale.x / 2) || motion_z.position.x >= window_width_px + (motion_z.scale.x / 2)) {
					motion_z.velocity.x *= -1;
					motion_z.scale.x *= -1;
				}
				motion_z.velocity.x = -30 * (motion_z.scale.x / abs(motion_z.scale.x));
			}

			// change zombie's facing direction
			if (motion_z.scale.x < 0) {
				zombie.face = DIRECTION::RIGHT;
			}
			else {
				zombie.face = DIRECTION::LEFT;
			}


			// Change zombie's scale
			if (motion_z.velocity.x > 0 && motion_z.scale.x > 0) {
				motion_z.scale.x *= -1;
			}
			if (motion_z.velocity.x < 0 && motion_z.scale.x < 0) {
				motion_z.scale.x *= -1;
			}
		}
	}
}

void AISystem::step(float elapsed_ms)
{
	updateZombiePath(elapsed_ms);
}