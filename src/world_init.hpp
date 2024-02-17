#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "render_system.hpp"


// These are ahrd coded to the dimensions of the entity texture
const float BUG_BB_WIDTH = 0.6f * 165.f;
const float BUG_BB_HEIGHT = 0.6f * 165.f;
const float EAGLE_BB_WIDTH = 0.6f * 300.f;
const float EAGLE_BB_HEIGHT = 0.6f * 202.f;
const float JOSH_BB_WIDTH = 0.6f * 165.f;
const float JOSH_BB_HEIGHT = 0.6f * 165.f;

// Zombie
const float ZOMBIE_BB_WIDTH = 0.6f * 108.f;
const float ZOMBIE_BB_HEIGHT = 0.6f * 184.f;

// platform
const float PLATFORM_WIDTH = 24.2f;
const float PLATFORM_HEIGHT = 24.2f;

// the player
Entity createJosh(RenderSystem* renderer, vec2 pos);
// the zombie
Entity createZombie(RenderSystem* renderer, vec2 position, int state = 0, double range = 200);
// the prey
Entity createBug(RenderSystem* renderer, vec2 position);
// the enemy
Entity createEagle(RenderSystem* renderer, vec2 position);
// Josh
Entity createJosh(RenderSystem* renderer, vec2 position);
// Platform
Entity createPlatform(RenderSystem* renderer, vec2 pos);

// a red line for debugging purposes
Entity createLine(vec2 position, vec2 size);
// a egg
Entity createEgg(vec2 pos, vec2 size);


