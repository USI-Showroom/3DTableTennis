#include "BallLauncher.h"
#include "Configuration.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


EasyLauncher::EasyLauncher():
	BallLauncher(),
    rangeX((int)(Configuration::getInstance()->get("TABLE_WIDTH")*0.8*1000.0)),
    startY(Configuration::getInstance()->get("TABLE_HEIGHT")),
    startZ(-Configuration::getInstance()->get("TABLE_LENGTH"))
{
    srand ( time(NULL) );
}

trajectory EasyLauncher::nextTrajectory()
{
    trajectory res;
    
    double vz = 1.5 + (rand()%1000)/1000.0;
	double x = ((rand()%rangeX)-rangeX/2.0)/1000.0;
	double y = startY + 0.7 + (rand()%1000)/4000.0;
	//double vz = 2;
	//double x = (rangeX/4.0)/1000.0;
	//double y = startY + 0.7;

    res.position = Point3d(x,y,startZ);
    
    res.velocity = Point3d(0,0,vz);
    
    res.acceleration = Point3d(0,0,0);
    res.spinAxis = Point3d(1,0,0);
    res.spinAmplitude = 0;
    
    return res;
}
