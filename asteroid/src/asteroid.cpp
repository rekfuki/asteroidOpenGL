#include "asteroid.h"

#include <random>
#include <cmath>
#include <vector>
#include <iostream>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include "utils.cpp"

Asteroid::Asteroid(int crater_density, float scale) : Icosphere(10.0f, 5.0f) {
	this->crater_density = crater_density;
	this->current_scale = scale;
	glGenBuffers(1, &shaderStorageObject);
	gen_craters(scale);
}

void Asteroid::re_scale_craters(float scale) {
	for (auto& crater : craters) {
		crater.size = crater.size / this->current_scale * scale;
		crater.smoothness = crater.smoothness / this->current_scale * scale;
		crater.depth = crater.depth / this->current_scale * scale;
	}
	this->current_scale = scale;

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, shaderStorageObject);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(crater) * craters.size(), &craters[0], GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, shaderStorageObject);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void Asteroid::gen_craters(float scale) {
	if (crater_density > 0) {
		glm::vec2 crater_size_min_max = glm::vec2(0.05f, 0.2f);
		glm::vec2 smooth_min_max = glm::vec2(0.1f, 1.0f);
		float size_distribution = 0.8f;

		std::random_device rd{};
		std::mt19937 gen{ rd() };

		std::uniform_int_distribution<int> d{ 0, getVertexCount() - 3 };

		craters = std::vector<crater>();
		for (int i = 0; i < crater_density; i++) {
			int random_vertex = d(gen);
			glm::vec4 xyz = glm::vec4{
				vertices.at(random_vertex * 3),
				vertices.at(random_vertex * 3 + 1),
				vertices.at(random_vertex * 3 + 2),
				1,
			};

			float k = biasedDistribution(size_distribution);

			float size = glm::mix(crater_size_min_max.x, crater_size_min_max.y, k) * scale;
			float depth = glm::mix(-1.2f, -0.2f, k + biasedDistribution(0.3f)) * scale;
			float smoothness = glm::mix(smooth_min_max.x, smooth_min_max.y, 1.0f - k) * scale;
			float rim_width = 1.6;
			float rim_steepness = 0.1;
			crater crater = { xyz, size, smoothness, depth, rim_width, rim_steepness };

			craters.push_back(crater);
		}

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, shaderStorageObject);
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(crater) * craters.size(), &craters[0], GL_STATIC_DRAW);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, shaderStorageObject);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}
}

void Asteroid::draw() {
	Icosphere::draw(2);
}