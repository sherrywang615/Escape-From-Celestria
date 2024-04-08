#pragma once
#include "common.hpp"
#include <vector>
#include <unordered_map>
#include <queue>
#include "../ext/stb_image/stb_image.h"

// Player component
struct Player
{
	bool standing = 0;
	bool against_wall = 0;
};

struct Deadly
{
};

// normal zombies
struct NormalZombie
{

	// zombie state 0 = unalerted, 1 = alerted
	int is_alerted = 0;
	// walking range of zombie based on initial position
	std::vector<double> walking_bound = {0, 0};
	float walking_range = 300;
	vec2 sensing_range = { 300, 100 };
	DIRECTION face = DIRECTION::RIGHT;
	// memory decides how long does it takes for an alerted zombie turn back into unalerted after losing the player in sight
	float memory = 3000;
	float alerted_speed = 100;
	bool is_jumping = 0;
	bool is_dead = false;
	float death_counter = 3000.0;

};

struct Platform
{
	// height and width
	// vec2 size = {300,300};
	vec2 position = {0, 0};
	vec2 scale = {10, 10};
};

struct Door
{
	bool is_open = false;
};

// Bug and Chicken have a soft shell
struct Eatable
{
};

struct Food
{
};

struct Bullet
{
};

struct Key
{
};

struct Text {
	std::string text = "";
	vec3 color = { 0.0f, 0.0f, 255.0f };
};

struct MenuElement {
	MENU_FUNC func = MENU_FUNC::ALL;
};

// All data relevant to the shape and motion of entities
struct Motion
{
	vec2 position = {0, 0};
	float angle = 0;
	vec2 velocity = {0, 0};
	vec2 scale = {10, 10};
};

struct Gravity
{
};

// Stucture to store collision information
struct Collision
{
	// Note, the first object is stored in the ECS container.entities
	Entity other; // the second object involved in the collision
	Collision(Entity &other) { this->other = other; };
};

// Data structure for toggling debug mode
struct Debug
{
	bool in_debug_mode = 0;
	bool in_freeze_mode = 0;
};
extern Debug debugging;

// Sets the brightness of the screen
struct ScreenState
{
	float darken_screen_factor = -1;
};

// A struct to refer to debugging graphics in the ECS
struct DebugComponent
{
	// Note, an empty struct has size 1
};

// A timer that will be associated to dying chicken
struct DeathTimer
{
	float counter_ms = 800;
};

// Single Vertex Buffer element for non-textured meshes (coloured.vs.glsl & chicken.vs.glsl)
struct ColoredVertex
{
	vec3 position;
	vec3 color;
};

// Single Vertex Buffer element for textured sprites (textured.vs.glsl)
struct TexturedVertex
{
	vec3 position;
	vec2 texcoord;
};

// Mesh datastructure for storing vertex and index buffers
struct Mesh
{
	static bool loadFromOBJFile(std::string obj_path, std::vector<ColoredVertex> &out_vertices, std::vector<uint16_t> &out_vertex_indices, vec2 &out_size);
	vec2 original_size = {1, 1};
	std::vector<ColoredVertex> vertices;
	std::vector<uint16_t> vertex_indices;
};

// When a direction key is released, the chicken will move towards its current direction with decreasing speed (momentum)
struct Sliding
{
	vec2 velocity = {0, 0};
};

// change josh's color on death
struct ColorChange
{
	vec3 color_start;
	vec3 color_end;
	float color_duration;
	float color_time_elapsed;
};

struct LinearMovement
{
	vec2 pos_start;
	vec2 pos_end;
	float duration;
	float time_elapsed;
};

struct DeductHpTimer
{
	float counter_ms = 900;
};

struct Character
{
	unsigned int TextureID; // ID handle of the glyph texture
	glm::ivec2 Size;		// Size of glyph
	glm::ivec2 Bearing;		// Offset from baseline to left/top of glyph
	unsigned int Advance;	// Offset to advance to next glyph
	char character;
};

struct Heart
{
};

struct Cabinet
{
};

struct SmallBullet
{
};

struct ShootBullet
{
};

struct NonPlayerCharacter
{
	int id;
};

struct Speech
{
	std::queue<std::pair<Entity, std::string>> texts;
	std::queue<float> timer;
	float counter_ms;
};

struct SpeechPoint
{
	bool isDone = false;
	unsigned int index;
};

struct Timer
{
	float counter_ms;
};

struct Gold{
	
};

struct Fireball {

};


struct Spikeball {
	int currDir = -1;
	int prevDir = -1;
	//direction:: left:0 right:1 up:2 down:3
};


struct InvincibleTimer
{
	float counter_ms = 5000;
};

struct SmallKey {
};

/**
 * The following enumerators represent global identifiers refering to graphic
 * assets. For example TEXTURE_ASSET_ID are the identifiers of each texture
 * currently supported by the system.
 *
 * So, instead of referring to a game asset directly, the game logic just
 * uses these enumerators and the RenderRequest struct to inform the renderer
 * how to structure the next draw command.
 *
 * There are 2 reasons for this:
 *
 * First, game assets such as textures and meshes are large and should not be
 * copied around as this wastes memory and runtime. Thus separating the data
 * from its representation makes the system faster.
 *
 * Second, it is good practice to decouple the game logic from the render logic.
 * Imagine, for example, changing from OpenGL to Vulkan, if the game logic
 * depends on OpenGL semantics it will be much harder to do the switch than if
 * the renderer encapsulates all asset data and the game logic is agnostic to it.
 *
 * The final value in each enumeration is both a way to keep track of how many
 * enums there are, and as a default value to represent uninitialized fields.
 */

enum class TEXTURE_ASSET_ID
{
	FOOD = 0,
	JOSH,
	JOSH1,
	JOSHGUN,
	JOSHGUN1,
	ZOMBIE,
	ZOMBIE1,
	ZOMBIE2,
	ZOMBIE_DIE,
	ZOMBIE_DIE1,
	PLATFORM,
	BULLET,
	DOOR,
	DOOR_CLOSE,
	KEY,
	HEART,

	HELP_INFO,
	HELP_SIGN,
	CABINET,
	BACKGROUND_TUTORIAL,
	BACKGROUND,
	BACKGROUND2,
	BACKGROUND3,
	BACKGROUND4,
	BACKGROUND6,
	BgEnd,
	BARREL,
	MENU,
	TUTOTRIAL,
	NPC1,
	NPC2,
	NPC3,
	NPC4,
	ALICE,
	GOLD1,
	BACKGROUND5,
	TITLE,
	BACKGROUND7,
	GROUNDVERT,
	SPIKEBALL,
	BACKGROUNDSTART,
	TEXTURE_COUNT
};
const int texture_count = (int)TEXTURE_ASSET_ID::TEXTURE_COUNT;

enum class EFFECT_ASSET_ID
{
	COLOURED = 0,
	EGG = COLOURED + 1,
	JOSH = EGG + 1,
	TEXTURED = JOSH + 1,
	WIND = TEXTURED + 1,
	// FONT = WIND + 1,
	EFFECT_COUNT = WIND + 1
};
const int effect_count = (int)EFFECT_ASSET_ID::EFFECT_COUNT;

enum class GEOMETRY_BUFFER_ID
{
	JOSH = 0,
	SPRITE = JOSH + 1,
	EGG = SPRITE + 1,
	DEBUG_LINE = EGG + 1,
	SCREEN_TRIANGLE = DEBUG_LINE + 1,
	GEOMETRY_COUNT = SCREEN_TRIANGLE + 1

};
const int geometry_count = (int)GEOMETRY_BUFFER_ID::GEOMETRY_COUNT;

struct RenderRequest
{
	TEXTURE_ASSET_ID used_texture = TEXTURE_ASSET_ID::TEXTURE_COUNT;
	EFFECT_ASSET_ID used_effect = EFFECT_ASSET_ID::EFFECT_COUNT;
	GEOMETRY_BUFFER_ID used_geometry = GEOMETRY_BUFFER_ID::GEOMETRY_COUNT;
};
