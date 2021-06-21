#pragma once

#include "wrapper_glfw.h"

class DepthMap {
public:
	DepthMap(float vpW, float vpH, unsigned int width, unsigned int height);
	~DepthMap();

	void bind_buffer();
	void unbind_buffer();
	void activate_texture();
	void update_viewport_size(int w, int h);

private:
	int shadow_map_width, shadow_map_height;
	float viewport_width, viewport_height;
	GLuint texture_object;
	GLuint buffer_object;
};
