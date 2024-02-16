// Header
#include "world_system.hpp"
#include "world_init.hpp"

// stlib
#include <cassert>
#include <sstream>
#include <iostream>
#include "physics_system.hpp"

// Game configuration
const size_t MAX_EAGLES = 15;
const size_t MAX_BUG = 5;
const size_t EAGLE_DELAY_MS = 2000 * 3;
const size_t BUG_DELAY_MS = 5000 * 3;

// Create the bug world
WorldSystem::WorldSystem()
	: points(0), next_eagle_spawn(0.f), next_bug_spawn(0.f)
{
	// Seeding rng with random device
	rng = std::default_random_engine(std::random_device()());
}

WorldSystem::~WorldSystem()
{
	// Destroy music components
	//if (background_music != nullptr)
	//	Mix_FreeMusic(background_music);
	//if (chicken_dead_sound != nullptr)
	//	Mix_FreeChunk(chicken_dead_sound);
	//if (chicken_eat_sound != nullptr)
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
	//if (SDL_Init(SDL_INIT_AUDIO) < 0)
	//{
	//	fprintf(stderr, "Failed to initialize SDL Audio");
	//	return nullptr;
	//}
	//if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1)
	//{
	//	fprintf(stderr, "Failed to open audio device");
	//	return nullptr;
	//}

	//background_music = Mix_LoadMUS(audio_path("music.wav").c_str());
	//chicken_dead_sound = Mix_LoadWAV(audio_path("chicken_dead.wav").c_str());
	//chicken_eat_sound = Mix_LoadWAV(audio_path("chicken_eat.wav").c_str());

	//if (background_music == nullptr || chicken_dead_sound == nullptr || chicken_eat_sound == nullptr)
	//{
	//	fprintf(stderr, "Failed to load sounds\n %s\n %s\n %s\n make sure the data directory is present",
	//			audio_path("music.wav").c_str(),
	//			audio_path("chicken_dead.wav").c_str(),
	//			audio_path("chicken_eat.wav").c_str());
	//	return nullptr;
	//}

	return window;
}

void WorldSystem::init(RenderSystem *renderer_arg)
{
	this->renderer = renderer_arg;
	// Playing background music indefinitely
	// !!! TODO: Bring this back in M4
	//Mix_PlayMusic(background_music, -1);
	//fprintf(stderr, "Loaded music\n");

	// Set all states to default
	restart_game();
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

	auto& zombie_registry = registry.zombies;
	for (int i = (int)zombie_registry.components.size() - 1; i >= 0; --i)
	{
		NormalZombie& zombie = zombie_registry.components[i];
		double xPosition = registry.motions.get(zombie_registry.entities[i]).position.x;
		// if zombie state == unalert (0), then check if it has reached the edge of its walking range and switch direction if so
		if (zombie.state == 0 && (xPosition <= zombie.walking_range[0] || xPosition >= zombie.walking_range[1])) {
			registry.motions.get(zombie_registry.entities[i]).velocity.x *= -1;
			registry.motions.get(zombie_registry.entities[i]).scale[0] *= -1;
		}
	}

	
	// Spawning new eagles
	// Do we need eagles???
	//next_eagle_spawn -= elapsed_ms_since_last_update * current_speed;
	//if (registry.deadlys.components.size() <= MAX_EAGLES && next_eagle_spawn < 0.f)
	//{
	//	// Reset timer
	//	next_eagle_spawn = (EAGLE_DELAY_MS / 2) + uniform_dist(rng) * (EAGLE_DELAY_MS / 2);
	//	// Create eagle with random initial position
	//	createEagle(renderer, vec2(50.f + uniform_dist(rng) * (window_width_px - 100.f), -100.f));
	//}
	
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
	screen.darken_screen_factor = 1 - min_counter_ms / 3000;

	return true;
}

// Reset the world state to its initial state
void WorldSystem::restart_game()
{
	// Debugging for memory/component leaks
	registry.list_all_components();
	printf("Restarting\n");

	// Reset the game speed
	current_speed = 1.f;

	// Remove all entities that we created
	// All that have a motion, we could also iterate over all bug, eagles, ... but that would be more cumbersome
	while (registry.motions.entities.size() > 0)
		registry.remove_all_components_of(registry.motions.entities.back());

	// Debugging for memory/component leaks
	registry.list_all_components();


	// Create a new chicken
	
	registry.colors.insert(player_chicken, {1, 0.8f, 0.8f});

	player_josh = createJosh(renderer, {window_width_px / 2, window_height_px-600});

	registry.colors.insert(player_josh, {1, 0.8f, 0.8f});
	//test zombie
	// TODO: Create a room setup function to call on restart
	//createZombie(renderer, vec2(400, 400));

	// create one level of platform for now
	// intialize x, the left grid
	float x = PLATFORM_WIDTH/2; 
	// fixed y for now, only bottom level of platform
	float y = window_height_px - PLATFORM_HEIGHT/2;
	float a = window_width_px/2;
	float b = 300;
	while (b < a + 200) {
		createPlatform(renderer, vec2(b, window_height_px - 400));
		b += PLATFORM_WIDTH;
	}
	float i = x;
	while(i-PLATFORM_WIDTH<window_width_px - 200){
		createPlatform(renderer, vec2(i, y));
		i +=PLATFORM_WIDTH;
	}
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
			if (registry.deadlys.has(entity_other))
			{
				// initiate death unless already dying
				if (!registry.deathTimers.has(entity))
				{
					// Scream, reset timer, and make the chicken sink
					registry.deathTimers.emplace(entity);
					Mix_PlayChannel(-1, chicken_dead_sound, 0);

					// !!! TODO A1: change the chicken orientation and color on death
					Motion &motion = registry.motions.get(entity);
					// Make chicken upside down (degree = 270)
					motion.angle = 4.71238898f;
					motion.velocity[0] = 0;
					motion.velocity[1] = 200;
					// Change color to red (255, 0, 0)
					vec3 death_color = {255.0f, 0.0f, 0.0f};
					vec3 color = registry.colors.get(entity);
					registry.colors.remove(entity);
					registry.colors.emplace(entity, death_color);
				}
			}
			// Checking Player - Eatable collisions
			else if (registry.eatables.has(entity_other))
			{
				if (!registry.deathTimers.has(entity))
				{
					// chew, count points, and set the LightUp timer
					registry.remove_all_components_of(entity_other);
					Mix_PlayChannel(-1, chicken_eat_sound, 0);
					++points;
				}
			}
		}
		else {	
			if (registry.zombies.has(entity)) {
				if (registry.platforms.has(entity_other)) {
					Motion& motion = registry.motions.get(entity);
					motion.velocity.y = 0;
					Gravity& gravity = registry.gravities.get(entity);
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
			if(josh_motion.scale.x > 0){
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
			if(josh_motion.scale.x < 0){
				josh_motion.scale.x *= -1;
			}
		}
		if (action == GLFW_RELEASE && (key == GLFW_KEY_RIGHT || key == GLFW_KEY_D))
		{
			Motion &josh_motion = registry.motions.get(player_josh);
			josh_motion.velocity.y = 0.f;
			josh_motion.velocity.x = 0.f;
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
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// TODO A1: HANDLE CHICKEN ROTATION HERE
	// xpos and ypos are relative to the top-left of the window, the chicken's
	// default facing direction is (1, 0)
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// for (Entity entity : registry.players.entities) {
	// 	if (registry.deathTimers.has(entity)) {
	// 		return;
	// 	}
	// 	Motion& motion = registry.motions.get(entity);
	// 	vec2 chicken_pos = motion.position;
	// 	float x = chicken_pos[0] - mouse_position[0];
	// 	float y = chicken_pos[1] - mouse_position[1];
	// 	motion.angle = atan2(y, x);
	// }
}
