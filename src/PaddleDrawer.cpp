#include "PaddleDrawer.h"
#include "Base.h"
#include "Defines.h"

#include <Eigen/Eigen>
using namespace Eigen;

PaddleDrawer *PaddleDrawer::getDrawer()
{
	static PaddleDrawer theInstance;
	return &theInstance;
}

PaddleDrawer::PaddleDrawer()
	:mesh(Mesh::meshFromOBJ(Path::meshFolder()+"\\paddle.obj"))
{
#ifndef USE_VIRTUAL_PADDLE
	paddle = Paddle::getInstance();
#else
	paddle = VirtualPaddle::getPaddle();
#endif
}

void PaddleDrawer::draw(unsigned int shaderId)
{
	glPushMatrix();

	Point3d pos = paddle->getPosition();
	glTranslatef(pos.x(),pos.y(),pos.z());

	Point3d normal = paddle->getNormal();
	double x=normal.x(), y=normal.y(), z=normal.z();

	/*double angleX=-atan2(x,sqrt(y*y+z*z))/M_PI*180.0;
	double angleY=atan2(y,sqrt(x*x+z*z))/M_PI*180.0;
	double angleZ=atan2(z,sqrt(x*x+y*y))/M_PI*180.0;*/

	//glRotated(angleX,0,0,1);
	//glRotated(angleY,0,1,0);
	//glRotated(angleZ,1,0,0);
	
	gluLookAt(0,0,0,-x,-y,z,0,1,0);

	Point3d elbowPos = paddle->getElbowPos();
	double ex=elbowPos.x(), ey=elbowPos.y(), ez=elbowPos.z();

	Point3d handle = pos - elbowPos;
	handle.z() = 0;
	double handleRot = handle.getAngle(Point3d(1,0,0));

	glRotated((handleRot / M_PI * 180) - 90, 0,0,1);
	glRotated(90,1,0,0);

	mesh.draw(shaderId);
	glPopMatrix();

	//glDisable(GL_CULL_FACE);
	//glPushMatrix();

	//glTranslatef(pos.x(),pos.y(),pos.z());
	//gluLookAt(0,0,0,-x,-y,z,0,1,0);

	//glColor3f(0.0,0.0,1.0); 
	//glBegin(GL_QUADS);
 //   glVertex3f(-0.25,-0.25,0);
 //   glVertex3f(0.25,-0.25,0);
 //   glVertex3f(0.25,0.25,0);
 //   glVertex3f(-0.25,0.25,0);
 //   glEnd();

	//glPopMatrix();
	
	/*glPointSize(20);
	glBegin(GL_POINTS);
	glPointSize(20);
	
	int isTexture_location = glGetUniformLocation(shaderId, "isTexture");
	glUniform1i(isTexture_location, 1);
	glColor3d(0,1,0);

	for(vector<Point3d>::iterator it=paddle->allPoints.begin();it!=paddle->allPoints.end();++it){
	
	glPointSize(20);
		glVertex3d(it->x(),it->y(),it->z());
		
	glPointSize(20);
	}
	
	glPointSize(20);
	glEnd();*/
}

