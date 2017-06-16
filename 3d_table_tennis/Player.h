#ifndef _PingPongPlayer_h
#define _PingPongPlayer_h

#include "Base.h"
#include "Helpers.h"

class PingPongPlayer
{
private:
	static const int listSize = 5;
	Point3d head, hip, leftFoot, rightFoot, leftHand, rightHand, leftElbow, rightElbow;
	bool isLefty;
    
    PingPongPlayer();
    virtual ~PingPongPlayer(){}
    
public:
    Point3d getHead();
	Point3d getHip();
	Point3d getLeftHand();
	Point3d getRightHand();
	Point3d getLeftElbow();
	Point3d getRightElbow();
	Point3d getLeftFoot();
	Point3d getRightFoot();

	Point3d getPlayingHand();
	Point3d getPlayingElbow();
	bool getLefty();
    double footHeight();
    
    static PingPongPlayer* getInstance();

	void setHead(const double x, const double y, const double z);
	void setHip(const double x, const double y, const double z);
    void setHand(const double x, const double y, const double z, const bool left);
	void setElbow(const double x, const double y, const double z, const bool left);
    void setFoot(const double x, const double y, const double z, const bool left);
	void setLefty(bool lefty);   
};

#endif
