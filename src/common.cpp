#include "common.hpp"

// Note, we could also use the functions from GLM but we write the transformations here to show the uderlying math
void Transform::scale(vec2 scale)
{
	mat3 S = { { scale.x, 0.f, 0.f },{ 0.f, scale.y, 0.f },{ 0.f, 0.f, 1.f } };
	mat = mat * S;
}

void Transform::rotate(float radians)
{
	float c = cosf(radians);
	float s = sinf(radians);
	mat3 R = { { c, s, 0.f },{ -s, c, 0.f },{ 0.f, 0.f, 1.f } };
	mat = mat * R;
}

void Transform::translate(vec2 offset)
{
	mat3 T = { { 1.f, 0.f, 0.f },{ 0.f, 1.f, 0.f },{ offset.x, offset.y, 1.f } };
	mat = mat * T;
}

bool gl_has_errors()
{
	GLenum error = glGetError();

	if (error == GL_NO_ERROR) return false;

	while (error != GL_NO_ERROR)
	{
		const char* error_str = "";
		switch (error)
		{
		case GL_INVALID_OPERATION:
			error_str = "INVALID_OPERATION";
			break;
		case GL_INVALID_ENUM:
			error_str = "INVALID_ENUM";
			break;
		case GL_INVALID_VALUE:
			error_str = "INVALID_VALUE";
			break;
		case GL_OUT_OF_MEMORY:
			error_str = "OUT_OF_MEMORY";
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			error_str = "INVALID_FRAMEBUFFER_OPERATION";
			break;
		}

		fprintf(stderr, "OpenGL: %s", error_str);
		error = glGetError();
		assert(false);
	}

	return true;
}

float findDistanceBetween(vec2 pos1, vec2 pos2) {
	float dist = sqrt(pow((pos1.x - pos2.x), 2) + pow((pos1.y - pos2.y), 2));
	return dist;
}
unsigned int Vertex::id_count = 1;
Graph graph;

void Graph::addVertex(Vertex* v) {
	if (vertices.size() < v->id) {
		vertices.push_back(v);
	}
	else {
		printf("Vertex already contained in vertices");
	}
}

void Graph::addEdge(Vertex* v1, Vertex* v2, ACTION action) {
	//v1->adjs.push_back(std::make_pair(v2, action));
	//v2->adjs.push_back(std::make_pair(v1, action));

	v1->adjs[v2] = action;
	//v2->adjs[v1] = action;
}

std::vector<Vertex*> Graph::getVertices() {
	return vertices;
}

