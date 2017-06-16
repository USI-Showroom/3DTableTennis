#include "Texture3D.h"


Texture3D::Texture3D(const std::string path)
	:loaded(false),
	target(GL_TEXTURE_2D),
	path(path)
{}


void Texture3D::finalizeTexture()
{
	QImage img(path.c_str());

	img = QGLWidget::convertToGLFormat(img);

	glGenTextures(1,&name);
	glBindTexture(target, name);

#ifndef USE_MIPMAPPING
	glTexImage2D(target, 0, GL_RGBA, img.width(), img.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img.bits());
#else
	gluBuild2DMipmaps(target, GL_RGBA, img.width(), img.height(), GL_RGBA, GL_UNSIGNED_BYTE, img.bits());
#endif

	//glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR_MIPMAP_LINEAR);


#ifndef USE_MIPMAPPING
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER,GL_LINEAR);
#else
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
#endif

	//glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);

	glTexParameteri(target, GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(target, GL_TEXTURE_WRAP_T,GL_REPEAT);
	loaded = true;
}

void Texture3D::enable()
{
	glEnable(target);
	glBindTexture(target, name);
}

void Texture3D::disable()
{
	glDisable(target);
}