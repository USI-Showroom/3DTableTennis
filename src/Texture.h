#ifndef TEXTURE_H
#define TEXTURE_H

#include "Base.h"
#include "Defines.h"
#include <QtOpenGL>

class Texture{

private:
	GLuint name;
	GLenum texTarget;

	bool bounded;
	
	void build(const char* imgPath);

public:
	Texture();
	Texture(const char* path);

	virtual ~Texture(){
		clear();
	}

	inline const GLuint getName() const { return name;} 

	void bind() const;
	void disable() const;
	void enable() const;
	void clear();
};

#endif
