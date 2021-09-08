#include "CalibrationPoser.h"
#include "Defines.h"

#include <QtOpenGL>

CalibrationPoser::CalibrationPoser()
	:mesh(Mesh::meshFromOBJ(Path::meshFolder()+"\\pose.obj")),
	angle(0.0)
{
}

void CalibrationPoser::update(const double tau)
{ 
	angle += tau;
}


void CalibrationPoser::draw(unsigned int shaderId)
{
	glPushMatrix();
	
	glTranslatef(0.0, 1.2, -2.5);
	glScalef(0.5, 0.5, 0.5);
	glRotatef(angle, 0, 1, 0);
	
	mesh.draw(shaderId);

	glPopMatrix();
}