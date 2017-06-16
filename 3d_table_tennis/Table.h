#ifndef TABLE_H
#define TABLE_H

#include "Mesh.h"
#include "Configuration.h"

class Table
{
private:

	Configuration* conf;
	Mesh table, net;

public:
	Table();
	virtual ~Table(){}

	void update(const double tau);
	void draw(unsigned int shaderId);
};

#endif
