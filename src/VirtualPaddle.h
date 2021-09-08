#ifndef VIRTUAL_PADDLE_H
#define VIRTUAL_PADDLE_H

#include "Base.h"

class VirtualPaddle
{
private:
	static VirtualPaddle *instance;

	VirtualPaddle();
	~VirtualPaddle() {}

	Point3d normal;

public:
	static VirtualPaddle *getPaddle();

	bool isInitialized();
	Point3d getNormal();
	Point3d getPosition(bool cm);
	Point3d getSpeed();


	void update(){}

};

#endif
