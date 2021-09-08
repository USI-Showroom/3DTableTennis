#ifndef BALL_LAUNCHER_H__
#define BALL_LAUNCHER_H__

#include "Base.h"

typedef struct trajectoy{
	Point3d position;
	Point3d velocity;
	Point3d acceleration;
	Point3d spinAxis;
    double spinAmplitude;
}trajectory;

class BallLauncher{
public:
	BallLauncher() {}
	virtual ~BallLauncher() {}
	virtual trajectoy nextTrajectory()=0;
};


class EasyLauncher: public BallLauncher{
private:
    int rangeX;
    double startY;
    double startZ;
public:
    EasyLauncher();
    trajectoy nextTrajectory();
};
#endif

