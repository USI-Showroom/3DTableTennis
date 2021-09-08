#ifndef SHADER_MAIN_C_H
#define SHADER_MAIN_C_H

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "glext.h"

class Shader
{
private:
	unsigned int shader_id;
    unsigned int shader_vp;
    unsigned int shader_fp;

	bool initGL();
	
	void init(const char* vsText, const char * fsText);

	static void validateProgram(unsigned int program);
protected:
	

public:
	Shader(const char* vsText, const char * fsText);
	virtual ~Shader();

	void bind() const;
    void unbind() const;
    
    unsigned int id() const;

};

#endif
