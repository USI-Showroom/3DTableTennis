#ifndef MESH_C_H
#define MESH_C_H

#include <vector>
#include <string>
#include "Material.h"

class Mesh
{
protected:
	std::vector<Material> materials;

public:
	Mesh();
	virtual ~Mesh(){}

	static Mesh meshFromOBJ(const std::string fileName);
	//void setMaterials(std::vector<Material *> materials);
	void addMaterial(Material m);
	void isSkybox(const bool aBool);

	void draw(unsigned int shaderId);

	std::vector<Material> getMaterials() const { return materials; }
};

#endif
