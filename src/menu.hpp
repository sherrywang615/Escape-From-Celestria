#pragma once

#include "world_init.hpp"
#include "common.hpp"
#include "tiny_ecs_registry.hpp"

void clearMenu();
void renderPauseMenu();
void saveGame();
//void loadGame(RenderSystem* renderer);
bool handleButtonEvents(Entity entity, RenderSystem* renderer, GLFWwindow* window, bool& has_key, int& hp_count, int& bullet_count);