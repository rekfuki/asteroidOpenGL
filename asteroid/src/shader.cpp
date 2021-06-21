#include "shader.h"

#include <iostream>
#include <fstream>
#include <vector>

// Most of the logic is taken from wrapper_glfw.cpp

Shader::Shader(vector<tuple<GLenum, string, vector<string>>> sinfo) {
	shaders_info = sinfo;

	program_id = build_shader_program(sinfo);
}

GLuint Shader::build_shader_program(vector<tuple<GLenum, string, vector<string>>> shaders_info) {
	GLuint vertShader, fragShader, geoShader, tcsShader, tesShader;

	vector<tuple<GLenum, string, vector<string>>> shaders_sources;
	for (const auto& shader_info : shaders_info) {
		string shader_source_str = read_file(get<1>(shader_info));

		vector<string> utils_sources_str;
		for (const auto& utils_file : get<2>(shader_info)) {
			utils_sources_str.push_back(read_file(utils_file));
		}

		shaders_sources.push_back(tuple(get<0>(shader_info), shader_source_str, utils_sources_str));
	}

	GLint result = GL_FALSE;
	int logLength;
	GLuint program = glCreateProgram();

	vector<GLuint> compiled_shaders = compile_shaders(shaders_sources);

	for (const auto& shader : compiled_shaders) {
		glAttachShader(program, shader);
	}

	glLinkProgram(program);

	glGetProgramiv(program, GL_LINK_STATUS, &result);
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
	vector<char> programError((logLength > 1) ? logLength : 1);
	glGetProgramInfoLog(program, logLength, NULL, &programError[0]);
	if (logLength > 1) {
		cout << &programError[0] << endl;
	}

	for (const auto& shader : compiled_shaders) {
		glDeleteShader(shader);
	}

	return program;
}
void Shader::reload_shader() {
	GLuint new_program = build_shader_program(shaders_info);
	if (new_program) {
		program_id = new_program;
	}
}

/* Build shaders from strings containing shader source code */
vector<GLuint> Shader::compile_shaders(vector<tuple<GLenum, string, vector<string>>> shaders_sources) {
	vector<GLuint> compiled_shaders;

	string v = "#version 430\n";
	for (const auto& shader_source : shaders_sources) {
		GLenum shader_type = get<0>(shader_source);
		GLuint shader = glCreateShader(shader_type);

		string main_source = get<1>(shader_source);
		vector<string> headers = get<2>(shader_source);

		vector<GLchar const*> merged_sources{ v.c_str() };
		vector<GLint> lenghts{ (GLint)v.size() };
		for (const auto& util : headers) {
			merged_sources.push_back(util.c_str());
			lenghts.push_back((GLint)util.size());
		}
		merged_sources.push_back(main_source.c_str());
		lenghts.push_back((GLint)main_source.size());

		glShaderSource(shader, merged_sources.size(), &merged_sources[0], &lenghts[0]);

		glCompileShader(shader);

		GLint status;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
		if (status == GL_FALSE) {
			// Output the compile errors

			GLint infoLogLength;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

			GLchar* strInfoLog = new GLchar[infoLogLength + 1];
			glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);

			const char* strShaderType = NULL;
			switch (shader_type) {
			case GL_VERTEX_SHADER: strShaderType = "vertex"; break;
			case GL_GEOMETRY_SHADER: strShaderType = "geometry"; break;
			case GL_FRAGMENT_SHADER: strShaderType = "fragment"; break;
			case GL_TESS_CONTROL_SHADER: strShaderType = "tess control"; break;
			case GL_TESS_EVALUATION_SHADER: strShaderType = "tess eval"; break;
			}

			cerr << "Compile error in " << strShaderType << "\n\t" << strInfoLog << endl;
			delete[] strInfoLog;

			throw exception("Shader compile exception");
		}

		compiled_shaders.push_back(shader);
	}
	return compiled_shaders;
}

/* Read a text file into a string*/
string Shader::read_file(string filePath) {
	string content;
	ifstream fileStream(filePath, ios::in);

	if (!fileStream.is_open()) {
		cerr << "Could not read file " << filePath << ". File does not exist." << endl;
		return "";
	}

	string line = "";
	while (!fileStream.eof()) {
		getline(fileStream, line);
		content.append(line + "\n");
	}

	fileStream.close();
	return content;
}

void Shader::use() {
	glUseProgram(program_id);
}