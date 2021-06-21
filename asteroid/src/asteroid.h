#pragma once
#include "wrapper_glfw.h"
#include "icosphere.h"
#include <glm/glm.hpp>
#include <vector>

class Asteroid : Icosphere {
public:
	Asteroid(int crater_density, float scale);
	~Asteroid() {}

	void draw();
	void gen_craters(float scale);
	void re_scale_craters(float scale);

	float radius;

private:
	GLuint shaderStorageObject;
	struct crater {
		glm::vec4 center;
		float size;
		float smoothness;
		float depth;
		float rim_width;
		float rim_steepness;
		uint32_t pad[3];    // Padding for shader storage buffer
	};
	std::vector<crater> craters;
	int crater_density;
	float current_scale;
};
