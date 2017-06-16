#ifndef TABLETENNIS_SYSTEM_H
#define TABLETENNIS_SYSTEM_H

#include "Table.h"
#include "ScoreBoard.h"
#include "Ball.h"
#include "OpponentPaddle.h"
#include "Configuration.h"
#include "Score.h"

class CCanvas;
class Shader;

class TableTennisSystem
{
private:
	Ball ball;
	Table table;
	OpponentPaddle* oppPaddle;
	ScoreBoard scoreBoard;
	Score score;

public:
	TableTennisSystem(CCanvas *canvas, Shader *text, Shader*light);
	virtual ~TableTennisSystem(){}

	void update(const double tau);
	void draw(unsigned int shaderId);
};
#endif
