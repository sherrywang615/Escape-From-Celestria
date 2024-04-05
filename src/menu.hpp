#pragma once

#include "world_init.hpp"
#include "common.hpp"
#include "tiny_ecs_registry.hpp"
#include "world_system.hpp"

void clearMenu();
void renderPauseMenu();
void renderStartMenu();
void saveGame();
//void loadGame(RenderSystem* renderer);
bool handleButtonEvents(Entity entity, RenderSystem* renderer, GLFWwindow* window, bool& has_key, int& hp_count, int& bullet_count);
int handleStartButtonEvents(Entity entity, RenderSystem* renderer, GLFWwindow* window, bool& has_key, int& hp_count, int& bullet_count);