#ifndef OBJREADER_C_H
#define OBJREADER_C_H

#include <map>
#include <vector>
#include <string>
#include "Base.h"
#include "Mesh.h"
#include "Material.h"

class ObjReader
{
private:
	typedef std::string string;
	typedef std::vector<Point3d> vec3;
	typedef std::vector<Point2d> vec2;
	typedef std::vector<string> vecS;
	typedef void (ObjReader::*func)(Mesh &mesh, vecS data); 

	Material *currentMaterial;
	string materialFile;
	vec3 vertices, normals;
	vec2 textureCoordinates;
	Point3d minPoint, maxPoint;
	std::map<string, Material *> materials;
	std::map<string, func> elements;

	void processVertex(Mesh &mesh, vecS data);
	void processNormal(Mesh &mesh, vecS data);
	void processTexture(Mesh &mesh, vecS data);
	void processMaterial(Mesh &mesh, vecS data);
	void processMaterialFile(Mesh &mesh, vecS data);
	void processFace(Mesh &mesh, vecS data);
public:

	ObjReader();
	Mesh meshFrom(string path);
	virtual ~ObjReader(){}
};

#endif
