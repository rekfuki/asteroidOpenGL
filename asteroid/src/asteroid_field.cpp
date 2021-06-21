#include "asteroid_field.h"
#include "utils.cpp"
#include "glm/gtc/matrix_transform.hpp"
#include <glm/glm.hpp>

#include <random>
#include <cmath>
#include <iostream>
#include <vector>

AsteroidField::AsteroidField() {
	this->base_sphere = Icosphere(1.0f, 2.0f);
	this->size_distribution = size_distribution;

	this->num_asteroids = 600;
	modelMatrices = new glm::mat4[this->num_asteroids];
	std::vector<glm::vec3> noiseVectors = std::vector<glm::vec3>();
	std::vector<float> scaleFactors = std::vector<float>();

	std::random_device rd{};
	std::mt19937 gen{ rd() };

	float radius = 50.0;
	float offset = 15.0;

	std::uniform_real_distribution<float> noise_distribution{ -1,  1 };
	std::uniform_real_distribution<float> rotation_distribution{ -180,  180 };
	std::uniform_real_distribution<float> displacement_distribution{ -offset,  offset };
	std::uniform_real_distribution<float> scale_distribution{ 0.20,  0.5 };

	// Code adapted from
	// https://learnopengl.com/Advanced-OpenGL/Instancing
	for (unsigned int i = 0; i < this->num_asteroids; i++) {
		glm::mat4 model = glm::mat4(1.0f);

		float angle = ((float)i / (float)this->num_asteroids * 360.0f);
		float displacement = displacement_distribution(gen);
		float x = sin(angle) * radius + displacement_distribution(gen);
		float y = displacement_distribution(gen) * 0.2f; // keep height of field smaller compared to width of x and z
		float z = cos(angle) * radius + displacement_distribution(gen);
		model = glm::translate(model, glm::vec3(x, y, z));

		float scale = scale_distribution(gen);
		model = glm::scale(model, glm::vec3(scale));

		modelMatrices[i] = model;

		noiseVectors.push_back(glm::vec3(noise_distribution(gen), noise_distribution(gen), noise_distribution(gen)));
		scaleFactors.push_back(scale);
	}

	glGenBuffers(1, &instanced_array_buffer_matrices);
	glBindBuffer(GL_ARRAY_BUFFER, instanced_array_buffer_matrices);
	glBufferData(GL_ARRAY_BUFFER, this->num_asteroids * sizeof(glm::mat4), &modelMatrices[0], GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &instanced_array_buffer_noise);
	glBindBuffer(GL_ARRAY_BUFFER, instanced_array_buffer_noise);
	glBufferData(GL_ARRAY_BUFFER, noiseVectors.size() * sizeof(noiseVectors[0]), &noiseVectors[0].x, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &instanced_array_buffer_scale);
	glBindBuffer(GL_ARRAY_BUFFER, instanced_array_buffer_scale);
	glBufferData(GL_ARRAY_BUFFER, scaleFactors.size() * sizeof(scaleFactors[0]), scaleFactors.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void AsteroidField::draw(bool is_shadow_pass) {
	int stride = base_sphere.getInterleavedStride();
	glBindBuffer(GL_ARRAY_BUFFER, base_sphere.icosphereBufferObject);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * 6));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, instanced_array_buffer_noise);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (GLvoid*)0);

	glBindBuffer(GL_ARRAY_BUFFER, instanced_array_buffer_scale);
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, instanced_array_buffer_matrices);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
	glEnableVertexAttribArray(7);
	glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));
	glEnableVertexAttribArray(8);

	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);

	glVertexAttribDivisor(5, 1);
	glVertexAttribDivisor(6, 1);
	glVertexAttribDivisor(7, 1);
	glVertexAttribDivisor(8, 1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, base_sphere.elementBuffer);

	if (is_shadow_pass) {
		glPatchParameteri(GL_PATCH_VERTICES, 3);
		glDrawElementsInstanced(GL_PATCHES, base_sphere.getIndexCount(), GL_UNSIGNED_INT, 0, num_asteroids);
	}
	else {
		glDrawElementsInstanced(GL_TRIANGLES, base_sphere.getIndexCount(), GL_UNSIGNED_INT, 0, num_asteroids);
	}

	for (unsigned int i = 0; i < num_asteroids; i++) {
		glm::vec4 orig_pos = modelMatrices[i][3];
		float length = glm::length(orig_pos);

		float p = std::sqrt(std::pow(length, -3)) * 50;
		modelMatrices[i] = glm::rotate(glm::mat4(1), glm::radians(p), glm::vec3(0.0f, 1.0f, 0.0f)) * modelMatrices[i];
	}

	glBindBuffer(GL_ARRAY_BUFFER, instanced_array_buffer_matrices);
	glBufferSubData(GL_ARRAY_BUFFER, 0, num_asteroids * sizeof(glm::mat4), &modelMatrices[0]);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}