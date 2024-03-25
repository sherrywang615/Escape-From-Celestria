#pragma once

// internal
#include "common.hpp"

// stlib
#include <vector>
#include <random>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>

#include "render_system.hpp"
#include "dialog_system.hpp"

// Container for all our entities and game logic. Individual rendering / update is
// deferred to the relative update() methods
class WorldSystem
{
public:
	WorldSystem();

	// for fps counter
	float fps;
	float fpsCount;
	float fpsTimer;

	int currentLevel = 1;

	bool zombie_died = false;
	bool is_invincible = false;

	// Creates a window
	GLFWwindow *create_window();

	// starts the game
	void init(RenderSystem *renderer, DialogSystem *dialog);

	// Releases all associated resources
	~WorldSystem();

	// Steps the game ahead by ms milliseconds
	bool step(float elapsed_ms);

	// Check for collisions
	void handle_collisions();

	// Should the game be over ?
	bool is_over() const;

	// Is the game paused?
	bool is_paused() const;

private:
	//start time
	std::chrono::system_clock::time_point start;
	std::chrono::system_clock::time_point zombie_die_start;
	int zombie_die_counter = 0;
	
	// Input callback functions
	bool renderInfo;
	void on_key(int key, int, int action, int mod);
	void on_mouse_move(vec2 pos);

	// read map
	bool createEntityBaseOnMap(std::vector<std::vector<char>> map);

	void showKeyOnScreen(RenderSystem *renderer, bool have_key);

	// restart level
	void restart_game();

	// render new level
	void render_new_level(int level);


	bool isJoshHidden = false;
	void hideJosh(RenderSystem *renderer);
	bool isNearCabinet(Entity player, Entity cabinet);
	bool isNearDoor(Entity player, Entity door);
	void removeSmallBullets(RenderSystem *renderer);
	vec2 joshPosition;
	vec2 joshScale;
	// OpenGL window handle
	GLFWwindow *window;

	// Number of bug eaten by the chicken, displayed in the window title
	unsigned int points;
	unsigned int hp_count;
	bool have_key;
	unsigned int bullets_count;
	
	// Game state
	RenderSystem *renderer;
	DialogSystem *dialog;
	float current_speed;
	float next_eagle_spawn;
	float next_bug_spawn;
	Entity player_chicken;
	Entity player_josh;
	bool jumped = false;
	unsigned int create_heart_distance = 50;
	unsigned int create_bullet_distance = 30;

	// music references
	Mix_Music *bg1_music;
	Mix_Music *bg2_music;
	Mix_Music *bg3_music;
	Mix_Music *bg4_music;
	Mix_Chunk *doorOpen_music;
	Mix_Chunk *eat_music;
	Mix_Chunk *shoot_music;
	Mix_Chunk *trush_music;

	// C++ random number generator
	std::default_random_engine rng;
	std::uniform_real_distribution<float> uniform_dist; // number between 0..1
};
