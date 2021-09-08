#include "Texture.h"

Texture::Texture(const char* imgPath):
	texTarget(GL_TEXTURE_2D), bounded(false)
{
	build(imgPath);	
}

Texture::Texture():
	texTarget(GL_TEXTURE_2D), bounded(false)
{ }


void Texture::build(const char* imgPath)
{	
	if (bounded)
		clear();


	QImage img(imgPath);

	img = QGLWidget::convertToGLFormat(img);

	glGenTextures(1,&name);
	glBindTexture(texTarget, name);

#ifndef USE_MIPMAPPING
	glTexImage2D(texTarget, 0, GL_RGBA, img.width(), img.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img.bits());
#else
	gluBuild2DMipmaps(texTarget, GL_RGBA, img.width(), img.height(), GL_RGBA, GL_UNSIGNED_BYTE, img.bits());
#endif

	//glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR_MIPMAP_LINEAR);


#ifndef USE_MIPMAPPING
	glTexParameteri(texTarget, GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(texTarget, GL_TEXTURE_MAG_FILTER,GL_LINEAR);
#else
	glTexParameteri(texTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(texTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
#endif

	//glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);

	glTexParameteri(texTarget, GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(texTarget, GL_TEXTURE_WRAP_T,GL_REPEAT);
	bounded = true;
}


void Texture::bind() const
{
	glBindTexture(texTarget, name);
}

void Texture::enable() const
{
	glEnable(texTarget);
}


void Texture::disable() const
{
	glDisable(texTarget);
	//glDeactivateTexture(GL_TEXTURE0);
}

void Texture::clear()
{
	glDeleteTextures(1, &name);
	bounded = false;
}