#include "Shader.h"
#include "..\utils\Base.h"

#include <iostream>

void Shader::validateProgram(GLuint program) {
	const unsigned int BUFFER_SIZE = 512;
	char buffer[BUFFER_SIZE];
	memset(buffer, 0, BUFFER_SIZE);
	GLsizei length = 0;
    
	memset(buffer, 0, BUFFER_SIZE);
	glGetProgramInfoLog(program, BUFFER_SIZE, &length, buffer);
	if (length > 0)
		std::cerr << "Program " << program << " link error: " << buffer << std::endl;
    
	glValidateProgram(program);
	GLint status;
	glGetProgramiv(program, GL_VALIDATE_STATUS, &status);
	if (status == GL_FALSE)
		std::cerr << "Error validating shader " << program << std::endl;
}

Shader::Shader(const char* vsText, const char * fsText)
{
	init(vsText,fsText);
}

Shader::~Shader()
{
	glDetachShader(shader_id, shader_fp);
	glDetachShader(shader_id, shader_vp);

	glDeleteShader(shader_fp);
	glDeleteShader(shader_vp);
	glDeleteProgram(shader_id);
}

void Shader::init(const char* vsText, const char * fsText)
{
	//glEnable(GL_FRAGMENT_PROGRAM_ARB);
	//glEnable(GL_VERTEX_PROGRAM_ARB);

	shader_vp = glCreateShader(GL_VERTEX_SHADER);
	shader_fp = glCreateShader(GL_FRAGMENT_SHADER);

	if (vsText == NULL || fsText == NULL) {
		std::cerr << "No code found" << std::endl;
		return;
	}

	glShaderSource(shader_vp, 1, &vsText, 0);
	glShaderSource(shader_fp, 1, &fsText, 0);

	glCompileShader(shader_vp);
	glCompileShader(shader_fp);

	shader_id = glCreateProgram();
	glAttachShader(shader_id, shader_fp);
	glAttachShader(shader_id, shader_vp);
	glLinkProgram(shader_id);
	
	validateProgram(shader_id);

	//glDisable(GL_FRAGMENT_PROGRAM_ARB);
	//glDisable(GL_VERTEX_PROGRAM_ARB);
}

unsigned int Shader::id() const {
	return shader_id;
}

void Shader::bind() const{
	glUseProgram(shader_id);
}

void Shader::unbind() const {
	glUseProgram(0);
}