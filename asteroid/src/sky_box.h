#pragma once
#include "wrapper_glfw.h"

#include <vector>
class SkyBox {
public:
	SkyBox(std::vector<std::string> faces);
	~SkyBox() {};
	void draw();
private:
	GLuint texture_id;
	GLuint position_buffer_object;
	GLuint attribute_v_coord;
};
