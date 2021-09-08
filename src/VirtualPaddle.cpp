#include "Configuration.h"
#include "VirtualPaddle.h"
#include "Defines.h"



VirtualPaddle *VirtualPaddle::instance = NULL;

VirtualPaddle::VirtualPaddle()
	:normal(0,1,0)
{
	normal.normalize();
}


VirtualPaddle *VirtualPaddle::getPaddle()
{
	if(VirtualPaddle::instance == NULL)
		VirtualPaddle::instance=new VirtualPaddle();

	return VirtualPaddle::instance;
}

Point3d VirtualPaddle::getPosition(bool cm)
{
	return Point3d(0, Configuration::getInstance()->get("TABLE_HEIGHT")+0.1, -0.1);
}

bool VirtualPaddle::isInitialized()
{
	return true;
}

Point3d VirtualPaddle::getNormal()
{
	//double x, y, z;
	//srand(time(NULL));
	//x = (double)rand() / (double)rand();
	//y = (double)rand() / (double)rand();
	//z = (double)rand() / (double)rand();

	//normal = Point3d(x, y, z);

	return normal;
}

Point3d VirtualPaddle::getSpeed()
{
	return Point3d(0, 0.1, 0); //m/s
}