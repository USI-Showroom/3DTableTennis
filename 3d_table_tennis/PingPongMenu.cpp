#include "PingPongMenu.h"
#include "Player.h"
#include "CCanvas.h"
#include "ReplayManager.h"

static const int	STAY_TIME = 200;
static const double MAX_STEP = 2;
static const double MIN_STEP = 0.2;

PingPongMenu::PingPongMenu(CCanvas *canvas):
	play(Mesh::meshFromOBJ(Path::meshFolder()+"\\play.obj")),
	replay(Mesh::meshFromOBJ(Path::meshFolder()+"\\replay.obj")),
	hand(Mesh::meshFromOBJ(Path::meshFolder()+"\\hand.obj")),
	paddle(Mesh::meshFromOBJ(Path::meshFolder()+"\\paddle.obj")),
	ball(Mesh::meshFromOBJ(Path::meshFolder()+"\\ball.obj")),
	camera(Mesh::meshFromOBJ(Path::meshFolder()+"\\cam3.obj")),
	oldHandPosition(0,0,0),
	stopCount(0),
	playAngle(0), 
	playStep(0.2),
	replayAngle(0), 
	replayStep(0.2),
	canvas(canvas),
	handPosScaling(3)
{
	gestureRecog = GestureRecognizer::getInstance();
}

void PingPongMenu::draw(unsigned int shaderId)
{
	double xTranslate, zTranslate;
	/*glPushMatrix();
	glTranslatef(-1.0, 2.5, -4.5);
	play.draw(shaderId);
	glTranslatef(0.0, -0.6, 0);
	replay.draw(shaderId);
	glPopMatrix();*/

	// Draw paddle and ball
	xTranslate = 1.7*(playStep/MAX_STEP)-(1.7*(MIN_STEP/MAX_STEP));
	zTranslate = 4.5*(playStep/MAX_STEP)-(4.5*(MIN_STEP/MAX_STEP));
	glPushMatrix();
	glTranslatef(xTranslate,0,zTranslate);
	glTranslatef(-1.7, 1.7, -4.5);
	glRotated(playAngle,0,1,0);
	glTranslatef(0, 0, 0.1);
	glRotated(90,1,0,0);
	glRotated(-45,0,1,0);
	glScaled(5,5,5);
	paddle.draw(shaderId);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(xTranslate,0,zTranslate);
	glTranslatef(-1.7, 1.7, -4.5);
	glRotated(playAngle,0,1,0);
	glTranslatef(0, 0, -0.5);
	glScaled(5,5,5);
	ball.draw(shaderId);
	glPopMatrix();

	// Draw videocamera
	glDisable(GL_CULL_FACE);
	xTranslate = -(1.7*(replayStep/MAX_STEP))+(1.7*(MIN_STEP/MAX_STEP));
	zTranslate = 4.5*(replayStep/MAX_STEP)-(4.5*(MIN_STEP/MAX_STEP));
	glPushMatrix();
	glTranslatef(xTranslate,0,zTranslate);
	glTranslatef(1.7, 1.2, -4.5);
	glRotated(-replayAngle,0,1,0);
	glScaled(2,2,2);
	camera.draw(shaderId);
	glPopMatrix();
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	/*glPushMatrix();
	Point3d handP=PingPongPlayer::getInstance()->getPlayingHand()*handPosScaling;
	glTranslatef(handP.x(),handP.y(),-4);
	double angle=-(stopStep*1.0)/(stay_time*1.0)*45.0;
	glRotated(angle,1,0,0);
	if(PingPongPlayer::getInstance()->getLefty())
		glRotated(180,0,1,0);
	glScaled(0.5,0.5,0.5);
	hand.draw(shaderId);
	glPopMatrix();*/
}

void PingPongMenu::update(const double tau)
{
	playAngle+=playStep;
	replayAngle+=replayStep;

	//Point3d rightHand = PingPongPlayer::getInstance()->getRightHand()*handPosScaling;
	//Point3d leftHand = PingPongPlayer::getInstance()->getLeftHand()*handPosScaling;
	
	/*double norm = (handP-oldHandPosition).squaredNorm();
	if(norm<1e-3)
		++stopStep;
	else
		stopStep=0;

	oldHandPosition = handP;

	if(stopStep>stay_time)
	{
		stopStep=0;
		if(handP.x()>-0.8 && handP.x()<0.4 && handP.y()>1.9 && handP.y()<2.3)
		{
			ofstream file;
			file.open(Path::ballPositionFile().c_str());
			file << "" << endl;
			file.close();
			canvas->play();
		}
		else if(handP.x()>-0.8 && handP.x()<0.4 && handP.y()>1.3 && handP.y()<1.7)
			canvas->replay();
	}*/

	int handOnSide = gestureRecog->recognizeHandOnSide();
	if(handOnSide == 1)
	{
		if(replayStep >= MAX_STEP)
		{
			replayStep = MAX_STEP;
			stopCount++;
			if(stopCount > STAY_TIME && !ReplayManager::Instance().getPoints().empty())
			{
				canvas->replay();
			}
		}
		else
		{
			replayStep *= 1.01;
			stopCount = 0;
		}
		playStep = playStep < MIN_STEP ? MIN_STEP : playStep/1.01;
	}
	else if(handOnSide == -1)
	{
		if(playStep >= MAX_STEP)
		{
			playStep = MAX_STEP;
			stopCount++;
			if(stopCount > STAY_TIME)
			{
				stopCount = 0;
				
				ReplayManager::Instance().clear();
				canvas->play();
			}
		}
		else
		{
			playStep *= 1.01;
			stopCount = 0;
		}
		replayStep = replayStep < MIN_STEP ? MIN_STEP : replayStep/1.01;
	}
	else
	{
		stopCount = 0;
		replayStep = replayStep < MIN_STEP ? MIN_STEP : replayStep/1.01;
		playStep = playStep < MIN_STEP ? MIN_STEP : playStep/1.01;
	}
}