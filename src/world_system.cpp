// Header
#include "world_system.hpp"
#include "world_init.hpp"
#include "menu.hpp"
#include "world_helper.hpp"

// stlib
#include <cassert>
#include <sstream>
#include <iostream>
#include "physics_system.hpp"


#include <fstream>
// #include <ft2build.h>
// #include FT_FREETYPE_H

// Game configuration
const float JOSH_SPEED = 200.f;
const float JOSH_JUMP = 1000.f;
const float KNOCKBACK_DIST = 50.f;

// Threshold to test if one thing is close enough to another
const float DIST_THRESHOLD = 50.f;

const int INITIAL_HP = 3;

// Key flags to track key pressed
bool leftKeyPressed = false;
bool rightKeyPressed = false;

// Animation controls
bool is_josh_moving = false;
int josh_step_counter = 0;

// flag to check if the game is paused
bool paused = false;

//flags to check if the current tutorial text can be escaped
bool can_jump = false;
bool can_move = false;
bool can_shot = false;
bool can_hide = false;
bool can_out = false;
bool can_get_key = false;
int tutorial_index = 0;

// track Menu
std::vector<Entity> buttons = {};
int current_button = 0;

// Create the bug world
WorldSystem::WorldSystem()
	: hp_count(0), bullets_count(0), have_key(false), fps(0.f), fpsCount(0.f), fpsTimer(0.f)
{
	// Seeding rng with random device
	start = std::chrono::system_clock::now();
	renderInfo = false;
	rng = std::default_random_engine(std::random_device()());
}

WorldSystem::~WorldSystem()
{
	// Destroy music components
	if (bg1_music != nullptr)
		Mix_FreeMusic(bg1_music);
	if (bg2_music != nullptr)
		Mix_FreeMusic(bg2_music);
	if (bg3_music != nullptr)
		Mix_FreeMusic(bg3_music);
	if (bg4_music != nullptr)
		Mix_FreeMusic(bg4_music);
	if (doorOpen_music != nullptr)
		Mix_FreeChunk(doorOpen_music);
	if (eat_music != nullptr)
		Mix_FreeChunk(eat_music);
	if (shoot_music != nullptr)
		Mix_FreeChunk(shoot_music);
	if (trush_music != nullptr)
		Mix_FreeChunk(trush_music);
	if (bonus_music != nullptr)
		Mix_FreeChunk(bonus_music);
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
		//     glfwTerminate();
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

	if (SDL_Init(SDL_INIT_AUDIO) < 0)
	{
		fprintf(stderr, "Failed to initialize SDL Audio");
		return nullptr;
	}
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1)
	{
		fprintf(stderr, "Failed to open audio device");
		return nullptr;
	}

	bg1_music = Mix_LoadMUS(audio_path("bg1.wav").c_str());

	if (bg1_music == nullptr)
	{
		fprintf(stderr, "Failed to load sounds %s make sure the data directory is present",
				audio_path("bg1.wav").c_str());
		return nullptr;
	}
	bg2_music = Mix_LoadMUS(audio_path("bg2.wav").c_str());
	if (bg2_music == nullptr)
	{
		fprintf(stderr, "Failed to load sounds %s make sure the data directory is present",
				audio_path("bg2.wav").c_str());
		return nullptr;
	}
	bg3_music = Mix_LoadMUS(audio_path("bg3.wav").c_str());
	if (bg3_music == nullptr)
	{
		fprintf(stderr, "Failed to load sounds %s make sure the data directory is present",
				audio_path("bg3.wav").c_str());
		return nullptr;
	}
	bg4_music = Mix_LoadMUS(audio_path("bg4.wav").c_str());
	if (bg4_music == nullptr)
	{
		fprintf(stderr, "Failed to load sounds %s make sure the data directory is present",
				audio_path("bg4.wav").c_str());
		return nullptr;
	}
	doorOpen_music = Mix_LoadWAV(audio_path("doorOpen.wav").c_str());
	if (doorOpen_music == nullptr)
	{
		fprintf(stderr, "Failed to load sounds %s make sure the data directory is present",
				audio_path("doorOpen.wav").c_str());
		return nullptr;
	}
	eat_music = Mix_LoadWAV(audio_path("eat.wav").c_str());
	if (eat_music == nullptr)
	{
		fprintf(stderr, "Failed to load sounds %s make sure the data directory is present",
				audio_path("eat.wav").c_str());
		return nullptr;
	}
	shoot_music = Mix_LoadWAV(audio_path("shoot.wav").c_str());
	if (shoot_music == nullptr)
	{
		fprintf(stderr, "Failed to load sounds %s make sure the data directory is present",
				audio_path("shoot.wav").c_str());
		return nullptr;
	}
	trush_music = Mix_LoadWAV(audio_path("trush.wav").c_str());
	if (trush_music == nullptr)
	{
		fprintf(stderr, "Failed to load sounds %s make sure the data directory is present",
				audio_path("trush.wav").c_str());
		return nullptr;
	}
	bonus_music = Mix_LoadWAV(audio_path("bonus.wav").c_str());
	if (bonus_music == nullptr)
	{
		fprintf(stderr, "Failed to load sounds %s make sure the data directory is present",
				audio_path("bonus.wav").c_str());
		return nullptr;
	}

	return window;
}

void WorldSystem::init(RenderSystem *renderer_arg, DialogSystem *dialog_arg)
{
	this->renderer = renderer_arg;
	this->dialog = dialog_arg;
	// Playing background music indefinitely
	// !!! TODO: Bring this back in M4
	// Mix_PlayMusic(background_music, -1);
	// fprintf(stderr, "Loaded music\n");

	Mix_VolumeChunk(bonus_music, 30);
	Mix_VolumeMusic(20);

	// Set all states to default
	restart_game();
}

// Linear interpolation
vec3 lerp(vec3 start, vec3 end, float t)
{
	return start * (1 - t) + end * t;
}


// Handle movement related key events
void handleMovementKeys(Entity entity)
{
	if (!registry.deathTimers.has(entity))
	{
		if (registry.motions.has(entity))
		{
			Motion &motion = registry.motions.get(entity);
			// Handle right key
			if (rightKeyPressed)
			{
				
				if (josh_step_counter % 2 == 0)
				{
					registry.renderRequests.get(entity) = {TEXTURE_ASSET_ID::JOSHGUN1,
														   EFFECT_ASSET_ID::TEXTURED,
														   GEOMETRY_BUFFER_ID::SPRITE};
				}
				else
				{
					registry.renderRequests.get(entity) = {TEXTURE_ASSET_ID::JOSHGUN,
														   EFFECT_ASSET_ID::TEXTURED,
														   GEOMETRY_BUFFER_ID::SPRITE};
				}
				if (motion.scale.x < 0 && !registry.players.get(entity).against_wall)
				{
					motion.scale.x *= -1;
				}
				motion.velocity.x = JOSH_SPEED;
			}

			// Handle left key
			if (leftKeyPressed)
			{
				
				if (josh_step_counter % 2 == 0)
				{
					registry.renderRequests.get(entity) = {TEXTURE_ASSET_ID::JOSHGUN1,
														   EFFECT_ASSET_ID::TEXTURED,
														   GEOMETRY_BUFFER_ID::SPRITE};
				}
				else
				{
					registry.renderRequests.get(entity) = {TEXTURE_ASSET_ID::JOSHGUN,
														   EFFECT_ASSET_ID::TEXTURED,
														   GEOMETRY_BUFFER_ID::SPRITE};
				}
				motion.velocity.x = -JOSH_SPEED;
				if (motion.scale.x > 0 && !registry.players.get(entity).against_wall)
				{
					motion.scale.x *= -1;
				}
			}

			// Handle when both key are pressed
			if (!leftKeyPressed ^ rightKeyPressed)
			{
				motion.velocity.x = 0;
			}


		}
	}
}

// Update our game world
bool WorldSystem::step(float elapsed_ms_since_last_update)
{
	//for josh movement animation
	auto end = std::chrono::system_clock::now();

	if(is_josh_moving){
    	auto elasped = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
		josh_step_counter = int(round(elasped)/100000);
		//std::cout<< josh_step_counter<<std::endl;
	}

	//for tutorial
	if(currentLevel == 0 && is_speech_point_index_assigned){
		
		if(!can_jump && tutorial_index == 0){
			tutorial_start = std::chrono::system_clock::now();
			paused = true;
			dialog->createSpeechPointTutorial(speech_point_index, 0); 
			tutorial_index=1;
		}
		if(can_jump && !can_move && tutorial_index == 1){
			auto elasped = std::chrono::duration_cast<std::chrono::microseconds>(end - tutorial_start).count();
			if(round(elasped)/100000 > 10){

				dialog->createSpeechPointTutorial(speech_point_index, 2); 
				// Speech& speech = registry.speech.get(registry.speech.entities[0]);
				// speech.texts.pop();
				// speech.timer.pop();
			
				paused = true;
				tutorial_index = 2;
			}
		}
		if(can_move && !can_shot && tutorial_index == 2){
			auto elasped = std::chrono::duration_cast<std::chrono::microseconds>(end - tutorial_start).count();
			if(round(elasped)/100000>10 && bullets_count>0){
				
				Speech& speech = registry.speech.get(registry.speech.entities[0]);
				speech.texts.pop();
				speech.timer.pop();
				dialog->createSpeechPointTutorial(speech_point_index, 4); 
				paused = true;
				tutorial_index = 3;
			}
		}
		if(can_shot && can_hide && can_out && !can_get_key && tutorial_index == 3){
			Speech& speech = registry.speech.get(registry.speech.entities[0]);
			speech.texts.pop();
			speech.timer.pop();
			dialog->createSpeechPointTutorial(speech_point_index, 6); 
			tutorial_index = 4;
		}
		if(can_shot && can_hide && can_out && !can_get_key && have_key && tutorial_index == 4){
			Speech& speech = registry.speech.get(registry.speech.entities[0]);
			speech.texts.pop();
			speech.timer.pop();
			dialog->createSpeechPointTutorial(speech_point_index, 6); 
			tutorial_index = 5;
		}

	}

	if (paused) {
		for (int i = 0; i < buttons.size(); i++) {

			if (!registry.texts.has(buttons[i])) {
				continue;
			}
			Text& text = registry.texts.get(buttons[i]);
			if (i != current_button) {
				text.color = { 1, 1, 1 };
			}
			else
			{
				text.color = { 1, 1, 0 };
			}
		}

		return true;
	}
	buttons.clear();

	handleMovementKeys(player_josh);
		
	
	// for fps counter
	fpsTimer += elapsed_ms_since_last_update;
	fpsCount++;
	if (fpsTimer >= 1000.0f)
	{
		fpsTimer = 0.0f;
		fps = fpsCount;
		fpsCount = 0;
		std::stringstream windowCaption;
		windowCaption << "Escape from Celestria - FPS Counter: " << fps;
		glfwSetWindowTitle(window, windowCaption.str().c_str());
	}

	// Remove debug info from the last step
	while (registry.debugComponents.entities.size() > 0)
		registry.remove_all_components_of(registry.debugComponents.entities.back());

	// Remove all time up entity
	for (Entity entity : registry.speech.entities)
	{
		// progress timer
		Speech& speech = registry.speech.get(entity);
		speech.counter_ms -= elapsed_ms_since_last_update;
		// remove entity if timer expired
		if (speech.counter_ms < 0 && currentLevel!=0)
		{
			speech.texts.pop();
			speech.timer.pop();
			if (speech.texts.size() > 0) 
			{
				speech.counter_ms = speech.timer.front();
			}
			else
			{
				registry.speech.remove(entity);
			}
		}
	}

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

	for (Entity entity : registry.invincibleTimers.entities)
	{
		// Progress timer
		InvincibleTimer &counter = registry.invincibleTimers.get(entity);
		counter.counter_ms -= elapsed_ms_since_last_update;

		if (counter.counter_ms < min_counter_ms)
		{
			min_counter_ms = counter.counter_ms;
		}

		if (counter.counter_ms < 0)
		{
			//vec3 invincible_color = registry.colorChanges.get(entity).color_start;

			//vec3 color = registry.colors.get(entity);
			//float duration = 0.1f;
			registry.colors.remove(entity);

			// registry.colors.emplace(entity, death_color);
			//ColorChange colorChange = {color, invincible_color, duration, 0.0f};
			//registry.colorChanges.emplace(entity, colorChange);
			registry.colors.emplace(entity, color);
			registry.invincibleTimers.remove(entity);
			return true;
		}
	}

	for (Entity entity : registry.zombies.entities){
		NormalZombie &zombie = registry.zombies.get(entity);
		if(zombie.is_dead){
			zombie.death_counter -=elapsed_ms_since_last_update;
			if(zombie.death_counter>=1000.0){
					registry.renderRequests.get(entity) = {TEXTURE_ASSET_ID::ZOMBIE_DIE1,
										EFFECT_ASSET_ID::TEXTURED,
										GEOMETRY_BUFFER_ID::SPRITE};
					
			}
			if(zombie.death_counter<1.0){
				registry.renderRequests.remove(entity);
				registry.remove_all_components_of(entity);
				
			}
		}
	}

	vec2 p0 = { 50, 150 }; // start point
	vec2 p1 = { 255, 1000 };
	vec2 p2 = { 765, 50 };
	vec2 p3 = { 970, 150 }; // end point

	for (Entity entity: registry.golds.entities){
		if (forward) {
        t += elapsed_ms_since_last_update / 1000.f * 0.2f;
        if (t >= 1) {
            t = 1.0f; 
            forward = false;
        }
    } else {
        t -= elapsed_ms_since_last_update / 1000.f * 0.2f;
        if (t <= 0) {
            t = 0.0f; 
            forward = true; 
        }
    }
		vec2 pos = cubicBezier(p0, p1, p2, p3, t);
		Motion &motion = registry.motions.get(entity);
		motion.position = pos;
		// std::cout << "Forward: " << forward << std::endl;
		// printf("Gold position: %f, %f\n", motion.position.x, motion.position.y);
	}

	// recreate Health base on current hp_count
	for (Entity entity : registry.hearts.entities) {
		registry.remove_all_components_of(entity);
	}


	for (int i = 0; i < hp_count; i++)
	{
		createHeart(renderer, vec2(30 + i * create_heart_distance, 20));
	}

	// recreate Bullets base on bullet_count
	removeSmallBullets(renderer);
	for (int i = 0; i < bullets_count; i++)
	{
		// if (i % 10 == 0)
		// {
		createBulletSmall(renderer, vec2(30 + i * create_bullet_distance, 20 + HEART_BB_HEIGHT));
		// }
	}
	vec2 p0F = { 100, 150 }; // start point
	vec2 p1F = { 345, 1000 };
	vec2 p2F = { 640, 50 };
	vec2 p3F = { 800, 150 }; // end point

	for (Entity entity : registry.fireballs.entities) {
		if (forward) {
			t += elapsed_ms_since_last_update / 1000.f * 0.2f;
			if (t >= 1) {
				t = 1.0f;
				forward = false;
			}
		}
		else {
			t -= elapsed_ms_since_last_update / 1000.f * 0.2f;
			if (t <= 0) {
				t = 0.0f;
				forward = true;
			}
		}
		vec2 pos = cubicBezier(p0F, p1F, p2F, p3F, t);
		Motion& motion = registry.motions.get(entity);
		motion.position = pos;
		// std::cout << "Forward: " << forward << std::endl;
		// printf("Gold position: %f, %f\n", motion.position.x, motion.position.y);
	}


	for (Entity entity : registry.smallKeys.entities) {
		registry.remove_all_components_of(entity);
	}
	if (have_key)
	{
		// show key on screen
		createSmallKey(renderer, vec2(30, SMALL_BULLET_BB_HEIGHT + HEART_BB_HEIGHT + 25));
	}
	if (hp_count <= 0) {
		if (!registry.deathTimers.has(player_josh)) {
			registry.deathTimers.emplace(player_josh);
			// Mix_PlayChannel(-1, chicken_dead_sound, 0);
	
  
			Motion& motion = registry.motions.get(player_josh);
			motion.velocity[0] = 0;
			motion.velocity[1] = 0;

			// change color to red on death
			vec3 death_color = { 255.0f, 0.0f, 0.0f };
			vec3 color = registry.colors.get(player_josh);
			float duration = 1.0f;
			registry.colors.remove(player_josh);

			// registry.colors.emplace(entity, death_color);
			ColorChange colorChange = { color, death_color, duration, 0.0f };
			registry.colorChanges.emplace(player_josh, colorChange);
		}

	}


	return true;
}

vec2 WorldSystem::cubicBezier(vec2 &p0, vec2 &p1, vec2 &p2, vec2 &p3, float t) {
    float t1 = 1.0f - t;
    vec2 pos = p0 * (t1 * t1 * t1) + p1 * (3 * t1 * t1 * t) + p2 * (3 * t1 * t * t) + p3 * (t * t * t);
    return pos;
}

bool WorldSystem::createEntityBaseOnMap(std::vector<std::vector<char>> map)
{
	// clear the graph first
	graph.clear();
	float josh_x = 0, josh_y = 0;
	std::vector<std::pair<float, float>> zombiePositions;
	//Create background entities first
	for (int i = 0; i < map.size(); i++)
	{
		for (int j = 0; j < map[i].size(); j++)
		{
			float x = j * 10;
			float y = i * 10;
			char tok = map[i][j];
			if(tok == 'T'){
				createBackgroundTutorial(renderer, {x, y});
			}
			if (tok == 'O')
			{
				createBackground(renderer, {x, y});
			}
			if (tok == 'Q')
			{
				createBackground2(renderer, {x, y});
			}
			if (tok == 'L')
			{
				createBackground3(renderer, { x, y });
			}
			if (tok == 'M')
			{
				createBackground4(renderer, { x, y });
			}
		}
	}

	//Create all other entities except for background
	for (int i = 0; i < map.size(); i++)
	{
		Vertex* latest = new Vertex(-100, -100);
		graph.addVertex(latest);
		for (int j = 0; j < map[i].size(); j++)
		{
			float x = j * 10;
			float y = i * 10;
			char tok = map[i][j];

			if (tok == ' ' || tok == 'O' || tok == 'Q' || tok == 'L' || tok == 'M'||tok =='T')
			{
				continue;
			}
			else if (tok == 'J')
			{
				josh_x = x;
				josh_y = y;
			}
			else if (tok == 'P')
			{
				Vertex* newV = new Vertex(x, y - PLATFORM_HEIGHT / 2 - (ZOMBIE_BB_HEIGHT * 0.6)/2);

				graph.addVertex(newV);
				if (findDistanceBetween({ newV->x, newV->y }, { latest->x, latest->y }) <= 10) {
					graph.addEdge(newV, latest, ACTION::WALK);
					graph.addEdge(latest, newV, ACTION::WALK);
				}
				latest = newV;
				createPlatform(renderer, {x, y});
			}
			else if (tok == 'Z')
			{
				zombiePositions.push_back({x, y});
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
			else if (tok == 'C')
			{
				createCabinet(renderer, {x, y});
			}
			else if (tok == 'E')
			{
				createObject(renderer, {x, y});
			}
			else if (tok == 'N')
			{
				unsigned int id = 0;
				if (map[i][++j] != ' ')
				{
					id = map[i][j] - '0';
				}
				createNPC(renderer, {x, y}, id);
			
			}
			else if (tok == 'S')
			{
				int index = map[i][++j] - '0';
				createSpeechPoint(renderer, {x, y}, index);
			}
			else if(tok == 'G'){
				createGold(renderer, {x, y});
			}
			else if (tok == '|') {
				createFireball(renderer, { x, y });
			}
			else
			{
				printf("Map contains invalid character '%c' at [%d, %d].", tok, i, j);
				return false;
			}
		}
	}

	// Create zombies in front of other entities
	for (const auto &pos : zombiePositions)
	{
		createZombie(renderer, {pos.first, pos.second});
	}

	// Recreate Josh so that Josh appears at the very front
	player_josh = createJosh(renderer, {josh_x, josh_y});
	registry.colors.insert(player_josh, {1, 0.8f, 0.8f});
	createGraph(currentLevel);
	return true;
}

Mix_Music* getMusicTrack(int level, const std::vector<Mix_Music*>& tracks) {
    if (level >= 1 && level <= tracks.size()) {
        return tracks[level - 1];
    } else {
        return nullptr;
    }
}

// Reset the world state to its initial state
void WorldSystem::restart_game()
{
	// Debugging for memory/component leaks
	registry.list_all_components();

	// Reset the game speed
	current_speed = 1.f;
	hp_count = INITIAL_HP;
	bullets_count = 0;

	// Reset current level
	//currentLevel = 1;

	// Remove all entities that we created
	// All that have a motion, we could also iterate over all bug, eagles, ... but that would be more cumbersome
	while (registry.motions.entities.size() > 0)
		registry.remove_all_components_of(registry.motions.entities.back());
	
	while (registry.speechPoint.entities.size() > 0)
		registry.remove_all_components_of(registry.speechPoint.entities.back());
	

	// Debugging for memory/component leaks
	registry.list_all_components();

	std::vector<Mix_Music*> musicTracks = {bg1_music, bg2_music, bg3_music, bg4_music};
	Mix_Music* currentMusicTrack = getMusicTrack(currentLevel, musicTracks);
    
	if (currentMusicTrack != nullptr) {
        Mix_PlayMusic(currentMusicTrack, -1);
    } else {
        std::cerr << "Error: Music track for level " << currentLevel << " not found." << std::endl;
    }

	auto map = loadMap(map_path() + "level" + std::to_string(currentLevel) + ".txt");
	createEntityBaseOnMap(map);


	//createHelpSign(renderer, vec2(window_width_px - 70, window_height_px - 700));

	for (int i = 0; i < hp_count; i++)
	{
		createHeart(renderer, vec2(30 + i * create_heart_distance, 20));
	}

	dialog->initializeDialog(dialog_path("level"+ std::to_string(currentLevel) + ".txt"));
	have_key = false;
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
			if (registry.deadlys.has(entity_other) && !registry.deductHpTimers.has(entity) && !registry.invincibleTimers.has(entity))
			{
				Motion &motion_p = registry.motions.get(entity);
				Motion motion_z = registry.motions.get(entity_other);
				motion_p.position.x -= (motion_z.position.x - motion_p.position.x) / abs(motion_z.position.x - motion_p.position.x) * KNOCKBACK_DIST;

				if (hp_count == 1 || registry.fireballs.has(entity_other))
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
					// std::cout << "hp count: " << hp_count << std::endl;

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
					Mix_PlayChannel(-1, eat_music, 0);
					registry.remove_all_components_of(entity_other);
					++hp_count;
					// std::cout << "hp count: " << hp_count << std::endl;

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
					bullets_count = bullets_count + 1;
					// std::cout << "bullets count: " << bullets_count << std::endl;

					removeSmallBullets(renderer);
					for (int i = 0; i < bullets_count; i++)
					{
						// if (i % 10 == 0)
						// {
						createBulletSmall(renderer, vec2(30 + i * create_bullet_distance, 20 + HEART_BB_HEIGHT));
						// }
					}
				}
				else if (registry.keys.has(entity_other))
				{
					registry.remove_all_components_of(entity_other);
					have_key = true;
					// std::cout << "have key: " << have_key << std::endl;
					showKeyOnScreen(renderer, have_key);
					// registry.doors.get(registry.doors.entities[0]).is_open = true;
				} 
				else if(registry.golds.has(entity_other)){
					registry.remove_all_components_of(entity_other);
					//registry.invincibleTimers.emplace(entity);
					////vec4 invincible_color = { 1.0f, 1.0f, 0.6f, 0.6f };
					//color = registry.colors.get(entity);
					////float duration = 0.1f;
					//vec4 new_color = { 1.f, 1.f, 0.6f, 0.6f };
					//registry.colors.remove(entity);

					//registry.colors.emplace(entity, new_color);
					hp_count = 0;
					//ColorChange colorChange = {color, invincible_color, duration, 0.0f};
					//registry.colorChanges.emplace(entity, colorChange);
					Mix_PlayChannel(-1, bonus_music, 0);
					Mix_VolumeChunk(bonus_music, 30);
				}
			}
			else if (registry.doors.has(entity_other))
			{
				if (have_key)
				{
					// open the door
					Door &door = registry.doors.get(entity_other);
					door.is_open = true;

					registry.renderRequests.get(entity_other) = {TEXTURE_ASSET_ID::DOOR_CLOSE,
																 EFFECT_ASSET_ID::TEXTURED,
																 GEOMETRY_BUFFER_ID::SPRITE};

					// remove the key from the screen
					showKeyOnScreen(renderer, false);
					if (isNearDoor(player_josh, entity_other))
					{
						Mix_PlayChannel(-1, doorOpen_music, 0);
						currentLevel++;
						render_new_level(currentLevel);
						have_key = false;
						if(currentLevel==0){
							Speech& speech = registry.speech.get(registry.speech.entities[0]);
							speech.texts.pop();
							speech.timer.pop();
						}
					}
				}
			}
			else if (registry.speechPoint.has(entity_other))
			{
				SpeechPoint& speechPoint = registry.speechPoint.get(entity_other);
				if (!speechPoint.isDone)
				{
					if(currentLevel == 0){
						speech_point_index = speechPoint.index;
						is_speech_point_index_assigned = true;
						speechPoint.isDone = true;
					}else{
						printf("speechPoint: %i\n", speechPoint.index);
						std::cout<<speechPoint.index<<std::endl;
						dialog->createSpeechPoint(speechPoint.index);
						speechPoint.isDone = true;
					}
					
				
					
				}
			}
		}
		// Zombie and Bullet collision
		else if (registry.zombies.has(entity))
		{
			NormalZombie &zombie = registry.zombies.get(entity);
			if (registry.shootBullets.has(entity_other)&&!zombie.is_dead)
			{
					//remove bullet render effect, enter 2 frames zombie death animation 
				    // zombie_die_start = std::chrono::system_clock::now();
					registry.renderRequests.get(entity) = {TEXTURE_ASSET_ID::ZOMBIE_DIE,
														EFFECT_ASSET_ID::TEXTURED,
														GEOMETRY_BUFFER_ID::SPRITE};
					registry.remove_all_components_of(entity_other);
					registry.renderRequests.remove(entity_other);
					registry.deadlys.remove(entity);
					zombie.is_dead = true;
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
					// Gravity &gravity = registry.gravities.get(entity);
				}
			}
		}
	}
	// Remove all collisions from this simulation step
	registry.collisions.clear();
}

// Show the key on top left of the screen
void WorldSystem::showKeyOnScreen(RenderSystem *renderer, bool have_key)
{
	if (have_key)
	{
		// show key on screen
		createSmallKey(renderer, vec2(30, SMALL_BULLET_BB_HEIGHT + HEART_BB_HEIGHT + 25));
	}
	else
	{
		// remove key from screen
		uint i = 0;
		while (i < registry.keys.components.size())
		{
			Entity entity = registry.keys.entities[i];
			registry.meshPtrs.remove(entity);
			registry.keys.remove(entity);
			registry.renderRequests.remove(entity);
		}
	}
}



// Render a new level
void WorldSystem::render_new_level(int level)
{
	while (registry.motions.entities.size() > 0)
		registry.remove_all_components_of(registry.motions.entities.back());
	auto map = loadMap(map_path() + "level" + std::to_string(level) + ".txt");
	createEntityBaseOnMap(map);

	if (level == 2)
	{
		Mix_PlayMusic(bg2_music, -1);
	}
	else if (level == 3)
	{
		Mix_PlayMusic(bg3_music, -1);
	} 
	else if (level == 4)
	{
		Mix_PlayMusic(bg4_music, -1);
	}

	if(level == 1){
		//entering level 1
		hp_count = INITIAL_HP;
	}
	for (int i = 0; i < hp_count; i++)
	{	
		createHeart(renderer, vec2(30 + i * create_heart_distance, 20));
	}
	createHelpSign(renderer, vec2(window_width_px - 70, window_height_px - 700));
	if (renderInfo)
	{
		createHelpInfo(renderer, vec2(window_width_px - 515, window_height_px - 350));
	}
	for (int i = 0; i < bullets_count; i++)
	{
		// if (i % 10 == 0)
		// {
		createBulletSmall(renderer, vec2(30 + i * create_bullet_distance, 20 + HEART_BB_HEIGHT));
		// }
	}

	std::string dialog_file = dialog_path("level" + std::to_string(level) + ".txt");
	dialog->initializeDialog(dialog_file);
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

	if(currentLevel == 0 && paused == true){
		if(!can_jump){
			tutorial_start = std::chrono::system_clock::now();
			if(action == GLFW_PRESS && key == GLFW_KEY_SPACE){
				dialog->createSpeechPointTutorial(speech_point_index, 1); 
				Speech& speech = registry.speech.get(registry.speech.entities[0]);
				speech.texts.pop();
				speech.timer.pop();
				can_jump = true;
				paused = false;
				//std::cout<<"1st line is displayed"<<std::endl;
			}
		}else if(!can_move){
			if(action == GLFW_PRESS && (key == GLFW_KEY_A || key == GLFW_KEY_D ||key == GLFW_KEY_RIGHT || key == GLFW_KEY_LEFT)){
				//std::cout<<"can move now"<<std::endl;
				dialog->createSpeechPointTutorial(speech_point_index, 3); 
				Speech& speech = registry.speech.get(registry.speech.entities[0]);
				speech.texts.pop();
				speech.timer.pop();
				can_move = true;
				paused = false;
				
			}
		}else if(!can_shot){
			if (bullets_count>0 && (action == GLFW_REPEAT || action == GLFW_PRESS) && (key == GLFW_KEY_J)){
				dialog->createSpeechPointTutorial(speech_point_index, 5); 
				Speech& speech = registry.speech.get(registry.speech.entities[0]);
				speech.texts.pop();
				speech.timer.pop();
				can_shot = true;
				paused = false;
				tutorial_index = 3;
				//std::cout<<"shot now"<<std::endl;
			}
		}


		// }else if(!can_hide){
			
		// }
	}
	if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE)
	{
		//glfwSetWindowShouldClose(window, true);
		paused = !paused;
		if (paused) {
			renderPauseMenu();
			for (Entity entity : registry.menus.entities) {
				auto& me = registry.menus.get(entity);
				if (me.func != MENU_FUNC::ALL) {
					buttons.push_back(entity);
				}
			}
		}
		else {
			for (Entity entity : registry.menus.entities) {
				registry.remove_all_components_of(entity);
			}
			buttons.clear();
		}

	}

	if (isJoshHidden && key != GLFW_KEY_H)
	{
		return;
	}
	
	if (key == GLFW_KEY_LEFT || key == GLFW_KEY_A)
	{
		if (action == GLFW_PRESS || action == GLFW_REPEAT)
		{
			is_josh_moving = true;
			//josh_step_counter++;
			leftKeyPressed = true;
		}
		else if (action == GLFW_RELEASE)
		{
			is_josh_moving = false;
			leftKeyPressed = false;
		}
	}
	if (key == GLFW_KEY_RIGHT || key == GLFW_KEY_D)
	{
		if (action == GLFW_PRESS || action == GLFW_REPEAT)
		{
			is_josh_moving = true;
			//josh_step_counter++;
			rightKeyPressed = true;
		}
		else if (action == GLFW_RELEASE)
		{
			is_josh_moving = false;
			rightKeyPressed = false;
		}
	}
	
	
	if (!registry.deathTimers.has(player_josh))
	{

		if ((action == GLFW_REPEAT || action == GLFW_PRESS) && (key == GLFW_KEY_J))
		{
			registry.renderRequests.get(player_josh) = {TEXTURE_ASSET_ID::JOSHGUN1,
														EFFECT_ASSET_ID::TEXTURED,
														GEOMETRY_BUFFER_ID::SPRITE};

			vec2 josh_pos = registry.motions.get(player_josh).position;

			if (bullets_count > 0)
			{
				Mix_PlayChannel(-1, shoot_music, 0);
				if (registry.motions.get(player_josh).scale.x > 0)
				{
					Entity bullet = createBulletShoot(renderer, vec2(josh_pos.x + JOSH_BB_WIDTH / 2, josh_pos.y));
					registry.eatables.remove(bullet);
					Motion &motion = registry.motions.get(bullet);
					motion.scale = vec2(20.0, 20.0);
					motion.velocity.x = 500.0;
					bullets_count--;
				}
				else
				{
					Entity bullet = createBulletShoot(renderer, vec2(josh_pos.x - JOSH_BB_WIDTH / 2, josh_pos.y));
					registry.eatables.remove(bullet);
					Motion &motion = registry.motions.get(bullet);
					motion.scale = vec2(-20.0, 20.0);
					motion.velocity.x = -500.0;
					bullets_count--;
				}
			}

			removeSmallBullets(renderer);
			for (int i = 0; i < bullets_count; i++)
			{
				createBulletSmall(renderer, vec2(30 + i * create_bullet_distance, 20 + HEART_BB_HEIGHT));
			}
		}
		
		if (action == GLFW_PRESS && key == GLFW_KEY_SPACE &&!jumped && registry.motions.get(player_josh).velocity.y == 0.f)
		{
			// josh jump
			Motion &josh_motion = registry.motions.get(player_josh);
			josh_motion.velocity.y = -JOSH_JUMP;
			jumped = true;
			//registry.players.get(player_josh).standing = false;
		}
		else if (action == GLFW_RELEASE && key == GLFW_KEY_SPACE)
		{
			jumped = false;
		}
	}

	if (action == GLFW_PRESS && key == GLFW_KEY_H)
	{
		//todo: remove later for debug purpose only
		can_hide=true;
		can_out=true;
		if (!isJoshHidden)
		{
			for (Entity entity : registry.cabinets.entities)
			{
				if (isNearCabinet(player_josh, entity))
				{
					// hide josh
					joshPosition = registry.motions.get(player_josh).position;
					joshScale = registry.motions.get(player_josh).scale;
					hideJosh(renderer);
					isJoshHidden = true;
					if(currentLevel==0){
						can_hide=true;
					}
					
					break;
				}
			}
		}
		else
		{
			// show josh
			player_josh = createJosh(renderer, joshPosition);
			registry.motions.get(player_josh).scale = joshScale;
			registry.colors.insert(player_josh, {1, 0.8f, 0.8f});
			isJoshHidden = false;
			if(currentLevel==0){
				can_out = true;
			}
			
			uint i = 0;
			while (i < registry.hearts.components.size())
			{
				Entity entity = registry.hearts.entities[i];
				registry.meshPtrs.remove(entity);
				registry.hearts.remove(entity);
				registry.renderRequests.remove(entity);
			}
			for (int i = 0; i < hp_count ; i++)
			{
				createHeart(renderer, vec2(30 + i * create_heart_distance, 20));
			}
		}
	}

	// Resetting game
	if (action == GLFW_RELEASE && key == GLFW_KEY_R)
	{
		int w, h;
		glfwGetWindowSize(window, &w, &h);

		restart_game();
	}

	if (action == GLFW_PRESS && key == GLFW_KEY_P)
	{
		Motion motion = registry.motions.get(player_josh);
		printf("Josh curr_loc: %f, %f\n", motion.position.x, motion.position.y);
	}

	// Handle Menu
	if (paused) {

		if (action == GLFW_RELEASE && key == GLFW_KEY_DOWN) {
			if (current_button == buttons.size() - 1) {
				current_button = 0;
			}
			else {
				current_button++;
			}
		}
		if (action == GLFW_RELEASE && key == GLFW_KEY_UP) {
			if (current_button == 0) {
				current_button = buttons.size() - 1;
			}
			else {
				current_button--;
			}
		}
		if (action == GLFW_RELEASE && key == GLFW_KEY_ENTER) {
			paused = handleButtonEvents(buttons[current_button], renderer, window, have_key, hp_count, bullets_count);
			for (Entity entity : registry.players.entities) {
				player_josh = entity;
			}
		}
	}

	// Debugging
	if (key == GLFW_KEY_K)
	{
		if (action == GLFW_RELEASE)
			debugging.in_debug_mode = false;
		else
			debugging.in_debug_mode = true;
	}

	// Control the current speed with `<` `>`
	if (action == GLFW_RELEASE && (mod & GLFW_MOD_SHIFT) && key == GLFW_KEY_COMMA)
	{
		//current_speed -= 0.1f;
		//printf("Current speed = %f\n", current_speed);
		if (currentLevel > 0) {
			currentLevel--;
			restart_game();
		}
	}
	if (action == GLFW_RELEASE && (mod & GLFW_MOD_SHIFT) && key == GLFW_KEY_PERIOD)
	{
		current_speed += 0.1f;
		printf("Current speed = %f\n", current_speed);
		if (currentLevel < 4) {
			currentLevel++;
			restart_game();
		}
	}
	current_speed = fmax(0.f, current_speed);
}

void WorldSystem::on_mouse_move(vec2 mouse_position)
{
}

bool WorldSystem::isNearCabinet(Entity player, Entity cabinet)
{
	vec2 playerPos = registry.motions.get(player).position;
	vec2 cabinetPos = registry.motions.get(cabinet).position;
	return findDistanceBetween(playerPos, cabinetPos) <= DIST_THRESHOLD;
}

// check if player is near the door
bool WorldSystem::isNearDoor(Entity player, Entity door)
{
	vec2 playerPos = registry.motions.get(player).position;
	vec2 doorPos = registry.motions.get(door).position;
	return findDistanceBetween(playerPos, doorPos) <= DIST_THRESHOLD;
}

void WorldSystem::hideJosh(RenderSystem *renderer)
{
	// remove josh from screen
	Mix_PlayChannel(-1, trush_music, 0);
	Entity entity = registry.players.entities[0];
	registry.remove_all_components_of(entity);
	leftKeyPressed = false;
	rightKeyPressed = false;
}

void WorldSystem::removeSmallBullets(RenderSystem *renderer)
{
	uint i = 0;
	while (i < registry.smallBullets.components.size())
	{
		Entity entity = registry.smallBullets.entities[i];
		registry.remove_all_components_of(entity);
	}
}

bool WorldSystem::is_paused() const {
	return paused;
}
