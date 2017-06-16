#ifndef BALL_H
#define BALL_H

#include "Mesh.h"
#include "Base.h"
#include "SoundEngine.h"
#include "Configuration.h"
#include "GestureRecognizer.h"
#include "BallLauncher.h"
#include "Score.h"

class OpponentPaddle;

class Ball
{
public:
	Ball(BallLauncher *launcher);
	virtual ~Ball(){};
	void setOppPaddle(OpponentPaddle* oPaddle);
	void resetPlayerReady();
	Point3d getVelocity();
	void draw(unsigned int shaderId);
	void update(const double tau);
	inline void setScore(Score *score) {this->score=score;}
	inline const Point3d &getPosition() {return position;}
	Point3d getStartPosition();
	bool isReadyToLaunch();
	bool isNetHit();

private:
	Score *score;
	Configuration* conf;
	GestureRecognizer* gestureRecog;
	Point3d position, prevPos, velocity, acceleration, startPos;
	BallLauncher *launcher;
	OpponentPaddle* oppPaddle;
	Point3d spinAxis;
	double spinAmplitude, lastDistance;
	SoundEngine *sounds;
	int bouncings, consecutiveHits;
	bool passed, goingForward, hitNet, myTurn;
	bool wasInContactPaddle;
	bool drawHelperLine;
	bool playerIsReady, readyToLaunch;
	Mesh ball, shadow, line;
	void computeAcceleration();
    void paddleBallInterception();
	void intersectPaddle();
    void oppPaddleBallInterception();
	void intersectOppPaddle();
	void computeBouncing(const Point3d &normalV,const Point3d &tangentialV, const double contactTime, const double tangentialFriction,const double cor);
	void reset();
};

#endif
