#include "ObjReader.h"
#include "MtlReader.h"

#include "Defines.h"
#include <fstream>
#include <assert.h>

static float texturesCoo[]={0,0, 0,1, 1,0};

void ObjReader::processVertex(Mesh &mesh, vecS data)
{
	_ASSERT(data.size() ==4);
	if (data.size() < 4)
		return;
	
	float x = atof(data[1].c_str());
	float y = atof(data[2].c_str());
	float z = atof(data[3].c_str());

	Point3d vertex(x,y,z);
	/*for (int i = 0; i < 3; ++i) {
		float value = [vertex get:i];
		if (value < [minPoint get:i])
			[minPoint set:i to:value];
		if (value > [maxPoint get:i])
			[maxPoint set:i to:value];
	}*/
	
	vertices.push_back(vertex);
}

void ObjReader::processNormal(Mesh &mesh, vecS data)
{
	_ASSERT(data.size() ==4);

	if (data.size() < 4)
		return;
	
	float x = atof(data[1].c_str());
	float y = atof(data[2].c_str());
	float z = atof(data[3].c_str());

	Point3d normal(x,y,z);
	normals.push_back(normal);
}

void ObjReader::processTexture(Mesh &mesh, vecS data)
{
	_ASSERT(data.size() ==3);

	if (data.size() < 3)
		return;
	
	float x = atof(data[1].c_str());
	float y = atof(data[2].c_str());
	Point2d texture(x,y);
	textureCoordinates.push_back(texture);
}

void ObjReader::processFace(Mesh &mesh, vecS data)
{
	_ASSERT(data.size()==4);
	for (int i = 1; i < 4; ++i) {
		vecS components = MtlReader::split(data[i],'/');
		int vertexIndex = atoi(components[0].c_str()) - 1;
		int textureIndex = atoi(components[1].c_str()) - 1;
		int normalIndex = atoi(components[2].c_str()) - 1;
		_ASSERT(vertexIndex >=0);
		_ASSERT(textureIndex >=0);
		_ASSERT(normalIndex >=0);

		Point3d vertex = vertices[vertexIndex];
        Point2d texture =textureCoordinates[textureIndex];
			//= (textureIndex == -1) ? 
            //[[Vector2 alloc] initX: (texturesCoo[(i-1)*2]) y:(texturesCoo[(i-1)*2+1])]: 
        
        Point3d normal = normals[normalIndex];;
        
        //if(normalIndex == -1){
         //   missingNormals[i-1]=YES;
         //   normal=[[Vector3 alloc] initX:0 y:0 z:0];
        //}
        currentMaterial->addVertex(vertex,normal,texture);
    }
}

void ObjReader::processMaterial(Mesh &mesh, vecS data)
{
	_ASSERT(data.size() ==2);

    string name = data[1];
    currentMaterial = materials[name];
    if (currentMaterial == NULL) {
        currentMaterial = new Material(name);
        materials[name]=currentMaterial;
    }
}

void ObjReader::processMaterialFile(Mesh &mesh, vecS data)
{
	materialFile = Path::meshFolder()+"\\"+data[1];
}

ObjReader::ObjReader()
	:vertices(),textureCoordinates(), normals(),
	materials(),elements() 
{
	elements["v"]=&ObjReader::processVertex;
	elements["vn"]=&ObjReader::processNormal;
	elements["vt"]=&ObjReader::processTexture;

	elements["f"]=&ObjReader::processFace;
	elements["usemtl"]=&ObjReader::processMaterial;
	elements["mtllib"]=&ObjReader::processMaterialFile;
}

Mesh ObjReader::meshFrom(string path)
{
	Mesh mesh;

	
	//minPoint = [[Vector3 alloc] initX:CGFLOAT_MAX y:CGFLOAT_MAX z:CGFLOAT_MAX];
	//maxPoint = [[Vector3 alloc] initX:CGFLOAT_MIN y:CGFLOAT_MIN z:CGFLOAT_MIN];	

	std::ifstream file (path);

	if(!file.is_open()){
		std::cout<<"Unable to open OBJ file: "<< path << std::endl;
		return mesh;
	}

	string line;

	while ( file.good() ){
		std::getline (file,line);
        //NSString *line = [rline stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]];
		if (line.length() == 0 || line[0] == '#')
			continue;

		vecS components = MtlReader::split(line, ' ');
		string key = components[0];
		func value = elements[key];
		if (value == NULL)
			continue;

		(this->*value)(mesh,components);
	}
//	vertices.clear();
//	textureCoordinates.clear();
//	normals.clear();

	MtlReader mtlReader;
	mtlReader.materialsFrom(materialFile,materials);

	std::map<string,Material *>::iterator it;
	for(it=materials.begin();it!=materials.end(); ++it)
		mesh.addMaterial(*(it->second));

	//[mesh setBoundingBox: minPoint and: maxPoint];
    
//	materials.clear();
    
	return mesh;
}