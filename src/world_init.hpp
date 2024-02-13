#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "render_system.hpp"

// These are ahrd coded to the dimensions of the entity texture

const float ZOMBIE_BB_WIDTH = 0.6f * 180.f;
const float ZOMBIE_BB_HEIGHT = 0.6f * 180.f;

// the player
Entity createJosh(RenderSystem* renderer, vec2 pos);
// the zombie
Entity createZombie(RenderSystem* renderer, vec2 position, int state = 0, double range = 200);
// a red line for debugging purposes
Entity createLine(vec2 position, vec2 size);
// a egg
Entity createEgg(vec2 pos, vec2 size);


