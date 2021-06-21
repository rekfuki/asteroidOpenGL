#pragma once
#include "wrapper_glfw.h"
#include "icosphere.h"
#include <vector>
#include <glm/glm.hpp>

class AsteroidField : Icosphere {
public:
	AsteroidField();
	~AsteroidField() {};

	void draw(bool is_shadow_pass = false);
private:
	GLuint instanced_array_buffer_matrices;
	GLuint instanced_array_buffer_noise;
	GLuint instanced_array_buffer_scale;

	Icosphere base_sphere;
	glm::mat4* modelMatrices;
	float size_distribution;
	int field_x;
	int field_y;

	int num_asteroids;
};
