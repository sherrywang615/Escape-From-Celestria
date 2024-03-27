#pragma once
#include "menu.hpp"

const float LINE_SPACE = 100;
const vec2 RESUME_POS = { window_width_px / 2 - 80, window_height_px / 2 + 200 };
const vec2 SAVE_POS = { window_width_px / 2 - 80, RESUME_POS.y - LINE_SPACE };
const vec2 LOAD_POS = { window_width_px / 2 - 80, SAVE_POS.y - LINE_SPACE };
// const vec2 QUIT_POS = { window_width_px / 2 - 80, LOAD_POS.y - LINE_SPACE };


std::vector<vec2> arrangeText(int num) {
	std::vector<vec2> result = {};
	for (int i = 0; i < num; i++) {
		vec2 pos = { window_width_px / 2 - 80, window_height_px / 2 + 200 - i *  LINE_SPACE};
		result.push_back(pos);
	}
	return result;
}

void renderPauseMenu() {
	Entity resume, save, load, quit, help;
	std::vector<Entity> elements = {Entity(), Entity(), Entity(), Entity(), Entity()};
	std::vector<std::string> texts = { "Resume", "HELP", "Save", "Load", "QUIT" };
	std::vector<MENU_FUNC> funcs = { MENU_FUNC::RESUME, MENU_FUNC::HELP, MENU_FUNC::SAVE, MENU_FUNC::LOAD, MENU_FUNC::QUIT };
	Entity background = createMenuBackground({ window_width_px / 2, window_height_px / 2 }, { 300, 600 });
	// auto& menu1 = registry.menus.emplace(background);
	std::vector<vec2> pos = arrangeText(elements.size());
	for (int i = 0; i < elements.size(); i++) {
		Entity entity = createText(pos[i], 0.8, {1, 1, 1}, texts[i]);
		auto& menu2 = registry.menus.emplace(entity);
		menu2.func = funcs[i];
	}
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
	// bool readingJosh = false;
	// bool readingZombie = false;

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
		graph.clear();
		glfwSetWindowShouldClose(window, true);
	}
	else if (me.func == MENU_FUNC::HELP) {
		Entity entity = createHelpInfo(renderer, vec2(window_width_px - 515, window_height_px - 350));
		registry.menus.emplace(entity);
	}
	return true;
}


