#ifndef LIGHT_SHADER_H
#define LIGHT_SHADER_H

#include "Shader.h"

class LightShader: public Shader
{
protected:
	inline static const char *vertexShaderCode();
	inline static const char *fragmentShaderCode();

public:
	LightShader();
	virtual ~LightShader(){}
};

#endif
