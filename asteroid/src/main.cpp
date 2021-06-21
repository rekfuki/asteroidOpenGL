#ifdef _DEBUG
#pragma comment(lib, "glfw3D.lib")
#pragma comment(lib, "glloadD.lib")
#else
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "glload.lib")
#endif
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "soil.lib")

#include "wrapper_glfw.h"
#include <iostream>

#include <stack>
#include <cmath>
#include <map>
#include <vector>

#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include "shader.h"
#include "depth_map.h"
#include "sky_box.h"

#include "asteroid_field.h"
#include "asteroid.h"

GLuint VAO;

float screen_width = 1024.0;
float screen_height = 768.0;

// Callback globals
GLfloat aspect_ratio = screen_width / screen_height;

using namespace std;

unique_ptr<Shader> scene_shader, shadows_shader, skybox_shader, asteroids_shader;
unique_ptr<DepthMap> shadow_map;

struct Camera {
	float lastX = screen_width / 2;
	float lastY = screen_height / 2;
	float xOffset = 0;
	float yOffset = 0;
	float yaw = -90;
	float pitch = 0;
	const float speed = 5;
	glm::vec3 pos = glm::vec3(-50, 10, 0);
	glm::vec3 front = -pos;
} camera;

map<int, tuple<bool, float>> key_map;

struct Scene {
	unique_ptr<Asteroid> main_asteroid;
	unique_ptr<AsteroidField> asteroid_field;
	unique_ptr<SkyBox> sky_box;
} scene;

// Frame timing for smooth movement
float elapsed_time, last_frame;

float scale = 5.0;
float rim_steepness = 0.1;
float rim_width = 4.4;

float noise_strength = 1.6;

void init(GLWrapper* glw) {
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	try {
		scene_shader = make_unique<Shader>(vector<tuple<GLenum, string, vector<string>>> {
			tuple(GL_VERTEX_SHADER, "shaders\\scene.vert", vector<string>{ "shaders\\surface.glsl", "shaders\\constants.glsl" }),
				tuple(GL_TESS_CONTROL_SHADER, "shaders\\scene.tesc", vector<string>{ "shaders\\surface.glsl" }),
				tuple(GL_TESS_EVALUATION_SHADER, "shaders\\scene.tese", vector<string>{ "shaders\\surface.glsl", "shaders\\constants.glsl" }),
				tuple(GL_FRAGMENT_SHADER, "shaders\\scene.frag", vector<string>{ "shaders\\surface.glsl", "shaders\\lighting.glsl", "shaders\\constants.glsl"})
		});
		shadows_shader = make_unique<Shader>(vector<tuple<GLenum, string, vector<string>>> {
			tuple(GL_VERTEX_SHADER, "shaders\\scene.vert", vector<string>{ "shaders\\surface.glsl", "shaders\\constants.glsl" }),
				tuple(GL_TESS_CONTROL_SHADER, "shaders\\scene.tesc", vector<string>{ "shaders\\surface.glsl" }),
				tuple(GL_TESS_EVALUATION_SHADER, "shaders\\scene.tese", vector<string>{ "shaders\\surface.glsl", "shaders\\constants.glsl" }),
				tuple(GL_FRAGMENT_SHADER, "shaders\\shadows.frag", vector<string>{ "shaders\\surface.glsl" })
		});
		skybox_shader = make_unique<Shader>(vector<tuple<GLenum, string, vector<string>>> {
			tuple(GL_VERTEX_SHADER, "shaders\\skybox.vert", vector<string>()),
				tuple(GL_FRAGMENT_SHADER, "shaders\\skybox.frag", vector<string>()),
		});
		asteroids_shader = make_unique<Shader>(vector<tuple<GLenum, string, vector<string>>> {
			tuple(GL_VERTEX_SHADER, "shaders\\asteroid_field.vert", vector<string>{"shaders\\surface.glsl", "shaders\\constants.glsl" }),
				tuple(GL_FRAGMENT_SHADER, "shaders\\asteroid_field.frag", vector<string>{"shaders\\surface.glsl", "shaders\\lighting.glsl", "shaders\\constants.glsl" }),
		});
	}
	catch (exception& e)
	{
		cout << "Caught exception: " << e.what() << endl;
		cin.ignore();
		exit(0);
	}

	scene.main_asteroid = make_unique<Asteroid>(40, scale);
	scene.sky_box = make_unique<SkyBox>(vector<string>{
		"textures\\skybox\\right.png",
			"textures\\skybox\\left.png",
			"textures\\skybox\\top.png",
			"textures\\skybox\\bottom.png",
			"textures\\skybox\\front.png",
			"textures\\skybox\\back.png",
	});
	scene.asteroid_field = make_unique<AsteroidField>();

	shadow_map = make_unique<DepthMap>(screen_width, screen_height, 4024, 4024);
}

float l_near_plane = 819.0f;
float l_far_plane = 966.0f;
float side = 120.0f;

// Not using sticky keys from GLFW because the movement is too laggy
void processInputs() {
	for (auto& [key, val] : key_map) {
		if (get<0>(val)) {
			switch (key) {
			case GLFW_KEY_SPACE:
				camera.pos += elapsed_time * camera.speed * glm::vec3(0, 1, 0);
				break;
			case GLFW_KEY_LEFT_SHIFT:
				camera.pos -= elapsed_time * camera.speed * glm::vec3(0, 1, 0);
				break;
			case GLFW_KEY_W:
				camera.pos += elapsed_time * camera.speed * camera.front;
				break;
			case GLFW_KEY_S:
				camera.pos -= elapsed_time * camera.speed * camera.front;
				break;
			case GLFW_KEY_A:
				camera.pos -= glm::normalize(glm::cross(camera.front, glm::vec3(0.0f, 1.0f, 0.0f))) * elapsed_time * camera.speed;
				break;
			case GLFW_KEY_D:
				camera.pos += glm::normalize(glm::cross(camera.front, glm::vec3(0.0f, 1.0f, 0.0f))) * elapsed_time * camera.speed;
				break;
			case GLFW_KEY_C:
				rim_width = glm::min(rim_width * 1.01f, 10.f);
				cout << "rim_width: " << rim_width << endl;
				break;
			case GLFW_KEY_V:
				rim_width = glm::max(rim_width * 0.99f, 0.1f);
				cout << "rim_width: " << rim_width << endl;
				break;
			case GLFW_KEY_B:
				rim_steepness = glm::min(rim_steepness * 1.01f, 10.f);
				cout << "rim_steepness: " << rim_steepness << endl;
				break;
			case GLFW_KEY_N:
				rim_steepness = glm::max(rim_steepness * 0.99f, 0.1f);
				cout << "rim_steepness: " << rim_steepness << endl;
				break;
			case GLFW_KEY_COMMA:
				scale = glm::min(scale * 1.010f, 10.0f);
				cout << "scale: " << scale << endl;
				scene.main_asteroid->re_scale_craters(scale);
				break;
			case GLFW_KEY_PERIOD:
				scale = glm::max(scale * 0.99f, 0.1f);
				cout << "scale: " << scale << endl;
				scene.main_asteroid->re_scale_craters(scale);
				break;
			case GLFW_KEY_Q:
				get<1>(val) -= elapsed_time;
				if (get<1>(val) > 0) {
					continue;
				}
				scene.main_asteroid->gen_craters(scale);
				get<1>(val) = elapsed_time * 30;
				break;
			case GLFW_KEY_MINUS:
				noise_strength *= 0.99f;
				cout << "noise_strength: " << noise_strength << endl;
				break;
			case GLFW_KEY_EQUAL:
				noise_strength *= 1.01f;
				cout << "noise_strength: " << noise_strength << endl;
				break;
			}
		}
	}
}

void display() {
	// So the movement does not depend on the framerate
	float currentFrame = glfwGetTime();
	elapsed_time = currentFrame - last_frame;
	last_frame = currentFrame;

	processInputs();

	int n = 0;
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);

	glm::mat4 light_projection = glm::ortho(
		-side, side,
		-side, side,
		l_near_plane, l_far_plane
	);

	glm::mat4 light_view = glm::lookAt(
		glm::vec3(-900.0f, 1.0f, 1.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)
	);

	// Shadow pass
	// ------------------------------------------------------------------------------
	shadows_shader->use();

	shadows_shader->set_uniform("is_shadow_pass", 1);
	shadows_shader->set_uniform("rim_width", rim_width);
	shadows_shader->set_uniform("rim_steepness", rim_steepness);

	// Currently has no effect on the shadow pass but set for clarity reasons
	shadows_shader->set_uniform("light_projection", light_projection);
	// Currently has no effect on the shadow pass but set for clarity reasons
	shadows_shader->set_uniform("light_view", light_view);

	// Set to reuse the code
	shadows_shader->set_uniform("projection", light_projection);
	// Set to reuse the code
	shadows_shader->set_uniform("view", light_view);

	shadows_shader->set_uniform("view_position", camera.pos);

	shadows_shader->set_uniform("noise_strength", noise_strength);

	shadow_map->bind_buffer();
	glDisable(GL_CULL_FACE);

	shadows_shader->set_uniform("is_asteroid_belt", 0);
	scene.main_asteroid->draw();

	shadows_shader->set_uniform("is_asteroid_belt", 1);
	scene.asteroid_field->draw(true);

	glEnable(GL_CULL_FACE);
	shadow_map->unbind_buffer();

	// Main asteroid + Shadows pass
	// -----------------------------------------------------------------------------
	scene_shader->use();
	scene_shader->set_uniform("is_shadow_pass", 0);
	scene_shader->set_uniform("light_projection", light_projection);
	scene_shader->set_uniform("light_view", light_view);
	scene_shader->set_uniform("noise_strength", noise_strength);

	glm::mat4 view = glm::lookAt(
		camera.pos,
		camera.pos + camera.front,
		glm::vec3(0, 1, 0)
	);
	scene_shader->set_uniform("view", view);
	scene_shader->set_uniform("view_position", camera.pos);

	float near_plane = 0.1f;
	float far_plane = 1000.0f;
	glm::mat4 projection = glm::perspective(glm::radians(90.f), aspect_ratio, near_plane, far_plane);
	scene_shader->set_uniform("projection", projection);

	glm::mat4 normal_matrix = glm::transpose(glm::inverse(view));
	scene_shader->set_uniform("normal_matrix", normal_matrix);

	scene_shader->set_uniform("rim_width", rim_width);
	scene_shader->set_uniform("rim_steepness", rim_steepness);
	scene_shader->set_uniform("is_asteroid_belt", 0);

	shadow_map->activate_texture();
	scene.main_asteroid->draw();

	// Asteroid belt pass
	// -----------------------------------------------------------------------------
	asteroids_shader->use();
	asteroids_shader->set_uniform("light_projection", light_projection);
	asteroids_shader->set_uniform("light_view", light_view);
	asteroids_shader->set_uniform("view", view);
	asteroids_shader->set_uniform("view_position", camera.pos);

	asteroids_shader->set_uniform("projection", projection);

	asteroids_shader->set_uniform("noise_strength", noise_strength);

	scene.asteroid_field->draw();

	// Skybox pass
	// -----------------------------------------------------------------------------
	glDepthFunc(GL_LEQUAL);
	skybox_shader->use();
	skybox_shader->set_uniform("view", glm::mat4(glm::mat3(view)));
	skybox_shader->set_uniform("projection", projection);

	glm::mat4 model = glm::mat4(1);
	model = glm::scale(model, glm::vec3(10, 10, 10));
	skybox_shader->set_uniform("model", model);
	scene.sky_box->draw();
	glDepthFunc(GL_LESS);

	glDisableVertexAttribArray(0);
	glUseProgram(0);
}

static void reshape(GLFWwindow* window, int w, int h) {
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	aspect_ratio = float(w) / float(h);
	shadow_map->update_viewport_size(w, h);
}

static void keyCallback(GLFWwindow* window, int key, int s, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (action == GLFW_PRESS) {
		key_map[key] = tuple<bool, float>(true, 0);
	}
	else if (action == GLFW_RELEASE) {
		key_map[key] = tuple<bool, float>(false, 0);
	}
}

void mouseCursorCallback(GLFWwindow* window, double xpos, double ypos) {
	camera.yaw += ((xpos - camera.lastX) * 0.1f);
	camera.pitch += ((camera.lastY - ypos) * 0.1f);

	if (camera.pitch > 89.0f) camera.pitch = 89.0f;
	if (camera.pitch < -89.0f) camera.pitch = -89.0f;

	glm::vec3 direction;
	direction.x = cos(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
	direction.y = sin(glm::radians(camera.pitch));
	direction.z = sin(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
	camera.front = glm::normalize(direction);
	camera.lastX = xpos;
	camera.lastY = ypos;
}

void printControls() {
	cout << endl << "Application controls:" << endl;
	cout << "Mouse to look around" << endl;
	cout << "W,A,S,D to move the camera" << endl;
	cout << "SHIFT to descend" << endl;
	cout << "SPACE to ascend\n" << endl;
	cout << "MINUS to reduce noise strength for terrain" << endl;
	cout << "EQUALS to increase noise strength for terrains\n" << endl;
	cout << "Q to regenerate craters" << endl;
	cout << "COMMA to increase the scale of the craters" << endl;
	cout << "PERIOD to decrease the scale of the craters" << endl;
	cout << "C to increase craters rim width" << endl;
	cout << "V to decrease craters rim width" << endl;
	cout << "B to increase craters rim steepness" << endl;
	cout << "N to decrease craters rim steepness" << endl;
}

int main(int argc, char* argv[]) {
	GLWrapper* glw = new GLWrapper(screen_width, screen_height, "Asteroid field");;

	if (!ogl_LoadFunctions())
	{
		fprintf(stderr, "ogl_LoadFunctions() failed. Exiting\n");
		return 0;
	}

	glw->setRenderer(display);
	glw->setKeyCallback(keyCallback);
	glw->setMouseCallback(mouseCursorCallback);
	glw->setReshapeCallback(reshape);

	glw->DisplayVersion();

	printControls();

	init(glw);

	glw->eventLoop();

	delete(glw);
	return 0;
}