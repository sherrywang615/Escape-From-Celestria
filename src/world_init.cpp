#include "world_init.hpp"
#include "tiny_ecs_registry.hpp"
#include <iostream>

Entity createJosh(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::JOSH);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = mesh.original_size * 250.f;
	motion.scale.y *= -1; // point front to the right

	// Create and (empty) Chicken component to be able to refer to all eagles
	registry.players.emplace(entity);
	registry.gravities.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TEXTURE_COUNT, // TEXTURE_COUNT indicates that no txture is needed
			EFFECT_ASSET_ID::JOSH,
			GEOMETRY_BUFFER_ID::JOSH });

	return entity;
}


/*Entity createJosh(RenderSystem* renderer, vec2 position)
{
    auto entity = Entity();

    Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
    registry.meshPtrs.emplace(entity, &mesh);
    auto& motion = registry.motions.emplace(entity);

    motion.angle = 0.f;
    motion.velocity = { 0, 50 };
    motion.position = position;
    motion.scale = vec2({ -JOSH_BB_WIDTH, JOSH_BB_HEIGHT });

    registry.renderRequests.insert(
        entity,
        { TEXTURE_ASSET_ID::JOSH, 
          EFFECT_ASSET_ID::TEXTURED,
          GEOMETRY_BUFFER_ID::SPRITE });

    return entity;
}*/

//Entity createBug(RenderSystem* renderer, vec2 position)
//{
//	// Reserve en entity
//	auto entity = Entity();
//
//	// Store a reference to the potentially re-used mesh object
//	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
//	registry.meshPtrs.emplace(entity, &mesh);
//
//	// Initialize the position, scale, and physics components
//	auto& motion = registry.motions.emplace(entity);
//	motion.angle = 0.f;
//	motion.velocity = { 0, 50 };
//	motion.position = position;
//
//	// Setting initial values, scale is negative to make it face the opposite way
//	motion.scale = vec2({ -BUG_BB_WIDTH, BUG_BB_HEIGHT });
//
//	// Create an (empty) Bug component to be able to refer to all bug
//	registry.eatables.emplace(entity);
//	registry.renderRequests.insert(
//		entity,
//		{ TEXTURE_ASSET_ID::BUG,
//			EFFECT_ASSET_ID::TEXTURED,
//			GEOMETRY_BUFFER_ID::SPRITE });
//
//	return entity;
//}

Entity createZombie(RenderSystem* renderer, vec2 position, int state, double range)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the motion of zombie to rightwards
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 30.f, 0 };
	motion.position = position;

	// Setting initial values, scale is negative to make it face the opposite way
	motion.scale = vec2({ -ZOMBIE_BB_WIDTH, ZOMBIE_BB_HEIGHT });

	// Create and (empty) Eagle component to be able to refer to all eagles
	registry.deadlys.emplace(entity);
	registry.zombies.emplace(entity);
	registry.gravities.emplace(entity);
	registry.zombies.get(entity).walking_range[0] = position.x - range;
	registry.zombies.get(entity).walking_range[1] = position.x + range;
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::ZOMBIE,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}


Entity createPlatform(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the motion of zombie to rightwards
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = {0.0f, 0.0f};
	motion.position = pos;
	motion.scale = {PLATFORM_WIDTH,PLATFORM_HEIGHT};

	
	registry.platforms.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::PLATFORM,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}


Entity createLine(vec2 position, vec2 scale)
{
	Entity entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TEXTURE_COUNT,
		 EFFECT_ASSET_ID::EGG,
		 GEOMETRY_BUFFER_ID::DEBUG_LINE });

	// Create motion
	Motion& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0, 0 };
	motion.position = position;
	motion.scale = scale;

	registry.debugComponents.emplace(entity);
	return entity;
}

Entity createEgg(vec2 pos, vec2 size)
{
	auto entity = Entity();

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = size;

	// Create and (empty) Chicken component to be able to refer to all eagles
	registry.deadlys.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TEXTURE_COUNT, // TEXTURE_COUNT indicates that no txture is needed
			EFFECT_ASSET_ID::EGG,
			GEOMETRY_BUFFER_ID::EGG });

	return entity;
}