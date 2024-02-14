#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "render_system.hpp"

// These are hard coded to the dimensions of the entity texture

const float ZOMBIE_BB_WIDTH = 0.6f * 180.f;
const float ZOMBIE_BB_HEIGHT = 0.6f * 180.f;
const float PLATFORM_WIDTH = 24.2f;
const float PLATFORM_HEIGHT = 24.2f;

// the player
Entity createJosh(RenderSystem* renderer, vec2 pos);
// the zombie
Entity createZombie(RenderSystem* renderer, vec2 position, int state = 0, double range = 200);

Entity createPlatform(RenderSystem* renderer, vec2 pos);
// a red line for debugging purposes
Entity createLine(vec2 position, vec2 size);
// a egg
Entity createEgg(vec2 pos, vec2 size);


