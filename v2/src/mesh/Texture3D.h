#ifndef TEXTURE_3D_H
#define TEXTURE_3D_H

#include "..\utils\Base.h"
#include "..\utils\Defines.h"
#include <QtOpenGL>
#include <string>

class Texture3D
{
private:
	GLuint name;
	GLenum target;
	bool loaded;
	std::string path;

public:
	Texture3D(const std::string path);
	virtual ~Texture3D(){};

	void enable();
	void disable();
	bool isEmpty();
	void finalizeTexture();
};

#endif
