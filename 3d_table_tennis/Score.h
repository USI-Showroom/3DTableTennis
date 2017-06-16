#ifndef SCORE_H__
#define SCORE_H__

#include "Shader.h"
#define MAXB 10

class CCanvas;

class Score{

public:
	Score(CCanvas *canvas, Shader *textShader, Shader *shader);
	void onLaunch();
	void onBallHit(int numHits);
	void onBallOnTable();
	void onWinMatch();
	void draw();
	void update();
	inline bool finish(){return launches>MAXB;}//false;} //launches>MAXB;}

private:
	Shader *textShader, *shader;
	int score, launches, timer;
	CCanvas *canvas;

};

#endif
