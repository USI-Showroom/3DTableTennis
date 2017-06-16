/************************************************************************/
/* guards                                                               */
/************************************************************************/
#ifndef CCANVAS_H
#define CCANVAS_H
#include <GL\glew.h>
#include "Configuration.h"
#include "TableTennisSystem.h"
#include "CalibrationPoser.h"
#include "FileShader.h"
#include <QTimer>

#include "Base.h"
#include "Defines.h"
#include "PingPongMenu.h"
#include "KinectManager.h"
#include "BallTrajectoryDrawer.h"
#include "GestureRecognizer.h"

#include <QGLViewer/qglviewer.h>

typedef enum gamestates {
	MENU, PLAYING, REPLAY
} gamestates;


class CustomCamera : public qglviewer::Camera
{
	virtual float zNear() const { return Configuration::getInstance()->get("NEAR_PLANE"); };
	virtual float zFar() const { return Configuration::getInstance()->get("FAR_PLANE"); };
};

class CCanvas : public QGLViewer
{
	Q_OBJECT

public:
	CCanvas(QWidget *parent = 0);

	void play();
	void replay();
	void goMenu() {currentState=MENU;}

protected:
	void init();
	void resizeGL(int width, int height);
	void draw();
	void keyPressEvent(QKeyEvent* event);

private slots:
	void updateKinect();

private:
	PingPongMenu *menu;
	TableTennisSystem *ttSystem;
	FileShader *lightShader,*textShader;
	CalibrationPoser* poser;
	Configuration* conf;
	gamestates currentState;
	KinectManager* kinectMngr;
	GestureRecognizer* gestureRecog;
	BallTrajectoryDrawer * trajectory;
	double angle, distance;

	void drawGame();
	void drawTrajectory();
	void drawMenu();
	void placeDefaultCamera();
};
#endif 