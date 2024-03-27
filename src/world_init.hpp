#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "render_system.hpp"

// These are ahrd coded to the dimensions of the entity texture
const float NPC_BB_WIDTH = 0.5f * 165.f;
const float NPC_BB_HEIGHT = 0.5f * 165.f;
const float FOOD_BB_WIDTH = 0.25f * 165.f;
const float FOOD_BB_HEIGHT = 0.25f * 165.f;
const float EAGLE_BB_WIDTH = 0.6f * 300.f;
const float EAGLE_BB_HEIGHT = 0.6f * 202.f;
const float JOSH_BB_WIDTH = 0.6f * 165.f;
const float JOSH_BB_HEIGHT = 0.6f * 165.f;
const float DOOR_BB_WIDTH = 0.7f * 165.f;
const float DOOR_BB_HEIGHT = 0.7f * 165.f;
const float HEART_BB_WIDTH = 0.33f * 165.f;
const float HEART_BB_HEIGHT = 0.33f * 165.f;
const float SMALL_BULLET_BB_WIDTH = 0.23f * 165.f;
const float SMALL_BULLET_BB_HEIGHT = 0.23f * 165.f;
const float KEY_BB_WIDTH = 0.23f * 165.f;
const float KEY_BB_HEIGHT = 0.23f * 165.f;
const float CABINET_BB_WIDTH = 0.5f * 165.f;
const float CABINET_BB_HEIGHT = 0.5f * 165.f;

// Zombie
const float ZOMBIE_BB_WIDTH = 0.6f * 108.f;
const float ZOMBIE_BB_HEIGHT = 0.6f * 184.f;

// platform
const float PLATFORM_WIDTH = 24.2f;
const float PLATFORM_HEIGHT = 24.2f;

// the zombie
Entity createZombie(RenderSystem *renderer, vec2 position, int state = 0, double range = 200);
// the prey
Entity createFood(RenderSystem *renderer, vec2 position);
Entity createBullet(RenderSystem *renderer, vec2 position);
Entity createDoor(RenderSystem *renderer, vec2 position);
Entity createKey(RenderSystem *renderer, vec2 position);
Entity createHeart(RenderSystem *renderer, vec2 position);
Entity createCabinet(RenderSystem *renderer, vec2 position);
Entity createBulletSmall(RenderSystem *renderer, vec2 position);
Entity createSmallKey(RenderSystem *renderer, vec2 position);
Entity createBulletShoot(RenderSystem *renderer, vec2 position);
Entity createBackground(RenderSystem* renderer, vec2 position);
Entity createBackground2(RenderSystem* renderer, vec2 position);
Entity createBackground3(RenderSystem* renderer, vec2 position);
Entity createBackground4(RenderSystem* renderer, vec2 position);
Entity createObject(RenderSystem *renderer, vec2 pos);
Entity createSpeechPoint(RenderSystem *renderer, vec2 position, unsigned int index);
Entity createNPC(RenderSystem *renderer, vec2 position, unsigned int index);
Entity createGold(RenderSystem *renderer, vec2 position);
Entity createFireball(RenderSystem* renderer, vec2 position);
// the enemy
Entity createEagle(RenderSystem *renderer, vec2 position);
// Josh
Entity createJosh(RenderSystem *renderer, vec2 position);
// Platform
Entity createPlatform(RenderSystem* renderer, vec2 pos);
//help help_sign
Entity createHelpSign(RenderSystem* renderer, vec2 position);
//help help_info
Entity createHelpInfo(RenderSystem* renderer, vec2 position);
// Text
Entity createText(vec2 pos, float scale, vec3 color, std::string content);
Entity createMenuBackground(vec2 pos, vec2 scale);

// a red line for debugging purposes
Entity createLine(vec2 position, vec2 size);
// a egg
Entity createEgg(vec2 pos, vec2 size);

std::vector<std::vector<char>> loadMap(std::string path);

