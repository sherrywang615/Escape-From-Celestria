// Header
#include "world_system.hpp"
#include "world_init.hpp"

// stlib
#include <cassert>
#include <sstream>
#include <iostream>
#include "physics_system.hpp"

#include <fstream>
#include <ft2build.h>
#include FT_FREETYPE_H

// Game configuration
const size_t MAX_EAGLES = 15;
const size_t MAX_BUG = 5;
const size_t EAGLE_DELAY_MS = 2000 * 3;
const size_t BUG_DELAY_MS = 5000 * 3;

// Create the bug world
WorldSystem::WorldSystem()
	: hp_count(0), next_eagle_spawn(0.f), next_bug_spawn(0.f), bullets_count(0), have_key(false)
{
	// Seeding rng with random device
	rng = std::default_random_engine(std::random_device()());
}

WorldSystem::~WorldSystem()
{
	// Destroy music components
	// if (background_music != nullptr)
	//	Mix_FreeMusic(background_music);
	// if (chicken_dead_sound != nullptr)
	//	Mix_FreeChunk(chicken_dead_sound);
	// if (chicken_eat_sound != nullptr)
	//	Mix_FreeChunk(chicken_eat_sound);
	Mix_CloseAudio();

	// Destroy all created components
	registry.clear_all_components();

	// Close the window
	glfwDestroyWindow(window);
}

// Debugging
namespace
{
	void glfw_err_cb(int error, const char *desc)
	{
		fprintf(stderr, "%d: %s", error, desc);
	}
}

// World initialization
// Note, this has a lot of OpenGL specific things, could be moved to the renderer
GLFWwindow *WorldSystem::create_window()
{
	///////////////////////////////////////
	// Initialize GLFW
	glfwSetErrorCallback(glfw_err_cb);
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW");
		return nullptr;
	}

	//-------------------------------------------------------------------------
	// If you are on Linux or Windows, you can change these 2 numbers to 4 and 3 and
	// enable the glDebugMessageCallback to have OpenGL catch your mistakes for you.
	// GLFW / OGL Initialization
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#if __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	glfwWindowHint(GLFW_RESIZABLE, 0);

	// Create the main window (for rendering, keyboard, and mouse input)
	window = glfwCreateWindow(window_width_px, window_height_px, "Escape from Celestria", nullptr, nullptr);
	if (window == nullptr)
	{
		fprintf(stderr, "Failed to glfwCreateWindow");
		return nullptr;
	}

	// Setting callbacks to member functions (that's why the redirect is needed)
	// Input is handled using GLFW, for more info see
	// http://www.glfw.org/docs/latest/input_guide.html
	glfwSetWindowUserPointer(window, this);
	auto key_redirect = [](GLFWwindow *wnd, int _0, int _1, int _2, int _3)
	{ ((WorldSystem *)glfwGetWindowUserPointer(wnd))->on_key(_0, _1, _2, _3); };
	auto cursor_pos_redirect = [](GLFWwindow *wnd, double _0, double _1)
	{ ((WorldSystem *)glfwGetWindowUserPointer(wnd))->on_mouse_move({_0, _1}); };
	glfwSetKeyCallback(window, key_redirect);
	glfwSetCursorPosCallback(window, cursor_pos_redirect);

	//////////////////////////////////////
	// Loading music and sounds with SDL
	// !!! TODO: added music back in M4
	// if (SDL_Init(SDL_INIT_AUDIO) < 0)
	//{
	//	fprintf(stderr, "Failed to initialize SDL Audio");
	//	return nullptr;
	//}
	// if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1)
	//{
	//	fprintf(stderr, "Failed to open audio device");
	//	return nullptr;
	//}

	// background_music = Mix_LoadMUS(audio_path("music.wav").c_str());
	// chicken_dead_sound = Mix_LoadWAV(audio_path("chicken_dead.wav").c_str());
	// chicken_eat_sound = Mix_LoadWAV(audio_path("chicken_eat.wav").c_str());

	// if (background_music == nullptr || chicken_dead_sound == nullptr || chicken_eat_sound == nullptr)
	//{
	//	fprintf(stderr, "Failed to load sounds\n %s\n %s\n %s\n make sure the data directory is present",
	//			audio_path("music.wav").c_str(),
	//			audio_path("chicken_dead.wav").c_str(),
	//			audio_path("chicken_eat.wav").c_str());
	//	return nullptr;
	// }

	return window;
}

void WorldSystem::init(RenderSystem *renderer_arg)
{
	this->renderer = renderer_arg;
	// Playing background music indefinitely
	// !!! TODO: Bring this back in M4
	// Mix_PlayMusic(background_music, -1);
	// fprintf(stderr, "Loaded music\n");

	// Set all states to default
	restart_game();
}

// Linear interpolation
vec3 lerp(vec3 start, vec3 end, float t)
{
	return start * (1 - t) + end * t;
}

// Update our game world
bool WorldSystem::step(float elapsed_ms_since_last_update)
{

	// Remove debug info from the last step
	while (registry.debugComponents.entities.size() > 0)
		registry.remove_all_components_of(registry.debugComponents.entities.back());

	// Removing out of screen entities
	auto &motions_registry = registry.motions;

	// Remove entities that leave the screen on the left side
	// Iterate backwards to be able to remove without unterfering with the next object to visit
	// (the containers exchange the last element with the current)
	for (int i = (int)motions_registry.components.size() - 1; i >= 0; --i)
	{
		Motion &motion = motions_registry.components[i];
		if (motion.position.x + abs(motion.scale.x) < 0.f)
		{
			if (!registry.players.has(motions_registry.entities[i])) // don't remove the player
				registry.remove_all_components_of(motions_registry.entities[i]);
		}
	}

	// change josh's color gradually
	auto &color_change_registry = registry.colorChanges;
	for (int i = (int)color_change_registry.components.size() - 1; i >= 0; --i)
	{
		Entity entity = color_change_registry.entities[i];
		ColorChange &color_change = color_change_registry.components[i];
		color_change.color_time_elapsed += elapsed_ms_since_last_update / 1000.0f;
		float t = color_change.color_time_elapsed / color_change.color_duration;
		if (t < 1.0f)
		{
			vec3 color_new = lerp(color_change.color_start, color_change.color_end, t);
			if (registry.colors.has(entity))
			{
				registry.colors.remove(entity);
				registry.colors.emplace(entity, color_new);
			}
			else
			{
				registry.colors.emplace(entity, color_new);
			}
		}
		else
		{
			if (!registry.colors.has(entity))
			{
				registry.colors.emplace(entity, color_change.color_end);
			}
			else
			{
				registry.colorChanges.remove(entity);
			}
		}
	}

	// Processing the chicken state
	assert(registry.screenStates.components.size() <= 1);
	ScreenState &screen = registry.screenStates.components[0];

	float min_counter_ms = 3000.f;
	for (Entity entity : registry.deathTimers.entities)
	{
		// progress timer
		DeathTimer &counter = registry.deathTimers.get(entity);
		counter.counter_ms -= elapsed_ms_since_last_update;
		if (counter.counter_ms < min_counter_ms)
		{
			min_counter_ms = counter.counter_ms;
		}

		// restart the game once the death timer expired
		if (counter.counter_ms < 0)
		{
			registry.deathTimers.remove(entity);

			screen.darken_screen_factor = 0;
			restart_game();
			return true;
		}
	}
	// reduce window brightness if any of the present chickens is dying
	// screen.darken_screen_factor = 1 - min_counter_ms / 3000;

	for (Entity entity : registry.deductHpTimers.entities)
	{
		// Progress timer
		DeductHpTimer &counter = registry.deductHpTimers.get(entity);
		counter.counter_ms -= elapsed_ms_since_last_update;

		if (counter.counter_ms < min_counter_ms)
		{
			min_counter_ms = counter.counter_ms;
		}

		if (counter.counter_ms < 0)
		{
			registry.deductHpTimers.remove(entity);
			return true;
		}
	}

	return true;
}

bool WorldSystem::createEntityBaseOnMap(std::vector<std::vector<char>> map)
{
	for (int i = 0; i < map.size(); i++)
	{
		for (int j = 0; j < map[i].size(); j++)
		{
			float x = j * 10;
			float y = i * 10;
			char tok = map[i][j];
			if (tok == ' ')
			{
				continue;
			}
			else if (tok == 'J')
			{
				player_josh = createJosh(renderer, {x, y});
				registry.colors.insert(player_josh, {1, 0.8f, 0.8f});
			}
			else if (tok == 'P')
			{
				createPlatform(renderer, {x, y});
			}
			else if (tok == 'Z')
			{
				createZombie(renderer, {x, y}, 0, 50);
			}
			else if (tok == 'F')
			{
				createFood(renderer, {x, y});
			}
			else if (tok == 'B')
			{
				createBullet(renderer, {x, y});
			}
			else if (tok == 'D')
			{
				createDoor(renderer, {x, y});
			}
			else if (tok == 'K')
			{
				createKey(renderer, {x, y});
			}
			else
			{
				printf("Map contains invalid character '%c' at [%d, %d].", tok, i, j);
				return false;
			}
		}
	}
	return true;
}

// Reset the world state to its initial state
void WorldSystem::restart_game()
{
	// Debugging for memory/component leaks
	registry.list_all_components();

	// Reset the game speed
	current_speed = 1.f;
	hp_count = 3;

	// Remove all entities that we created
	// All that have a motion, we could also iterate over all bug, eagles, ... but that would be more cumbersome
	while (registry.motions.entities.size() > 0)
		registry.remove_all_components_of(registry.motions.entities.back());

	// Debugging for memory/component leaks
	registry.list_all_components();

	auto map = loadMap(MAP_PATH + "level1.txt");
	createEntityBaseOnMap(map);

	// player_josh = createJosh(renderer, {window_width_px / 2, window_height_px - 500});

	// registry.colors.insert(player_josh, {1, 0.8f, 0.8f});
	// // test zombie
	// //  TODO: Create a room setup function to call on restart

	// createFood(renderer, vec2(300, window_height_px - 450));
	// createZombie(renderer, vec2(400, 400), 0, 50);
	// createDoor(renderer, vec2(900, window_height_px - 80));
	// createBullet(renderer, vec2(600, window_height_px - 450));
	// createKey(renderer, vec2(400, window_height_px - 450));

	for (int i = 0; i < hp_count; i++)
	{
		createHeart(renderer, vec2(30 + i * create_heart_distance, 20));
	}

	// create one level of platform for now
	// intialize x, the left grid
	// float x = PLATFORM_WIDTH / 2;
	// // fixed y for now, only bottom level of platform
	// float y = window_height_px - PLATFORM_HEIGHT / 2;
	// float a = window_width_px / 2;
	// float b = 300;
	// while (b < a + 200)
	// {
	// 	createPlatform(renderer, vec2(b, window_height_px - 400));
	// 	b += PLATFORM_WIDTH;
	// }
	// float i = x;
	// while (i - PLATFORM_WIDTH < window_width_px)
	// {
	// 	createPlatform(renderer, vec2(i, y));
	// 	i += PLATFORM_WIDTH;
	// }
}
// Compute collisions between entities
void WorldSystem::handle_collisions()
{
	// Loop over all collisions detected by the physics system
	auto &collisionsRegistry = registry.collisions;
	for (uint i = 0; i < collisionsRegistry.components.size(); i++)
	{
		// The entity and its collider
		Entity entity = collisionsRegistry.entities[i];
		Entity entity_other = collisionsRegistry.components[i].other;

		// For now, we are only interested in collisions that involve the chicken
		if (registry.players.has(entity))
		{
			// Player& player = registry.players.get(entity);
			// Checking Player - Deadly collisions
			if (registry.deadlys.has(entity_other) && !registry.deductHpTimers.has(entity))
			{
				if (hp_count == 1)
				{
					// Game over and update the hearts
					uint i = 0;
					while (i < registry.hearts.components.size())
					{
						Entity entity = registry.hearts.entities[i];
						registry.meshPtrs.remove(entity);
						registry.hearts.remove(entity);
						registry.renderRequests.remove(entity);
					}
					for (int i = 0; i < hp_count - 1; i++)
					{
						createHeart(renderer, vec2(30 + i * create_heart_distance, 20));
					}
					// initiate death unless already dying
					if (!registry.deathTimers.has(entity))
					{
						// Scream, reset timer, and make the chicken sink
						registry.deathTimers.emplace(entity);
						// Mix_PlayChannel(-1, chicken_dead_sound, 0);

						Motion &motion = registry.motions.get(entity);
						motion.velocity[0] = 0;
						motion.velocity[1] = 0;

						// change color to red on death
						vec3 death_color = {255.0f, 0.0f, 0.0f};
						vec3 color = registry.colors.get(entity);
						float duration = 1.0f;
						registry.colors.remove(entity);

						// registry.colors.emplace(entity, death_color);
						ColorChange colorChange = {color, death_color, duration, 0.0f};
						registry.colorChanges.emplace(entity, colorChange);
					}
				}
				else
				{
					hp_count = fmax(0, hp_count - 1);
					registry.deductHpTimers.emplace(entity);
					std::cout << "hp count: " << hp_count << std::endl;

					// update hearts
					uint i = 0;
					while (i < registry.hearts.components.size())
					{
						Entity entity = registry.hearts.entities[i];
						registry.meshPtrs.remove(entity);
						registry.hearts.remove(entity);
						registry.renderRequests.remove(entity);
					}
					for (int i = 0; i < hp_count; i++)
					{
						createHeart(renderer, vec2(30 + i * create_heart_distance, 20));
					}
				}
			}
			// Checking Player - Eatable collisions
			else if (registry.eatables.has(entity_other))
			{
				if (registry.foods.has(entity_other))
				{
					// chew, add hp if hp is not full
					registry.remove_all_components_of(entity_other);
					++hp_count;
					std::cout << "hp count: " << hp_count << std::endl;

					uint i = 0;
					while (i < registry.hearts.components.size())
					{
						Entity entity = registry.hearts.entities[i];
						registry.meshPtrs.remove(entity);
						registry.hearts.remove(entity);
						registry.renderRequests.remove(entity);
					}
					for (int i = 0; i < hp_count; i++)
					{
						createHeart(renderer, vec2(30 + i * create_heart_distance, 20));
					}
				}
				else if (registry.bullets.has(entity_other))
				{
					registry.remove_all_components_of(entity_other);
					bullets_count = bullets_count + 10;
					std::cout << "bullets count: " << bullets_count << std::endl;
				}
				else if (registry.keys.has(entity_other))
				{
					registry.remove_all_components_of(entity_other);
					have_key = true;
					std::cout << "have key: " << have_key << std::endl;

					// registry.doors.get(registry.doors.entities[0]).is_open = true;
				}
			}
		}
		else
		{
			if (registry.zombies.has(entity))
			{
				if (registry.platforms.has(entity_other))
				{
					Motion &motion = registry.motions.get(entity);
					motion.velocity.y = 0;
					Gravity &gravity = registry.gravities.get(entity);
				}
			}
		}
	}
	// Remove all collisions from this simulation step
	registry.collisions.clear();
}

// Should the game be over ?
bool WorldSystem::is_over() const
{
	return bool(glfwWindowShouldClose(window));
}

// On key callback
void WorldSystem::on_key(int key, int, int action, int mod)
{
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// TODO A1: HANDLE CHICKEN MOVEMENT HERE
	// key is of 'type' GLFW_KEY_
	// action can be GLFW_PRESS GLFW_RELEASE GLFW_REPEAT
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	float velocity = 200.0f;

	if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE)
	{
		glfwSetWindowShouldClose(window, true);
	}

	// control chicken movement
	if (!registry.deathTimers.has(player_josh))
	{
		if ((action == GLFW_REPEAT || action == GLFW_PRESS) && (key == GLFW_KEY_LEFT || key == GLFW_KEY_A))
		{
			Motion &josh_motion = registry.motions.get(player_josh);
			josh_motion.velocity.x = -200.f * cos(josh_motion.angle);
			josh_motion.velocity.y = 200.f * sin(josh_motion.angle);
			if (josh_motion.scale.x > 0)
			{
				josh_motion.scale.x *= -1;
			}
		}
		if (action == GLFW_RELEASE && (key == GLFW_KEY_LEFT || key == GLFW_KEY_A))
		{
			Motion &josh_motion = registry.motions.get(player_josh);
			josh_motion.velocity.y = 0.f;
			josh_motion.velocity.x = 0.f;
		}
		if ((action == GLFW_REPEAT || action == GLFW_PRESS) && (key == GLFW_KEY_RIGHT || key == GLFW_KEY_D))
		{
			Motion &josh_motion = registry.motions.get(player_josh);
			josh_motion.velocity.x = -200.f * cos(josh_motion.angle - M_PI);
			josh_motion.velocity.y = 00.f * sin(josh_motion.angle - M_PI);
			if (josh_motion.scale.x < 0)
			{
				josh_motion.scale.x *= -1;
			}
		}
		if (action == GLFW_RELEASE && (key == GLFW_KEY_RIGHT || key == GLFW_KEY_D))
		{
			Motion &josh_motion = registry.motions.get(player_josh);
			josh_motion.velocity.y = 0.f;
			josh_motion.velocity.x = 0.f;
		}

		
		// josh jump
		if (action == GLFW_PRESS && key == GLFW_KEY_SPACE && !jumped && registry.motions.get(player_josh).velocity.y == 0.f)
		{
			Motion &josh_motion = registry.motions.get(player_josh);
			josh_motion.velocity.y = -700.f;
			jumped = true;
		}
		else if (action == GLFW_RELEASE && key == GLFW_KEY_SPACE)
		{
			jumped = false;
		}
	}

	// Resetting game
	if (action == GLFW_RELEASE && key == GLFW_KEY_R)
	{
		int w, h;
		glfwGetWindowSize(window, &w, &h);

		restart_game();
	}

	// Debugging
	if (key == GLFW_KEY_D)
	{
		if (action == GLFW_RELEASE)
			debugging.in_debug_mode = false;
		else
			debugging.in_debug_mode = true;
	}

	if (action == GLFW_PRESS && key == GLFW_KEY_B)
	{
		std::fstream file;
		file.open("..\\..\\..\\data\\saving\\save.txt");

		if (file.is_open())
		{
			for (Entity player : registry.players.entities)
			{
				Motion motion = registry.motions.get(player);
				file << "Josh ";
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
		}
		else
		{
			printf("Cannot save because cannot open saving file\n");
		}
		file.close();
	}

	if (action == GLFW_PRESS && key == GLFW_KEY_N)
	{
		std::fstream file;
		file.open("..\\..\\..\\data\\saving\\save.txt");
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
					for (Entity player : registry.players.entities)
					{
						Motion &motion = registry.motions.get(player);
						motion.position.x = std::stof(toks[1]);
						motion.position.y = std::stof(toks[2]);
					}
				}
				else if (toks[0] == "Zombie")
					for (Entity zombie : registry.zombies.entities)
					{
						Motion &motion = registry.motions.get(zombie);
						motion.position.x = std::stof(toks[1]);
						motion.position.y = std::stof(toks[2]);
					}
			}
		}
		else
		{
			printf("Cannot save because cannot open saving file\n");
		}
		file.close();
	}

	// Control the current speed with `<` `>`
	if (action == GLFW_RELEASE && (mod & GLFW_MOD_SHIFT) && key == GLFW_KEY_COMMA)
	{
		current_speed -= 0.1f;
		printf("Current speed = %f\n", current_speed);
	}
	if (action == GLFW_RELEASE && (mod & GLFW_MOD_SHIFT) && key == GLFW_KEY_PERIOD)
	{
		current_speed += 0.1f;
		printf("Current speed = %f\n", current_speed);
	}
	current_speed = fmax(0.f, current_speed);
}

void WorldSystem::on_mouse_move(vec2 mouse_position)
{
}
