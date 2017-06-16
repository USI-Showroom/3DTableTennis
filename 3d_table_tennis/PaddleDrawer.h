#ifndef PADDLE_DRAWER_H
#define PADDLE_DRAWER_H

#include "Mesh.h"
#include "Paddle.h"
#include "VirtualPaddle.h"

class PaddleDrawer
{
private:
	//static PaddleDrawer *instance;
	Mesh mesh;
#ifndef USE_VIRTUAL_PADDLE
		Paddle* paddle;
#else
		VirtualPaddle* paddle;
#endif
	PaddleDrawer();
	virtual ~PaddleDrawer(){};

public:
	static PaddleDrawer *getDrawer();
	void draw(unsigned int shaderId);


};

#endif