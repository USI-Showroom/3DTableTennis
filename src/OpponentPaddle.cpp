#include "OpponentPaddle.h"
#include "Configuration.h"
#include "Defines.h"

OpponentPaddle::OpponentPaddle(Ball* ball) :
	ball(ball),
	normal(0,0,1),
	posZ(-Configuration::getInstance()->get("TABLE_LENGTH")-0.1),
	speed(Configuration::getInstance()->get("OPPONENT_SPEED")),
	velocity(0,0,0),
	mesh(Mesh::meshFromOBJ(Path::meshFolder()+"\\paddle.obj")),
	inLaunchPosition(false)
{
	position = Point3d(0,1,posZ);
	normal.normalize();
}

Point3d OpponentPaddle::getPosition(bool cm)
{
	return position;
}

bool OpponentPaddle::isInitialized()
{
	return true;
}

Point3d OpponentPaddle::getNormal()
{
	return normal;
}

Point3d OpponentPaddle::getVelocity()
{
	return velocity; //m/s
}

bool OpponentPaddle::isInPosition()
{
	return inLaunchPosition;
}

void OpponentPaddle::update(const double tau)
{
	Point3d posDirection;
	if(ball->getVelocity().z() < 0 && !ball->isNetHit())
	{
		posDirection = ball->getPosition() - position;
	}
	else if(ball->isReadyToLaunch())
	{
		posDirection = ball->getStartPosition() + Point3d(0,0,0.1) - position;
		inLaunchPosition = posDirection.norm() <= 0.201;
	}
	else
	{
		Point3d defaultPosition = Point3d(0,1.2,0); 
		posDirection = defaultPosition - position;
	}
	posDirection.normalize();
	position+=speed*posDirection;
	position[2] = posZ;

	Point3d normalGoal(0,1.5,-1);
	normal = normalGoal - position;
	normal.normalize();
}

void OpponentPaddle::draw(unsigned int shaderId)
{
	glPushMatrix();
	
	glTranslatef(position.x(),position.y(),position.z());

	gluLookAt(0,0,0,normal.x(),normal.y(),-normal.z(),0,1,0);

	glRotated(180,0,1,0);
	glRotated(90,1,0,0);

	mesh.draw(shaderId);

	glPopMatrix();

}