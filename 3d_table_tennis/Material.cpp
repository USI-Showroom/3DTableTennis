#include "Material.h"


Material::~Material()
{    
/*	delete texture;
	glDeleteBuffers(1, &verticesVBO);
	glDeleteBuffers(1, &textureCoordinatesVBO);
	glDeleteBuffers(1, &normalsVBO);*/
}

Material::Material()
	:name(),
	ambient(0.2,0.2,0.2,1.0),
	diffuse(0.8,0.8,0.8,1.0),
	specular(0,0,0,1),
	shininess(0),
	verticesArray(),
	textureCoordinatesArray(),
	normalsArray(),
	ambientToDiffuse(true),
	skybox(false),
	finalized(false)
{
	texture=NULL;
}

Material::Material(const std::string name)
	:name(name),
	ambient(0.2,0.2,0.2,1.0),
	diffuse(0.8,0.8,0.8,1.0),
	specular(0,0,0,1),
	shininess(0),
	verticesArray(),
	textureCoordinatesArray(),
	normalsArray(),
	ambientToDiffuse(true),
	skybox(false),
	finalized(false)
{
	texture=NULL;
}

void Material::isSkybox(const bool sb)
{
	skybox=sb;
}

void Material::setAmbientToDiffuse(const bool val)
{
	ambientToDiffuse = val;
}

const std::string &Material::getName()
{
	return name;
}

void Material::setAmbient(float r, float g, float b)
{
	ambient[0] = r;
	ambient[1] = g;
	ambient[2] = b;
}

void Material::setDiffuse(float r, float g, float b)
{
	diffuse[0] = r;
	diffuse[1] = g;
	diffuse[2] = b;
}

void Material::setSpecular(float r, float g, float b)
{
	specular[0] = r;
	specular[1] = g;
	specular[2] = b;
}

void Material::setTransparency(float someTransparency)
{
	ambient[3] = someTransparency;
	diffuse[3] = someTransparency;
	specular[3] = someTransparency;
}

void Material::setShininess(float someShininess)
{
	shininess = someShininess;
}

void Material::setTexture(const std::string aTextureFile)
{
	clearTexture();
	texture=new Texture3D(aTextureFile);
}

void Material::clearTexture()
{
	if(texture!=NULL)
		delete texture;

	texture=NULL;
}

void Material::setTexture(Texture3D *txt)
{
	texture=txt;
}

void Material::draw(unsigned int shaderId)
{
	if (!finalized)
		finalizeMaterial();

	bool light=true;
	bool blend=true;
	bool depth=true;

	if(skybox){
		depth=glIsEnabled(GL_DEPTH_TEST);
		light=glIsEnabled(GL_LIGHTING);
		blend=glIsEnabled(GL_BLEND);

		glDisable(GL_DEPTH_TEST);
		glDisable(GL_LIGHTING);
		glDisable(GL_BLEND);

		glCullFace(GL_FRONT);
	}
	else{
		
		if (ambientToDiffuse)
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color2Arr(diffuse));
		else {
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, color2Arr(ambient));
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, color2Arr(diffuse));
		}

		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, color2Arr(specular));
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
	}

	int isTexture_location = glGetUniformLocationARB(shaderId, "isTexture");
	glUniform1iARB(isTexture_location, texture != NULL);

	if (texture != NULL)
		texture->enable();
	
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, size);

	if (texture != NULL)
		texture->disable(); 

	if(skybox){
		if(depth)
			glEnable( GL_DEPTH_TEST );
		if(light)
			glEnable( GL_LIGHTING );
		if(blend)
			glEnable( GL_BLEND );
		glCullFace(GL_BACK);
	}
	glBindVertexArray(0);
}

const Texture3D *Material::getTexture() const
{
	return texture;
}

void Material::addVertex(Point3d vertex, Point3d normal, Point2d textureCoordinates)
{
	verticesArray.push_back(vertex);
	normalsArray.push_back(normal);
	textureCoordinatesArray.push_back(textureCoordinates);
}

void Material::setNormalAt(Point3d normal,  int index)
{
	Point3d &n=normalsArray[normalsArray.size()-1-index];

	n[0]=normal[0];
	n[1]=normal[1];
	n[2]=normal[2];
}

void Material::finalizeMaterial()
{
	assert(verticesArray.count % 3 == 0);
	assert(verticesArray.count == textureCoordinatesArray.count);
	assert(normalsArray.count == verticesArray.count);
	size = verticesArray.size();

	if (texture != NULL)
		texture->finalizeTexture();


	int unitSize = sizeof(GLfloat);
	GLfloat *verticesData = (GLfloat *) malloc(unitSize*8*size);

	for (int i = 0; i < size; ++i) {
		Point3d &vertex = verticesArray[i];
		Point2d &textureCoords = textureCoordinatesArray[i];
		Point3d &normal = normalsArray[i];

		GLfloat x=p32Arr(vertex)[0];
		GLfloat y=p32Arr(vertex)[1];
		GLfloat z=p32Arr(vertex)[2];

		memcpy(verticesData+3*i, p32Arr(vertex), 3*unitSize);
		memcpy(verticesData+2*i+3*size, p22Arr(textureCoords), 2*unitSize);
		memcpy(verticesData+3*i+5*size, p32Arr(normal), 3*unitSize);
	}
	
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffersARB(1, &verticesVBO);
	glBindBufferARB(GL_ARRAY_BUFFER, verticesVBO);
	glBufferDataARB(GL_ARRAY_BUFFER, unitSize*size*8, verticesData, GL_STATIC_DRAW);

	glVertexPointer(3, GL_FLOAT, 0, (void *)0);
	glTexCoordPointer(2, GL_FLOAT, 0, (void *)(3*size*unitSize));
	glNormalPointer(GL_FLOAT, 0, (void *) (5*size*unitSize));

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);

	glBindVertexArray(0);
	glBindBufferARB(GL_ARRAY_BUFFER, 0);

	free(verticesData);
	finalized = true;
}
