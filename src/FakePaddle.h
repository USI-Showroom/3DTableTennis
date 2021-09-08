#ifndef FAKE_PADDLE_H
#define FAKE_PADDLE_H

#include "Base.h"
#include <deque>

class FakePaddle
{
	int list_size, normal_size;
	std::deque<int> x;
	std::deque<int> y;
	std::deque<int> depth;
	double x_speed, y_speed, x_speed_old, y_speed_old; // pixel per frame
	double d_speed, d_speed_old; // mm per frame
	std::deque<Point3d> normal;

	Point3d p1, p2, p3;

	FakePaddle(){
		normal.push_back(Point3d (0.0, 0.0, 0.0));
		list_size = 5;
		normal_size = 3;
		d_speed_old = y_speed_old = x_speed_old = d_speed = y_speed = x_speed = 0;
		x.push_back(0);
		y.push_back(0);
		depth.push_back(2000);
	
		Point3d p1 (320, 240, depth.front());
		Point3d p2 (310, 240, depth.front()+100);
		Point3d p3 (330, 240, depth.front()+100);
	}

	~FakePaddle(){
	}

	void computeSpeed();


public:

	static FakePaddle* getInstance();
	int* getPosition();
	double* getSpeed();
	double* getAcceleration();
	Point3d getNormal();

};

#endif
