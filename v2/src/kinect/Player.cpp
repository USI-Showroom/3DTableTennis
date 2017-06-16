#include "Player.h"
#include "Defines.h"


PingPongPlayer* PingPongPlayer::getInstance()
{
	static PingPongPlayer theInstance;
	return &theInstance;
}

PingPongPlayer::PingPongPlayer():
    head(), 
	hip(), 
	leftFoot(), 
	rightFoot(), 
	leftHand(), 
	rightHand(),
	isLefty(false)
{ }

void PingPongPlayer::setHead(const double x,const double y,const double z)
{ 
    Point3d tmp(x,y,z);
    head = tmp;
}

void PingPongPlayer::setHip(const double x,const double y,const double z)
{ 
    Point3d tmp(x,y,z);
    hip = tmp;
}

void PingPongPlayer::setHand(const double x,const double y,const double z,const bool left)
{
    Point3d tmp(x,y,z);
    if (left)
        leftHand = tmp;
    else
        rightHand = tmp;
}

void PingPongPlayer::setFoot(const double x,const double y,const double z,const bool left)
{
    Point3d tmp(x,y,z);
    if (left)
        leftFoot = tmp;
    else
        rightFoot = tmp;
}

void PingPongPlayer::setLefty(bool lefty)
{
	isLefty = lefty;
}

Point3d PingPongPlayer::getHead()
{
    return head;
}

Point3d PingPongPlayer::getHip()
{
    return hip;
}

Point3d PingPongPlayer::getLeftHand()
{
    return leftHand;
}

Point3d PingPongPlayer::getRightHand()
{
    return rightHand;
}

Point3d PingPongPlayer::getLeftFoot()
{
    return leftFoot;
}

Point3d PingPongPlayer::getRightFoot()
{
    return rightFoot;
}

Point3d PingPongPlayer::getPlayingHand()
{
    return isLefty ? leftHand : rightHand;
}

double PingPongPlayer::footHeight()
{   
    return min(leftFoot.y(), rightFoot.y());
}

bool PingPongPlayer::getLefty()
{
	return isLefty;
}