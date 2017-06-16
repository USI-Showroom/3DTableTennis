#include "Table.h"
#include "Defines.h"

#include <QtOpenGL>

Table::Table()
	:table(Mesh::meshFromOBJ(Path::meshFolder()+"\\table.obj")),
	net(Mesh::meshFromOBJ(Path::meshFolder()+"\\net.obj"))
{
	conf = conf->getInstance();
}

void Table::update(const double tau)
{ }


void Table::draw(unsigned int shaderId)
{
	static const double TABLE_WIDTH = conf->get("TABLE_WIDTH");
	static const double TABLE_HEIGHT = conf->get("TABLE_HEIGHT");
	static const double TABLE_LENGTH = conf->get("TABLE_LENGTH");

	glPushMatrix();
	
	glTranslatef(-TABLE_WIDTH/2.0f,0,0);
	
	table.draw(shaderId);

	glPushMatrix();
	glTranslatef(0,TABLE_HEIGHT,-TABLE_LENGTH/2);
	net.draw(shaderId);
	glPopMatrix();

	glPopMatrix();
}