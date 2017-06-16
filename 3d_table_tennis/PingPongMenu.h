#ifndef PP_MENU_H
#define PP_MENU_H

#include "Mesh.h"
#include "Base.h"
#include "GestureRecognizer.h"

class CCanvas;

class PingPongMenu{
public:
	PingPongMenu(CCanvas *canvas);
	void draw(unsigned int shaderId);
	void update(const double tau);

private:
	CCanvas *canvas;
	GestureRecognizer* gestureRecog;
	Mesh play, replay, paddle, ball, camera;
	Mesh hand;
	int stopCount;
	double playAngle, playStep;
	double replayAngle, replayStep;
	Point3d oldHandPosition;
	double handPosScaling;
};
#endif
