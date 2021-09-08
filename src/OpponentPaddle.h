#ifndef OPPONENT_PADDLE_H
#define OPPONENT_PADDLE_H

#include "Ball.h"
#include "Mesh.h"
#include "Base.h"

class OpponentPaddle
{
private:
	Ball* ball;
	Mesh mesh;

	Point3d normal;
	Point3d position;
	double posZ;
	Point3d velocity;
	double speed;
	bool inLaunchPosition;

public:
	OpponentPaddle(Ball* ball);
	~OpponentPaddle() {};

	bool isInitialized();
	Point3d getNormal();
	Point3d getPosition(bool cm);
	Point3d getVelocity();
	bool isInPosition();

	void draw(unsigned int shaderId);
	void update(const double tau);

};

#endif
