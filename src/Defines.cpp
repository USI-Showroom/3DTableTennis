#include "Defines.h"


Path *Path::instance=NULL;
Fonts *Fonts::instance=NULL;

const string Path::projectRoot()
{
	if(Path::instance==NULL)
		Path::instance=new Path();

	return Path::instance->projRoot;
}

const string Path::meshFolder()
{
	if(Path::instance==NULL)
		Path::instance=new Path();

	return Path::instance->mesh;
}

const string Path::textureFolder()
{
	if(Path::instance==NULL)
		Path::instance=new Path();

	return Path::instance->textures;
}


Fonts::Fonts()
{
	string path(Path::projectRoot());
	path+="\\fonts\\Quartz Regular.ttf";

	int result = QFontDatabase::addApplicationFont(path.c_str());

	if(result==-1)
		log=QFont("TypeWriter",25,QFont::Bold);
	else
		log=QFont("Quartz",25);

	log.setStyleStrategy(QFont::OpenGLCompatible);
}

const QFont Fonts::logFont()
{
	if(Fonts::instance==NULL)
		Fonts::instance=new Fonts();

	return Fonts::instance->log;
}