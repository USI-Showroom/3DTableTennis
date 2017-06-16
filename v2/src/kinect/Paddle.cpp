#include "Paddle.h"
#include "KinectManager.h"
#include "Defines.h"

/// <summary>
/// Constructor
/// </summary>
Paddle::Paddle()
{
	normal.push_back(Point3d (0.0, 0.0, -1.0));
	x.push_back(0);
	y.push_back(0);
	depth.push_back(0);

	conf = Configuration::getInstance();
}

/// <summary>
/// Destructor
/// </summary>
Paddle::~Paddle()
{
}

Paddle* Paddle::getInstance()
{
	static Paddle theInstance;
	return &theInstance;
}

Point3d Paddle::getPosition()
{
	static const double CENTER_HEIGHT = conf->get("CENTER_HEIGHT");
	static const double CENTER_DISTANCE = conf->get("CENTER_DISTANCE");
	
	if(positions.size() == 0)
		return Point3d(0,0,0);

	Point3d paddlePos = positions.front();
	double px = paddlePos.x();
	double py = paddlePos.y() + CENTER_HEIGHT;
	double pz = paddlePos.z() - CENTER_DISTANCE;
	return Point3d(px, py, pz);
}

Point3d Paddle::getNormal()
{
	return normal.front();
}

Point3d Paddle::getSpeed()
{
	return speed;
}

void Paddle::updatePosition(const Point3d newPos)
{
	points.push_front(newPos);

	positions.push_front(Helpers::smooth(points));

	if(points.size() >= list_size-1)
		points.pop_back();

	if(positions.size() >= list_size-1)
		positions.pop_back();

	computeSpeed();
}

void Paddle::updateNormal(const Point3d newNormal)
{
	normalRaw.push_front(newNormal);
	normal.push_front(Helpers::smooth(normalRaw).normalized());

	if(this->normal.size() >= list_size)
	{
		this->normal.pop_back();
		normalRaw.pop_back();
	}
}

void Paddle::computeSpeed()
{
	//if(positions.size() >= 2)
	//{
	//	Point3d newSpeed(positions[0]-positions[1]);
	//	double newTime = timer->getElapsedTime();
	//	newSpeed/=newTime;
	//	//cout << newTime << "	" << newSpeed.x() << "	" << newSpeed.y() << "	" << newSpeed.z() << endl;
	//	cout << newSpeed.norm() << "	" << positions[0] - positions[1] << endl;
	//	acceleration=(newSpeed-speed)/newTime;
	//	speed=newSpeed;
	//}
	//
	//timer->start();
}