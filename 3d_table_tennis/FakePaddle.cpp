#include "FakePaddle.h"

FakePaddle* FakePaddle::getInstance()
{
	static FakePaddle theInstance;
	return &theInstance;
}

int* FakePaddle::getPosition()
{	
	int* position = new int[3];

	position[0] = x.front();
	position[1] = y.front();
	position[2] = depth.front();

	return position;
}

double* FakePaddle::getSpeed(){
	double result[3] = {x_speed, y_speed, d_speed};
	return result;
}


double* FakePaddle::getAcceleration(){
	double result[3] = {(x_speed - x_speed_old), (y_speed - y_speed_old), (d_speed - d_speed_old)};
	return result;
}

void FakePaddle::computeSpeed(){
	//int total_x = sum(x);
	x_speed_old = x_speed;
	x_speed = (x.front() - x.back()) /(double)x.size();

	//int total_y = sum(y);
	y_speed_old = y_speed;
	y_speed = (y.front() - y.back()) /(double)y.size();

	//int total_d = sum(depth);
	d_speed_old = d_speed;
	d_speed = (depth.front() - depth.back()) /(double)depth.size();

}

Point3d FakePaddle::getNormal(){

	Point3d n = (p2 - p1) ^ (p3 - p1);
	n.normalize();
	return n;
}