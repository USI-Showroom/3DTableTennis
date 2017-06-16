#include "MtlReader.h"

#include <fstream>
#include "Defines.h"

void MtlReader::processColorAmbient(Material *material, vector data)
{
	assert(data.size() ==4);

	if (material == NULL)
		return;
	material->setAmbient(atof(data[1].c_str()),atof(data[2].c_str()),atof(data[3].c_str()));
}

void MtlReader::processColorDiffuse(Material *material, vector data)
{
	assert(data.size() ==4);

	if (material == NULL)
		return;
	material->setDiffuse(atof(data[1].c_str()),atof(data[2].c_str()),atof(data[3].c_str()));

}

void MtlReader::processColorSpecular(Material *material, vector data)
{
	assert(data.size() ==4);

	if (material == NULL)
		return;
	material->setSpecular(atof(data[1].c_str()),atof(data[2].c_str()),atof(data[3].c_str()));	
}

void MtlReader::processTextureDiffuse(Material *material, vector data)
{
	assert(data.size() ==2);

	if (material == NULL)
		return;
	material->setTexture(Path::textureFolder() + "\\" + data[1]);
}

void MtlReader::processTransparency(Material *material, vector data)
{
	assert(data.size() ==2);

	if (material == NULL)
		return;
	material->setTransparency(atof(data[1].c_str()));
}

void MtlReader::processShininess(Material *material, vector data)
{
	assert(data.size() ==2);

	if (material == NULL)
		return;
	material->setShininess(atof(data[1].c_str()));
}

MtlReader::MtlReader()
	:elements()
{
	elements["Ka"]=&MtlReader::processColorAmbient;
	elements["Kd"]=&MtlReader::processColorDiffuse;
	elements["Ks"]=&MtlReader::processColorSpecular;

	elements["d"]=&MtlReader::processTransparency;
	elements["Tr"]=&MtlReader::processTransparency;
	elements["Ns"]=&MtlReader::processShininess;

	elements["map_Kd"]=&MtlReader::processTextureDiffuse;
}

void MtlReader::materialsFrom(std::string path,  std::map<std::string, Material *> &source)
{
	Material *currentMaterial = NULL;

	std::ifstream file (path);

	if(!file.is_open()){
		std::cout<<"Unable to open MTL file: "<< path << std::endl;
		return;
	}

	string line;

	while ( file.good() ){
		std::getline (file,line);

		if (line.length() == 0 || line[0] == '#')
			continue;

		std::vector<string> components(split(line, ' ' ));
		string key = components[0];

		if (key=="newmtl") {
			string materialName = components[1];
			currentMaterial = source[materialName];
			continue;
		}

		func value = elements[key];
		if (value == NULL)
			continue;

		(this->*value)(currentMaterial,components);
	} 
}

std::map<std::string, Material> MtlReader::materialsFrom(string path)
{
	std::map<std::string, Material> materials;

	Material *currentMaterial = NULL;

	std::ifstream file (path);

	if(!file.is_open()){
		std::cout<<"Unable to open MTL file: "<< path << std::endl;
		return materials;
	}

	string line;

	while ( file.good() ){
		std::getline (file,line);
		if (line.length() == 0 || line[0] == '#')
			continue;

		vector components(split(line,' '));
		string key = components[0];

		if (key=="newmtl") {
			string materialName = components[1];
			if (currentMaterial != NULL)
				materials[currentMaterial->getName()] = *currentMaterial;

			currentMaterial = &materials[materialName];
			if (currentMaterial == NULL)
				currentMaterial = new Material(materialName);
			continue;
		}

		func value = elements[key];
		if (value == NULL)
			continue;

		(this->*value)(currentMaterial,components);
	}

	if (currentMaterial != NULL)
		materials[currentMaterial->getName()] = *currentMaterial;

	return materials;
}
