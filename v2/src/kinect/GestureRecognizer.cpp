#include "GestureRecognizer.h"
#include "Base.h"

using namespace std;

GestureRecognizer* GestureRecognizer::getInstance()
{
	static GestureRecognizer theInstance;
	return &theInstance;
}

GestureRecognizer::GestureRecognizer():
	player(PingPongPlayer::getInstance()),
	lastPos(0,0,0),
	lastDist(0.0),
	isSwipe(false),
	swipeScaling(3),
	isZoom(false),
	zoomScaling(3),
	crossCount(0),
	prevFootUp(false),
	readyCount(0)
{
}


double GestureRecognizer::recognizeSwipe()
{
	double swipeAmount = 0;

	Point3d hip = player->getHip();
	Point3d leftHand = player->getLeftHand();
	Point3d rightHand = player->getRightHand();

	bool rightHandIsRaised = isHandRaised(hip, rightHand);
	bool leftHandIsRaised = isHandRaised(hip, leftHand);

	if(rightHandIsRaised ^ leftHandIsRaised)
	{
		Point3d raisedHand = rightHandIsRaised ? rightHand : leftHand;
		if(!isSwipe)
			lastPos = raisedHand;

		swipeAmount = raisedHand.x() - lastPos.x();
		lastPos = raisedHand;
		isSwipe = true;
	}
	else
	{
		isSwipe = false;
	}

	return swipeAmount*swipeScaling;
}

double GestureRecognizer::recognizeZoom()
{
	double zoomAmount = 0;

	Point3d hip = player->getHip();
	Point3d leftHand = player->getLeftHand();
	Point3d rightHand = player->getRightHand();

	if(isHandRaised(hip, rightHand) && isHandRaised(hip, leftHand))
	{
		double handsDist = (leftHand - rightHand).norm();
		if(!isZoom)
			lastDist = handsDist;

		zoomAmount = lastDist - handsDist;
		lastDist = handsDist;
		isZoom = true;
	}
	else
	{
		isZoom = false;
	}

	return zoomAmount*zoomScaling;
}

bool GestureRecognizer::recognizeCross()
{
	Point3d hip = player->getHip();
	Point3d leftHand = player->getLeftHand();
	Point3d rightHand = player->getRightHand();

	if (!isHandRaised(hip, rightHand) && 
		!isHandRaised(hip, leftHand) && 
		rightHand.x() - leftHand.x() < -0.1 &&
		abs(rightHand.y() - leftHand.y()) < 0.1)
	{
		crossCount++;
		if(crossCount>20)
			return true;
	}
	else
		crossCount = 0;

	return false;
}

int GestureRecognizer::recognizeHandUp()
{
	Point3d head = player->getHead();
	Point3d leftHand = player->getLeftHand();
	Point3d rightHand = player->getRightHand();
	if(isHandUp(head, rightHand))
		return 1;
	else if(isHandUp(head, leftHand))
		return -1;
	else
		return 0;
}

bool GestureRecognizer::recognizeFootUp()
{
	Point3d leftFoot = player->getLeftFoot();
	Point3d rightFoot = player->getRightFoot();
	bool footUp = abs(leftFoot.y() - rightFoot.y()) > 0.2;
	bool footGotUp = !prevFootUp && footUp;
	prevFootUp = footUp;
	return footGotUp;
}

bool GestureRecognizer::recognizeReady()
{
	Point3d head = player->getHead();
	Point3d hip = player->getHip();
	if(head.z() - hip.z() < -0.05)
	{
		readyCount++;
		if(readyCount>50)
			return true;
	}
	else
		readyCount = 0;

	return false;
}

int GestureRecognizer::recognizeHandOnSide()
{
	Point3d hip = player->getHip();
	Point3d leftHand = player->getLeftHand();
	Point3d rightHand = player->getRightHand();
	if(isHandOnSide(hip, rightHand))
		return 1;
	else if(isHandOnSide(hip, leftHand))
		return -1;
	else
		return 0;
}

bool GestureRecognizer::isHandRaised(Point3d hip, Point3d hand)
{
	Point3d hipToHand = hip - hand;
	//cout << hipToHand.x() << " " << hipToHand.y() << " " << hipToHand.z() << endl;
	return hipToHand.y()<-0.4 && hipToHand.z()>0.2;
}

bool GestureRecognizer::isHandUp(Point3d head, Point3d hand)
{
	Point3d headToHand = head - hand;
	//cout << headToHand.x() << " " << headToHand.y() << " " << headToHand.z() << endl;
	return headToHand.y()<-0.2 && headToHand.z()<0.1;
}

bool GestureRecognizer::isHandOnSide(Point3d hip, Point3d hand)
{
	Point3d hipToHand = hip - hand;
	//cout << hipToHand.x() << " " << hipToHand.y() << " " << hipToHand.z() << endl;
	return abs(hipToHand.x())>0.6 && hipToHand.y()<-0.1;
}

void GestureRecognizer::resetReadyCounter()
{
	readyCount = 0;
}

