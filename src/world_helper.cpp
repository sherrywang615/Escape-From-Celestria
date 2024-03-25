#pragma once
#include "world_helper.hpp"

const float LINE_SPACE = 130;
const vec2 RESUME_POS = { window_width_px / 2 - 80, window_height_px / 2 + 200 };
const vec2 SAVE_POS = { window_width_px / 2 - 80, RESUME_POS.y - LINE_SPACE };
const vec2 LOAD_POS = { window_width_px / 2 - 80, SAVE_POS.y - LINE_SPACE };
const vec2 QUIT_POS = { window_width_px / 2 - 80, LOAD_POS.y - LINE_SPACE };

void renderPauseMenu() {
	Entity resume, save, load, quit;
	std::vector<Entity> elements = {};
	Entity background = createMenuBackground({ window_width_px / 2, window_height_px / 2 }, { 300, 600 });
	auto& menu1  = registry.menus.emplace(background);
	resume = createText(RESUME_POS, 0.8, { 1, 1, 1 }, "Resume");
	auto& menu2 = registry.menus.emplace(resume);
	menu2.func = MENU_FUNC::RESUME;
	save = createText(SAVE_POS, 1, { 1, 1, 1 }, "SAVE");
	auto& menu3 = registry.menus.emplace(save);
	menu3.func = MENU_FUNC::SAVE;
	load = createText(LOAD_POS, 1, { 1, 1, 1 }, "LOAD");
	auto& menu4 = registry.menus.emplace(load);
	menu4.func = MENU_FUNC::LOAD;
	quit = createText(QUIT_POS, 1, { 1, 1, 1 }, "QUIT");
	auto& menu5 = registry.menus.emplace(quit);
	menu5.func = MENU_FUNC::QUIT;
}

void clearMenu() {
	for (Entity entity : registry.menus.entities) {
		registry.remove_all_components_of(entity);
	}
}


void saveGame() {
	std::ofstream file(SAVE_PATH, std::ofstream::out);
	//file.open(SAVE_PATH);

	if (file.is_open())
	{
		for (Entity player : registry.players.entities)
		{
			Motion motion = registry.motions.get(player);
			file << "Josh ";
			file << std::to_string(motion.position.x) << " ";
			file << std::to_string(motion.position.y) << "\n";
		}
		for (Entity key : registry.keys.entities) {
			Motion motion = registry.motions.get(key);
			file << "Key ";
			file << std::to_string(motion.position.x) << " ";
			file << std::to_string(motion.position.y) << "\n";
		}
		for (Entity zombie : registry.zombies.entities)
		{
			Motion motion = registry.motions.get(zombie);
			file << "Zombie ";
			file << std::to_string(motion.position.x) << " ";
			file << std::to_string(motion.position.y) << "\n";
		}
		for (Entity food : registry.foods.entities) {
			Motion motion = registry.motions.get(food);
			file << "Food ";
			file << std::to_string(motion.position.x) << " ";
			file << std::to_string(motion.position.y) << "\n";
		}
		for (Entity bullet : registry.bullets.entities) {
			Motion motion = registry.motions.get(bullet);
			file << "Bullet ";
			file << std::to_string(motion.position.x) << " ";
			file << std::to_string(motion.position.y) << "\n";
		}
	}
	else
	{
		printf("Cannot save because cannot open saving file\n");
	}
	file.close();
}


void loadGame(RenderSystem* renderer) {
	// clear all existing characters
	for (Entity entity : registry.players.entities) {
		registry.remove_all_components_of(entity);
	}
	for (Entity entity : registry.zombies.entities) {
		registry.remove_all_components_of(entity);
	}
	for (Entity entity : registry.keys.entities) {
		registry.remove_all_components_of(entity);
	}
	for (Entity entity : registry.foods.entities) {
		registry.remove_all_components_of(entity);
	}
	for (Entity entity : registry.bullets.entities) {
		registry.remove_all_components_of(entity);
	}
	std::fstream file;
	file.open(SAVE_PATH);
	bool readingJosh = false;
	bool readingZombie = false;

	if (file.is_open())
	{
		std::string line;
		while (getline(file, line))
		{
			std::vector<std::string> toks;
			std::string delimiter = " ";
			while (line.find(delimiter) != std::string::npos)
			{
				int delim_loc = line.find(delimiter);
				std::string token = line.substr(0, delim_loc);
				toks.push_back(token);
				line = line.substr(delim_loc + 1, line.size());
			}
			toks.push_back(line);

			if (toks[0] == "Josh")
			{
				Entity josh = createJosh(renderer, { std::stof(toks[1]), std::stof(toks[2]) });
				registry.colors.insert(josh, { 1, 0.8f, 0.8f });
			}
			else if (toks[0] == "Zombie")
			{
				createZombie(renderer, { std::stof(toks[1]), std::stof(toks[2]) });
			}
			else if (toks[0] == "Key") {
				createKey(renderer, { std::stof(toks[1]), std::stof(toks[2]) });
			}
			else if (toks[0] == "Food") {
				createFood(renderer, { std::stof(toks[1]), std::stof(toks[2]) });
			}
			else if (toks[0] == "Bullet") {
				createBullet(renderer, { std::stof(toks[1]), std::stof(toks[2]) });
			}
		}
	}
	else
	{
		printf("Cannot load because cannot open saving file\n");
	}
	file.close();
}

bool handleButtonEvents(Entity entity, RenderSystem* renderer, GLFWwindow* window) {
	MenuElement me = registry.menus.get(entity);
	if (me.func == MENU_FUNC::RESUME) {
		clearMenu();
		return false;
	}
	else if (me.func == MENU_FUNC::SAVE) {
		saveGame();
		printf("Game saved to %s\n", SAVE_PATH.c_str());
		return true;
	}
	else if (me.func == MENU_FUNC::LOAD) {
		loadGame(renderer);
		clearMenu();
		return false;
	}
	else if (me.func == MENU_FUNC::QUIT) { 
		glfwSetWindowShouldClose(window, true);
	}
	return true;
}