#ifndef MTLREADER_H
#define MTLREADER_H

#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include "Material.h"

class MtlReader
{
private:
	typedef std::string string;
	typedef std::vector<string> vector;
	typedef void (MtlReader::*func)(Material *, vector ); 


	std::map<string, func> elements;

	void processColorAmbient(Material *material, vector data);
	void processColorDiffuse(Material *material, vector data);
	void processColorSpecular(Material *material, vector data);
	void processTextureDiffuse(Material *material, vector data);
	void processTransparency(Material *material, vector data);
	void processShininess(Material *material, vector data);

public:
	static inline vector split(const string &s, char delim)
	{
		vector elems;

		std::stringstream ss(s);
		string item;

		while(std::getline(ss, item, delim))
			elems.push_back(item);

		return elems;
	}

public:
	MtlReader();
	~MtlReader(){}

	std::map<string, Material> materialsFrom(string path);
	void materialsFrom(string path,  std::map<string, Material *> &source);
};

#endif
