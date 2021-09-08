#ifndef FILE_SHADER_H
#define FILE_SHADER_H

#include "Shader.h"
#include <string>

class FileShader: public Shader
{

public:
	FileShader(const std::string vertexShaderPath, const std::string fragmentShaderPath);
	virtual ~FileShader(){}

private:
	static const char* readFile(const std::string path);
};

#endif
