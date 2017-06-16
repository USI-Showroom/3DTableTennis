#ifndef MATERIAL_C_H
#define MATERIAL_C_H

#include <GL\glew.h>
#include <vector>
#include <string>

#include "Base.h"
#include "Defines.h"

#include "Texture3D.h"

class Material
{
private:
	std::string name;
	Color4f ambient;
	Color4f diffuse;
	Color4f specular;
	float shininess;
	Texture3D *texture;
	int size;
	bool ambientToDiffuse;
	bool finalized;
	bool skybox;

	std::vector<Point3d> verticesArray;
	std::vector<Point2d> textureCoordinatesArray;
	std::vector<Point3d> normalsArray;

	GLuint verticesVBO;
	GLuint textureCoordinatesVBO;
	GLuint normalsVBO;

	GLuint vao;

	inline static GLfloat *color2Arr(const Color4f &col)
	{
		GLfloat *res =new GLfloat[4];
		res[0]= col.red();
		res[1]= col.green();
		res[2]= col.blue();
		res[3]= col.alpha();

		return res;	
	}

	inline static GLfloat *p32Arr(const Point3d &p)
	{
		GLfloat *res =new GLfloat[3];
		res[0]= fabs(p.x())<1e-6?0:(GLfloat)p.x();
		res[1]= fabs(p.y())<1e-6?0:(GLfloat)p.y();
		res[2]= fabs(p.z())<1e-6?0:(GLfloat)p.z(); 

		return res;
	}

	inline static GLfloat *p22Arr(const Point2d &p)
	{
		GLfloat *res =new GLfloat[2];
		res[0]= fabs(p.x())<1e-6?0:(GLfloat)p.x();
		res[1]= fabs(p.y())<1e-6?0:(GLfloat)p.y();

		return res;
	}

public:
	Material();
	Material(const std::string name);
	virtual ~Material();

	void setTexture(const std::string aTextureFile);
	void setTexture(Texture3D *txt);
	void clearTexture();
	void setAmbient(float r, float g, float b);
	void setDiffuse(float r, float g, float b);
	void setSpecular(float r, float g, float b);
	void setTransparency(float alpha);
	void setShininess(float val);
	void setAmbientToDiffuse(bool yesOrNo);
	const std::string &getName();
	void draw(unsigned int shaderId);
	void finalizeMaterial();
	const Texture3D *getTexture() const;
	void addVertex(Point3d vertex, Point3d normal, Point2d textureCoordinates);
	void setNormalAt(Point3d normal,  int index);
	void isSkybox(const bool sb);
};

#endif
