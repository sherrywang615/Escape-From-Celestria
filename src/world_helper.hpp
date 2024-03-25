#pragma once

#include "world_init.hpp"
#include "common.hpp"
#include "tiny_ecs_registry.hpp"




void renderPauseMenu();

void clearMenu();

void saveGame();

void loadGame(RenderSystem* renderer);

bool handleButtonEvents(Entity entity, RenderSystem* renderer, GLFWwindow* window);