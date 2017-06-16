#ifndef _SCOREBOARD_H
#define _SCOREBOARD_H

#include "Mesh.h"
#include "Material.h"
#include "Texture3D.h"
#include <vector>

class ScoreBoard
{
private:
	Mesh mesh;
	Material *d1, *d2, *u1, *u2;
	std::vector<Texture3D *> textures;

	double time;

	int score1, score2;
	int currentD1, currentD2, currentU1, currentU2;

public:
	ScoreBoard();
	virtual ~ScoreBoard(){}

	void update(const double tau);
	void draw(unsigned int shaderId);

	void setScore1(int v);
	void setScore2(int v);
	
	void increaseScore1();
	void increaseScore2();
};

#endif
