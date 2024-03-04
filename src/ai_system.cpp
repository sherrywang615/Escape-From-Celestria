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
		//parent[best] = start;

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
	return std::queue<Vertex*>();
}


// Zombie will move according to the path
void followPath(Motion& motion, std::queue<Vertex*> path,ACTION action, Vertex* end) {
	if (!path.empty()) {
		Vertex* v = path.front();
		//ACTION action = v->adjs[v];
		//printf("Target vertex {%f} with id {%d}\n", v->x, v->id);
		//printf("Current Action {%d}\n", action);
		float dp = v->x - motion.position.x;
		float current_h = findDistanceBetween(motion.position, { end->x, end->y });
		float potential_h = findDistanceBetween({ v->x, v->y }, { end->x, end->y });
		if (current_h < potential_h) {
			path.pop();
			Vertex* next = path.front();
			ACTION action = v->adjs[next];
			//printf("Reached vertex {%f}\n", v->x);
			followPath(motion, path, action, end);
			return;
		}

		float dir = dp / abs(dp);
		//printf("%f\n", dir);
		if (action == ACTION::WALK) {
			motion.velocity.x = dir * 30;
		}
		else if (action == ACTION::JUMP) {
			motion.velocity.x = dir * 30;
			motion.velocity.y = -50;
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
			float dist = findDistanceBetween(motion_z.position, motion_p.position);

			// chase player if player is within sensing range and zombie is facing the plaeyr
			if (dist <= zombie.sensing_range) {
				if (zombie.face == DIRECTION::RIGHT && (motion_p.position.x > motion_z.position.x)) {
					zombie.is_alerted = true;
					zombie.memory = memory;
					Vertex* start = findNearestVertex(motion_z.position);
					Vertex* end = findNearestVertex(motion_p.position);
					auto path = findPathAStar(start, end);
					followPath(motion_z, path, ACTION::WALK, end);
				}
				if (zombie.face == DIRECTION::LEFT && (motion_p.position.x < motion_z.position.x)) {
					zombie.is_alerted = true;
					zombie.memory = memory;
					Vertex* start = findNearestVertex(motion_z.position);
					Vertex* end = findNearestVertex(motion_p.position);
					auto path = findPathAStar(start, end);
					followPath(motion_z, path, ACTION::WALK, end);
				}
			}
			// Zombie lose memory when it is not alerted
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