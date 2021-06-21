#include "depth_map.h"

DepthMap::DepthMap(float vpW, float vpH, unsigned int w, unsigned int h) {
	viewport_width = vpW;
	viewport_height = vpH;
	shadow_map_width = w;
	shadow_map_height = h;

	glGenTextures(1, &texture_object);
	glBindTexture(GL_TEXTURE_2D, texture_object);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		shadow_map_width, shadow_map_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glGenFramebuffers(1, &buffer_object);
	glBindFramebuffer(GL_FRAMEBUFFER, buffer_object);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		GL_TEXTURE_2D, texture_object, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		throw std::exception("Failed to create depth map");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

DepthMap::~DepthMap() {}
void DepthMap::update_viewport_size(int w, int h) {
	viewport_width = w;
	viewport_height = h;
}

// This seems like an overkill
void DepthMap::bind_buffer() {
	glViewport(0, 0, shadow_map_width, shadow_map_height);
	glBindFramebuffer(GL_FRAMEBUFFER, buffer_object);
	glClear(GL_DEPTH_BUFFER_BIT); // Not sure I need this ?
}

void DepthMap::unbind_buffer() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, viewport_width, viewport_height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void DepthMap::activate_texture() {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_object);
}