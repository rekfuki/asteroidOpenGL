#pragma once

#include "wrapper_glfw.h"
#include <glm/glm.hpp>

#include <map>
#include <iostream>
#include <vector>

using namespace std;

class Shader {
public:
	//Shader(string vert, string frag) : Shader(vert, frag, "", "", "", "") {};
	//Shader(string vert, string frag, string geo) : Shader(vert, frag, geo, "", "", "") {};
	//Shader(string vert, string frag, string tcs, string tes) : Shader(vert, frag, "", tcs, tes, "") {};
	//Shader(string vert, string frag, string geo, string tcs, string tes, string utils);
	Shader(vector<tuple<GLenum, string, vector<string>>> shaders_source);
	~Shader() {};

	void use();
	void reload_shader();
	template <typename T> void set_uniform(string name, T t) {
		auto iter = uniforms.find(name);
		if (iter == uniforms.end()) {
			uniforms[name] = glGetUniformLocation(program_id, name.c_str());
		}

		if constexpr (is_same<T, glm::mat4>::value) {
			glUniformMatrix4fv(uniforms[name], 1, GL_FALSE, &t[0][0]);
		}
		else if constexpr (is_same<T, glm::mat3>::value) {
			glUniformMatrix3fv(uniforms[name], 1, GL_FALSE, &t[0][0]);
		}
		else if constexpr (is_same<T, glm::vec3>::value) {
			glUniform3fv(uniforms[name], 1, &t[0]);
		}
		else if constexpr (is_same<T, float>::value) {
			glUniform1fv(uniforms[name], 1, &t);
		}
		else if constexpr (is_same<T, int>::value) {
			glUniform1iv(uniforms[name], 1, &t);
		}
	}
private:

	//GLuint build_shader_program(string vert, string frag, string geo, string tcs, string tes, string utils);
	GLuint build_shader_program(vector<tuple<GLenum, string, vector<string>>> shaders_source);
	vector<GLuint> compile_shaders(vector<tuple<GLenum, string, vector<string>>> shaders_source);
	string read_file(string filePath);

	GLuint program_id;
	map<string, GLuint> uniforms;

	//string vsPath;				// Vertex shader path
	//string fsPath;				// Fragment shader path
	//string geoPath;				// Geometry shader path
	//string tcsPath;				// Tessellation control shader path
	//string tesPath;				// Tessellation eval shader path
	//string utilsPath;           // Utils shader path
	vector<tuple<GLenum, string, vector<string>>> shaders_info;
};