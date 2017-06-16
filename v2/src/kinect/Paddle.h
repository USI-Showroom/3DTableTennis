#ifndef PADDLE_H
#define PADDLE_H

#include "Base.h"
#include "Helpers.h"
#include "Configuration.h"

using namespace std;

class Paddle 
{
private:
	static const int list_size=5;
    //int normal_size;
	// coordinates of paddle are always saved as projective (pixel, pixel, millimeters)
	deque<int> x;
	deque<int> y;
	deque<int> depth;
	deque<Point3d> points, positions;// in meters
	Point3d speed, acceleration;//m/s
	deque<Point3d> normal, normalRaw;
	Configuration* conf;
	
	Paddle();
	~Paddle();

	void computeSpeed(); 

public:
	static Paddle* getInstance();
	Point3d getPosition();
	Point3d getNormal();
	Point3d getSpeed();

	void updatePosition(const Point3d newPos);
	void updateNormal(const Point3d newNormal);
};

#endif
