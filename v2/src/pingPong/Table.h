#ifndef TABLE_H
#define TABLE_H

#include "..\mesh\Mesh.h"
#include "..\utils\Configuration.h"

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
