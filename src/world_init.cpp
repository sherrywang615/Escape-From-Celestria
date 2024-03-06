#include "world_init.hpp"
#include "tiny_ecs_registry.hpp"
#include <iostream>

Entity createJosh(RenderSystem *renderer, vec2 position)
{
	auto entity = Entity();

    Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::JOSH);
    registry.meshPtrs.emplace(entity, &mesh);
    auto& motion = registry.motions.emplace(entity);

	motion.angle = 0.f;
	motion.velocity = {0, 0};
	motion.position = position;
	motion.scale = vec2({-JOSH_BB_WIDTH * 0.6, JOSH_BB_HEIGHT * 0.6});
	registry.players.emplace(entity);
	registry.gravities.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{TEXTURE_ASSET_ID::JOSHGUN1,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE});

	return entity;
}

Entity createZombie(RenderSystem *renderer, vec2 position, int state, double range)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh &mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the motion of zombie to rightwards
	auto &motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = {0, 0};
	motion.position = position;

	// Setting initial values, scale is negative to make it face the opposite way
	motion.scale = vec2({ZOMBIE_BB_WIDTH * 0.6, ZOMBIE_BB_HEIGHT * 0.6});

	// Create and (empty) Eagle component to be able to refer to all eagles
	registry.deadlys.emplace(entity);
	registry.zombies.emplace(entity);
	registry.gravities.emplace(entity);
	registry.zombies.get(entity).walking_bound[0] = position.x - range;
	registry.zombies.get(entity).walking_bound[1] = position.x + range;
	registry.renderRequests.insert(
		entity,
		{TEXTURE_ASSET_ID::ZOMBIE,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE});

	return entity;
}

Entity createFood(RenderSystem *renderer, vec2 position)
{
	// Reserve en entity
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh &mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the position, scale, and physics components
	auto &motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = {0, 0};
	motion.position = position;

	// Setting initial values, scale is negative to make it face the opposite way
	motion.scale = vec2({-FOOD_BB_WIDTH, FOOD_BB_HEIGHT});

	// Create an (empty) Bug component to be able to refer to all bug
	registry.eatables.emplace(entity);
	registry.foods.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{TEXTURE_ASSET_ID::FOOD,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE});

	return entity;
}

Entity createBullet(RenderSystem *renderer, vec2 position)
{
	// Reserve en entity
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh &mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the position, scale, and physics components
	auto &motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = {0, 0};
	motion.position = position;

	// Setting initial values, scale is negative to make it face the opposite way
	motion.scale = vec2({-HEART_BB_WIDTH, HEART_BB_HEIGHT});

	// Create an (empty) Bug component to be able to refer to all bug
	registry.eatables.emplace(entity);
	registry.bullets.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{TEXTURE_ASSET_ID::BULLET,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE});

	return entity;
}

Entity createBulletShoot(RenderSystem *renderer, vec2 position)
{
	// Reserve en entity
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh &mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the position, scale, and physics components
	auto &motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = {0, 0};
	motion.position = position;

	// Setting initial values, scale is negative to make it face the opposite way
	motion.scale = vec2({-HEART_BB_WIDTH, HEART_BB_HEIGHT});

	// Create an (empty) Bug component to be able to refer to all bug
	registry.shootBullets.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{TEXTURE_ASSET_ID::BULLET,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE});

	return entity;
}

Entity createBulletSmall(RenderSystem *renderer, vec2 position)
{
	// Reserve en entity
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh &mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the position, scale, and physics components
	auto &motion = registry.motions.emplace(entity);
	motion.angle = 2.4f; 
	motion.velocity = {0, 0};
	motion.position = position;

	// Setting initial values, scale is negative to make it face the opposite way
	motion.scale = vec2({-SMALL_BULLET_BB_WIDTH, SMALL_BULLET_BB_HEIGHT});
	

	// Create an (empty) Bug component to be able to refer to all bug
	registry.smallBullets.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{TEXTURE_ASSET_ID::BULLET,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE});

	return entity;
}

Entity createKey(RenderSystem *renderer, vec2 position)
{
	// Reserve en entity
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh &mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the position, scale, and physics components
	auto &motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = {0, 0};
	motion.position = position;

	// Setting initial values, scale is negative to make it face the opposite way
	motion.scale = vec2({-HEART_BB_WIDTH, HEART_BB_HEIGHT});

	// Create an (empty) Bug component to be able to refer to all bug
	registry.eatables.emplace(entity);
	registry.keys.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{TEXTURE_ASSET_ID::KEY,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE});

	return entity;
}

Entity createSmallKey(RenderSystem *renderer, vec2 position)
{
	// Reserve en entity
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh &mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the position, scale, and physics components
	auto &motion = registry.motions.emplace(entity);
	motion.angle = 0.8f;
	motion.velocity = {0, 0};
	motion.position = position;

	// Setting initial values, scale is negative to make it face the opposite way
	motion.scale = vec2({-KEY_BB_WIDTH, KEY_BB_HEIGHT});

	// Create an (empty) Bug component to be able to refer to all bug
	// registry.eatables.emplace(entity);
	// registry.keys.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{TEXTURE_ASSET_ID::KEY,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE});

	return entity;
}


Entity createDoor(RenderSystem *renderer, vec2 position)
{
	// Reserve en entity
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh &mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the position, scale, and physics components
	auto &motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = {0, 0};
	motion.position = position;

	// Setting initial values, scale is negative to make it face the opposite way
	motion.scale = vec2({-DOOR_BB_WIDTH, DOOR_BB_HEIGHT});

	// Create an (empty) Bug component to be able to refer to all bug
	registry.doors.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{TEXTURE_ASSET_ID::DOOR,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE});

	return entity;
}

Entity createCabinet(RenderSystem *renderer, vec2 position)
{
	// Reserve en entity
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh &mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the position, scale, and physics components
	auto &motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = {0, 0};
	motion.position = position;

	// Setting initial values, scale is negative to make it face the opposite way
	motion.scale = vec2({-DOOR_BB_WIDTH, DOOR_BB_HEIGHT});

	// Create an (empty) Bug component to be able to refer to all bug
	registry.cabinets.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{TEXTURE_ASSET_ID::CABINET,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE});

	return entity;
}

Entity createPlatform(RenderSystem *renderer, vec2 pos)
{
	auto entity = Entity();

	Mesh &mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	auto &platform = registry.platforms.emplace(entity);
	platform.position = pos;
	platform.scale = {PLATFORM_WIDTH, PLATFORM_HEIGHT};

	auto &motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = {0.0f, 0.0f};
	motion.position = pos;
	motion.scale = {PLATFORM_WIDTH, PLATFORM_HEIGHT};

	// registry.platforms.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{TEXTURE_ASSET_ID::PLATFORM,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE});

	return entity;
}

Entity createHeart(RenderSystem *renderer, vec2 position)
{
	// Reserve en entity
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh &mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the position, scale, and physics components
	auto &motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = {0, 0};
	motion.position = position;

	// Setting initial values, scale is negative to make it face the opposite way
	motion.scale = vec2({-HEART_BB_HEIGHT, HEART_BB_WIDTH});

	// Create an (empty) Bug component to be able to refer to all bug
	registry.hearts.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{TEXTURE_ASSET_ID::HEART,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE});

	return entity;
}

Entity createLine(vec2 position, vec2 scale)
{
	Entity entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	registry.renderRequests.insert(
		entity,
		{TEXTURE_ASSET_ID::TEXTURE_COUNT,
		 EFFECT_ASSET_ID::EGG,
		 GEOMETRY_BUFFER_ID::DEBUG_LINE});

	// Create motion
	Motion &motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = {0, 0};
	motion.position = position;
	motion.scale = scale;

	registry.debugComponents.emplace(entity);
	return entity;
}

Entity createHelpInfo(RenderSystem* renderer, vec2 position)
{
	// Reserve en entity
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the position, scale, and physics components
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0, 0 };
	motion.position = position;

	// Setting initial values, scale is negative to make it face the opposite way
	motion.scale = vec2(700,280);

	registry.eatables.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::HELP_INFO,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	registry.debugComponents.emplace(entity);
	return entity;
}

Entity createHelpSign(RenderSystem* renderer, vec2 position)
{
	// Reserve en entity
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the position, scale, and physics components
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0, 0 };
	motion.position = position;

	// Setting initial values, scale is negative to make it face the opposite way
	motion.scale = vec2(90,85);

	registry.eatables.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::HELP_SIGN,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createEgg(vec2 pos, vec2 size)
{
	auto entity = Entity();

	// Setting initial motion values
	Motion &motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.velocity = {0.f, 0.f};
	motion.scale = size;

	// Create and (empty) Chicken component to be able to refer to all eagles
	registry.deadlys.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{TEXTURE_ASSET_ID::TEXTURE_COUNT, // TEXTURE_COUNT indicates that no txture is needed
		 EFFECT_ASSET_ID::EGG,
		 GEOMETRY_BUFFER_ID::EGG});

	return entity;
}


std::vector<std::vector<char>> loadMap(std::string path) {
	std::fstream file;
	file.open(path);
	std::vector<std::vector<char>> map;

	if (file.is_open()) {
		std::string line;
		int i = 0;
		while (getline(file, line)) {
			if (line[0] == '1') {
				continue;
			}
			map.push_back(std::vector<char>());
			for (int j = 0; j < line.length(); j++) {
				map[i].push_back(line[j]);
			}
			i++;
		}
	}
	else {
		printf("Cannot load map. Check file path!\n");
	}
	file.close();

	return map;
}

