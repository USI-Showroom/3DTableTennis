#include <algorithm>

#include "Ball.h"
#include "Defines.h"
#include "VirtualPaddle.h"
#include "OpponentPaddle.h"
#include "Paddle.h"
#include "GestureRecognizer.h"
#include "ReplayManager.h"

#include <QtOpenGL>

Ball::Ball(BallLauncher *launcher):
	launcher(launcher),
	sounds(SoundEngine::getInstance()),
	bouncings(0),
	consecutiveHits(0),
	passed(false),
	hitNet(false),
	myTurn(false),
	wasInContactPaddle(false),
	ball(Mesh::meshFromOBJ(Path::meshFolder()+"\\ball.obj")),
    shadow(Mesh::meshFromOBJ(Path::meshFolder()+"\\shadow.obj")),
    line(Mesh::meshFromOBJ(Path::meshFolder()+"\\line.obj")),
	lastDistance(-1),
	score(NULL),
	prevPos(0,0,-10.0),
	drawHelperLine(true),
	playerIsReady(false)
{
	trajectoy t = launcher->nextTrajectory();
	startPos = t.position;
	position = t.position;
	velocity = t.velocity;
	acceleration = t.acceleration;
	spinAxis = t.spinAxis;
	spinAmplitude = t.spinAmplitude;
	
	goingForward = velocity.z() < 0;

	conf = Configuration::getInstance();
	gestureRecog = GestureRecognizer::getInstance();

	readyToLaunch = true;
}

void Ball::setOppPaddle(OpponentPaddle* oPaddle)
{
	oppPaddle = oPaddle;
}

void Ball::resetPlayerReady()
{
	playerIsReady = false;
}

Point3d Ball::getVelocity()
{
	return velocity;
}

Point3d Ball::getStartPosition()
{
	return startPos;
}

bool Ball::isReadyToLaunch()
{
	return readyToLaunch;
}

bool Ball::isNetHit()
{
	return hitNet;
}

void Ball::draw(unsigned int shaderId)
{
	if(!oppPaddle->isInPosition())
		return;

#ifndef USE_VIRTUAL_PADDLE
	Paddle* paddle = Paddle::getInstance();
#else
	VirtualPaddle* paddle = VirtualPaddle::getPaddle();
#endif
    
	if(drawHelperLine)
	{
		Point3d toConvert = paddle->getPosition()-position;
		double norm = toConvert.norm();
		toConvert /= norm;
    
		double angle = acos(toConvert.z())/M_PI*180.;
		Point3d axis = toConvert^Point3d(0,0,-1);

		glPushMatrix();
		glTranslated(position.x(),position.y(),position.z());
		glRotatef(angle,axis.x(),axis.y(),axis.z());
		glScaled(1,1,norm/0.02);
		line.draw(shaderId);
		glPopMatrix();
	}
    
    static const double TH = conf->get("TABLE_HEIGHT")+0.01;
	static const double TABLE_LENGTH = conf->get("TABLE_LENGTH");
	static const double TABLE_WIDTH = conf->get("TABLE_WIDTH");
    static const double FH = 0.01;
    glPushMatrix();
	bool overTable = position[0] >= -TABLE_WIDTH/2.0 && position[0] <= TABLE_WIDTH/2.0 && position[2] <=0 && position[2] >=-TABLE_LENGTH;
	if(overTable)
		glTranslated (position[0], TH, position[2]);
	else
		glTranslated (position[0], FH, position[2]);
	shadow.draw(shaderId);
	glPopMatrix();
	    
	glPushMatrix();
	glTranslated (position[0], position[1], position[2]);
	ball.draw(shaderId);
	glPopMatrix();
}

void Ball::update(const double tau)
{
	if(!oppPaddle->isInPosition())
	{
		return;
	}
	else if(!playerIsReady)
	{
		// Recognize when player is ready
		playerIsReady = gestureRecog->recognizeReady();

		// Gesture recognition to toggle ball-paddle helper line
		if(gestureRecog->recognizeFootUp())
			drawHelperLine = !drawHelperLine;

		// Gesture recognition to change lefty or righty player
		int upHand = gestureRecog->recognizeHandUp();
		if(upHand == 1)
			PingPongPlayer::getInstance()->setLefty(false);
		else if(upHand == -1)
			PingPongPlayer::getInstance()->setLefty(true);

		return;
	}
	
	readyToLaunch = false;
	
	static const double BALL_RADIUS = conf->get("BALL_RADIUS");
	static const double TABLE_LENGTH = conf->get("TABLE_LENGTH");
	static const double TABLE_WIDTH = conf->get("TABLE_WIDTH");
	static const double TABLE_HEIGHT = conf->get("TABLE_HEIGHT");
	static const double NET_HEIGHT = conf->get("NET_HEIGHT");
	static const double TABLE_CONTACT_TIME = conf->get("TABLE_CONTACT_TIME");
	static const double TABLE_TANGENTIAL_FRICTION = conf->get("TABLE_TANGENTIAL_FRICTION");
	static const double BALL_NET_COR = conf->get("BALL_NET_COR");
	static const double BALL_TABLE_COR = conf->get("BALL_TABLE_COR");
	static const double NET_Z_POS = -TABLE_LENGTH/2.0;

	velocity += 0.5*tau*acceleration;
	position += tau*velocity;
	velocity += 0.5*tau*acceleration;

	goingForward = velocity[2] < 0;
	ReplayManager::Instance().add(position);

	intersectPaddle();
	intersectOppPaddle();
	computeAcceleration();

	bool overTable = position[0] >= -TABLE_WIDTH/2.0 && position[0] <= TABLE_WIDTH/2.0 && position[2] <=0 && position[2] >=-TABLE_LENGTH;
	
	// Check net collision
	if (overTable && 
		position[2] > -TABLE_LENGTH+1 &&
		((prevPos[2] < NET_Z_POS && position[2] >= NET_Z_POS) || 
		 (prevPos[2] > NET_Z_POS && position[2] <= NET_Z_POS)))
	{
		double slope = (prevPos[1] - position[1]) / abs(prevPos[2] - position[2]);
		double distNet = abs(NET_Z_POS - prevPos[2]);
		double ballYNet = prevPos[1]-BALL_RADIUS + distNet*slope;
		if(ballYNet <= TABLE_HEIGHT+NET_HEIGHT)
		{
			const Point3d nv(0,0,velocity[2]);
			const Point3d tv(velocity[0],velocity[1],0);
			computeBouncing(nv,tv,TABLE_CONTACT_TIME,TABLE_TANGENTIAL_FRICTION,BALL_NET_COR);
				
			position[2] = NET_Z_POS + BALL_RADIUS*(goingForward?-1:1);

			sounds->play(net);
			bouncings = 0;
			hitNet = true;
		}
	}
	prevPos = position;

	bool onTable = overTable && position[1]-BALL_RADIUS<=TABLE_HEIGHT && velocity[1] < 0;
	bool onGround = !overTable && position[1]-BALL_RADIUS<=0;
	if(onTable || onGround) //hitting the table or ground
	{
		const Point3d nv(0,velocity[1],0);
		const Point3d tv(velocity[0],0,velocity[2]);
		computeBouncing(nv,tv,TABLE_CONTACT_TIME,TABLE_TANGENTIAL_FRICTION,BALL_TABLE_COR);

		//velocity[1]=-velocity[1]*BALL_TABLE_COR;
		position[1] = onTable?TABLE_HEIGHT+BALL_RADIUS : BALL_RADIUS;

		sounds->play(bouncing);
		if(score && onTable && !hitNet)
		{
			bouncings++;
			if(velocity.z() < 0)
				score->onBallOnTable();	
		}
	}

	if(onGround || abs(position.z())>7.74 || fabs(position.x())>3.0 || velocity[0]*velocity[0]+velocity[2]*velocity[2] < 0.01)
	{
		if(bouncings == 0 && !myTurn)
			score->onWinMatch();
        reset();
	}
}

void Ball::reset()
{
	trajectoy t = launcher->nextTrajectory();
	startPos = t.position;
	position = t.position;
	velocity = t.velocity;
	acceleration = t.acceleration;
	spinAxis = t.spinAxis;
	spinAmplitude = t.spinAmplitude;

    lastDistance = -1;
    bouncings = 0;
	consecutiveHits = 0;
    
    passed = false;
	hitNet = false;
	myTurn = false;
    goingForward = velocity.z() < 0;
    wasInContactPaddle = false;
    
    spinAxis = Point3d(1,0,0);
    spinAmplitude = 0;
	if(score)
		score->onLaunch();

	readyToLaunch = true;
}

void Ball::computeAcceleration()
{
	static const double BALL_RADIUS = conf->get("BALL_RADIUS");
	static const double AIR_DENSITY = conf->get("AIR_DENSITY");
	static const double BALL_MASS = conf->get("BALL_MASS");
	static const double DRAG_COEFF = conf->get("DRAG_COEFF");
	static const double GRAVITY_ACCELERATION = conf->get("GRAVITY_ACCELERATION");

	// Ad/2
	static const double coef = M_PI * BALL_RADIUS * BALL_RADIUS * 0.5 * AIR_DENSITY/BALL_MASS;

	for(int i = 0 ; i < 3 ; ++i)
	{
		double v = velocity[i];
		acceleration[i] = v*v*coef;
		double angularV = spinAxis[(i-1)%3]*spinAmplitude*BALL_RADIUS;

		double cd = DRAG_COEFF; //drag coefficient
		double cl = 0; //lift coefficient

		if(angularV != 0)
		{
			v /= angularV;
			cl = 1/(2+v);
			cd += 1.0/pow(22.5+4.2*pow(fabs(v),2.5),0.4);
			cd = min(cd,1.0);
		}

		acceleration[i] *= (cd * (v<0?1:-1)+cl);
	}

	acceleration[1] += GRAVITY_ACCELERATION;

	//acceleration+=externalAcceleration;
}

void Ball::computeBouncing(const Point3d &normalV,const Point3d &tangentialV, const double contactTime, const double tangentialFriction,const double cor)
{
	static const double BALL_RADIUS = conf->get("BALL_RADIUS");

	velocity = tangentialV*tangentialFriction - normalV * cor;
	const Point3d spinNewV = normalV.normalized()^spinAxis*spinAmplitude*BALL_RADIUS*contactTime;
	velocity += spinNewV;
}

void Ball::intersectPaddle()
{
	static const double BALL_RADIUS = conf->get("BALL_RADIUS");
	static const double PADDLE_RADIUS = conf->get("PADDLE_RADIUS");

#ifndef USE_VIRTUAL_PADDLE
	Paddle* paddle = Paddle::getInstance();
#else
	VirtualPaddle* paddle = VirtualPaddle::getPaddle();
#endif

	const Point3d paddlePos = paddle->getPosition();
	const Point3d paddleNormal = paddle->getNormal();
	Point3d relPos(position-paddlePos);
	double distance = paddleNormal*relPos;

//	if(distance>BALL_RADIUS || distance < 0){
//		if(!(distance<0 && lastDistance>0)){
//			lastDistance=distance;
//			return;
//		}
//	}
		
	lastDistance = distance;

	relPos -= (distance*paddleNormal);

    if((lastDistance>BALL_RADIUS && distance < -BALL_RADIUS) || distance<=BALL_RADIUS){
		if(relPos.norm()+BALL_RADIUS<=PADDLE_RADIUS)
		{
			position = paddlePos+relPos+paddleNormal*BALL_RADIUS;
			paddleBallInterception();
			bouncings = 0;
			myTurn = true;
		}
	}
}

void Ball::paddleBallInterception()
{
#ifndef USE_VIRTUAL_PADDLE
	Paddle* paddle = Paddle::getInstance();
#else
	VirtualPaddle* paddle = VirtualPaddle::getPaddle();
#endif
    
    static const double BALL_RADIUS = conf->get("BALL_RADIUS");
	static const double PADDLE_RADIUS = conf->get("PADDLE_RADIUS");
	static const double PADDLE_CONTACT_TIME = conf->get("PADDLE_CONTACT_TIME");
	static const double PADDLE_TANGENTIAL_FRICTION = conf->get("PADDLE_TANGENTIAL_FRICTION");
	static const double BALL_PADDLE_COR = conf->get("BALL_PADDLE_COR");
    
    const Point3d paddlePos = paddle->getPosition();
    const Point3d paddleNormal = paddle->getNormal();
	Point3d relPos(position-paddlePos);
	double distance = paddleNormal*relPos;
    
	position = paddlePos+relPos+paddleNormal*BALL_RADIUS;

	consecutiveHits++;
	if(score)
		score->onBallHit(consecutiveHits);

    sounds->play(paddleSound);
    
	Point3d speed(paddle->getSpeed());
    
	const Point3d paddleNormalSpeed = (paddleNormal*speed)*paddleNormal;
	Point3d paddleTangentialSpeed = speed-paddleNormalSpeed;

	double newSpinAmplitude = paddleTangentialSpeed.norm();
	if(newSpinAmplitude != 0)
		paddleTangentialSpeed /= newSpinAmplitude;
	newSpinAmplitude *= PADDLE_CONTACT_TIME/BALL_RADIUS;

	velocity -= paddleNormalSpeed;

	const Point3d normal = (velocity*paddleNormal)*paddleNormal;
	const Point3d tangential = velocity - normal;

	computeBouncing(normal,tangential,PADDLE_CONTACT_TIME,PADDLE_TANGENTIAL_FRICTION,BALL_PADDLE_COR);

	//compute spin
	const Point3d newSpinAxis = paddleNormal^paddleTangentialSpeed;
	spinAxis = newSpinAxis*newSpinAmplitude+spinAxis*spinAmplitude;
	spinAmplitude = spinAxis.norm();
	if(spinAmplitude != 0)
		spinAxis /= spinAmplitude;

	spinAmplitude /= 100.;
}

void Ball::intersectOppPaddle()
{
	static const double BALL_RADIUS = conf->get("BALL_RADIUS");
	static const double PADDLE_RADIUS = conf->get("PADDLE_RADIUS");

	const Point3d paddlePos = oppPaddle->getPosition(true);
	const Point3d paddleNormal = oppPaddle->getNormal();
	Point3d relPos(position-paddlePos);
	double distance = paddleNormal*relPos;
		
	lastDistance = distance;

	relPos -= (distance*paddleNormal);

    if((lastDistance > BALL_RADIUS && distance < -BALL_RADIUS) || distance<=BALL_RADIUS)
	{
		if(relPos.norm()+BALL_RADIUS<=PADDLE_RADIUS)
		{
			position = paddlePos+relPos+paddleNormal*BALL_RADIUS;
			oppPaddleBallInterception();
			bouncings = 0;
			myTurn = false;
		}
	}
}

void Ball::oppPaddleBallInterception()
{
	static const double BALL_RADIUS = conf->get("BALL_RADIUS");
	static const double PADDLE_RADIUS = conf->get("PADDLE_RADIUS");
	static const double PADDLE_CONTACT_TIME = conf->get("PADDLE_CONTACT_TIME");
	static const double PADDLE_TANGENTIAL_FRICTION = conf->get("PADDLE_TANGENTIAL_FRICTION");
	static const double BALL_OPPONENT_PADDLE_COR = conf->get("BALL_OPPONENT_PADDLE_COR");
    
    const Point3d paddlePos = oppPaddle->getPosition(true);
    const Point3d paddleNormal = oppPaddle->getNormal();
	Point3d relPos(position-paddlePos);
	double distance = paddleNormal*relPos;
    
	position = paddlePos+relPos+paddleNormal*BALL_RADIUS;

    sounds->play(paddleSound);
    
	Point3d speed(oppPaddle->getVelocity());
	const Point3d paddleNormalSpeed = (paddleNormal*speed)*paddleNormal;
	Point3d paddleTangentialSpeed = speed-paddleNormalSpeed;

	double newSpinAmplitude = paddleTangentialSpeed.norm();
	if(newSpinAmplitude != 0)
		paddleTangentialSpeed /= newSpinAmplitude;
	newSpinAmplitude *= PADDLE_CONTACT_TIME/BALL_RADIUS;

	velocity -= paddleNormalSpeed;

	const Point3d normal = (velocity*paddleNormal)*paddleNormal;
	const Point3d tangential = velocity - normal;

	computeBouncing(normal,tangential,PADDLE_CONTACT_TIME,PADDLE_TANGENTIAL_FRICTION,BALL_OPPONENT_PADDLE_COR);

	//compute spin
	const Point3d newSpinAxis = paddleNormal^paddleTangentialSpeed;
	spinAxis = newSpinAxis*newSpinAmplitude+spinAxis*spinAmplitude;
	spinAmplitude = spinAxis.norm();
	if(spinAmplitude != 0)
		spinAxis /= spinAmplitude;

	spinAmplitude /= 100.;
}

