#include "Mesh.h"
#include "ObjReader.h"

Mesh::Mesh()
	: materials()
{ }

void Mesh::isSkybox(const bool sb)
{
	for(int i=0;i<materials.size();++i)
		materials[i].isSkybox(sb);
}

void Mesh::draw(unsigned int shaderId)
{
	for(int i=0;i<materials.size();++i)
		materials[i].draw(shaderId);
}

Mesh Mesh::meshFromOBJ(const std::string fileName)
{
	ObjReader reader;
	return reader.meshFrom(fileName);
}

/*void Mesh::setMaterials(std::vector<Material *> materials)
{
	materials.insert(materials.end(),materials);
}*/

void Mesh::addMaterial(Material m)
{
	materials.push_back(m);
}

/*

NSString *fileName = [[NSBundle mainBundle] pathForResource:aPath ofType:@"trm"];
if (fileName == nil)
return;
NSString *handle = [NSString stringWithContentsOfFile:fileName encoding: NSASCIIStringEncoding error: nil];
if (handle == nil)
return;
NSArray *lines = [handle componentsSeparatedByCharactersInSet:[NSCharacterSet newlineCharacterSet]];
for (NSString *rline in lines) {
NSString *matName = [rline stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]];
Art3DMaterial *mForName = [someMaterials valueForKey:matName];
if (mForName == nil)
continue;
[materials removeObject:mForName];
[materials addObject:mForName];
}
}*/