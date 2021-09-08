#ifndef GESTURE_RECOG_H
#define GESTURE_RECOG_H

#include "Player.h"

class GestureRecognizer
{
private:
	PingPongPlayer* player;
	Point3d lastPos;
	double lastDist;
	bool isSwipe;
	double swipeScaling;
	bool isZoom;
	double zoomScaling;
	int crossCount;
	bool prevFootUp;
	int readyCount;

	GestureRecognizer();
	~GestureRecognizer() {};
	bool isHandRaised(Point3d hip, Point3d hand);
	bool isHandUp(Point3d head, Point3d hand);
	bool isHandOnSide(Point3d hip, Point3d hand);
	
public:
	static GestureRecognizer* getInstance();

	double recognizeSwipe();
	double recognizeZoom();
	bool recognizeCross();
	int recognizeHandUp();
	bool recognizeFootUp();
	bool recognizeReady();
	void resetReadyCounter();
	int recognizeHandOnSide();

};

#endif