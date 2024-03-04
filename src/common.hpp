#pragma once

// stlib
#include <fstream> // stdout, stderr..
#include <string>
#include <tuple>
#include <vector>
#include <map>

// glfw (OpenGL)
#define NOMINMAX
#include <gl3w.h>
#include <GLFW/glfw3.h>

// The glm library provides vector and matrix operations as in GLSL
#include <glm/vec2.hpp>				// vec2
#include <glm/ext/vector_int2.hpp>  // ivec2
#include <glm/vec3.hpp>             // vec3
#include <glm/mat3x3.hpp>           // mat3
using namespace glm;

#include "tiny_ecs.hpp"

// Simple utility functions to avoid mistyping directory name
// audio_path("audio.ogg") -> data/audio/audio.ogg
// Get defintion of PROJECT_SOURCE_DIR from:
#include "../ext/project_path.hpp"
inline std::string data_path() { return std::string(PROJECT_SOURCE_DIR) + "data"; };
inline std::string shader_path(const std::string& name) {return std::string(PROJECT_SOURCE_DIR) + "/shaders/" + name;};
inline std::string textures_path(const std::string& name) {return data_path() + "/textures/" + std::string(name);};
inline std::string audio_path(const std::string& name) {return data_path() + "/audio/" + std::string(name);};
inline std::string mesh_path(const std::string& name) {return data_path() + "/meshes/" + std::string(name);};

const int window_width_px = 1020;
const int window_height_px = 700;

const int width = window_width_px / 10;
const int height = window_height_px / 10;

// const std::string MAP_PATH = "..//..//..//data//maps//";
 const std::string MAP_PATH = "../../../data/maps/";


#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

// The 'Transform' component handles transformations passed to the Vertex shader
// (similar to the gl Immediate mode equivalent, e.g., glTranslate()...)
// We recomment making all components non-copyable by derving from ComponentNonCopyable
struct Transform {
	mat3 mat = { { 1.f, 0.f, 0.f }, { 0.f, 1.f, 0.f}, { 0.f, 0.f, 1.f} }; // start with the identity
	void scale(vec2 scale);
	void rotate(float radians);
	void translate(vec2 offset);
};

bool gl_has_errors();

enum class DIRECTION {
	RIGHT = 0,
	TOP = RIGHT + 1,
	LEFT = TOP + 1,
	BOT = LEFT + 1,
	ALL = BOT + 1         // ALL is used to check if one object is colliding at all
};

enum class ACTION {
	WALK = 0,
	JUMP = WALK + 1
};

float findDistanceBetween(vec2 pos1, vec2 pos2);

struct Vertex {
	unsigned int id;
	static unsigned int id_count;
public:
	float x;
	float y;
	std::unordered_map<Vertex*, ACTION> adjs;
	Vertex(float _x, float _y)
	{
		id = id_count++;
		x = _x;
		y = _y;
	}
};


struct Graph {

private:
	std::vector<Vertex*> vertices;

public:
	void addVertex(Vertex* v);
	void addEdge(Vertex* v1, Vertex* v2, ACTION action);
	std::vector<Vertex*> getVertices();
};

extern Graph graph;


