#include "BallTrajectoryDrawer.h"
#include "Defines.h"
#include <sstream>
#include "ReplayManager.h"

#include <QtOpenGL>



BallTrajectoryDrawer::BallTrajectoryDrawer()
    :ball(Mesh::meshFromOBJ(Path::meshFolder()+"\\ball.obj")),
    line(Mesh::meshFromOBJ(Path::meshFolder()+"\\line.obj")),
    points(),translations(),lengths(), angles(), anglesAxis(),
    currentFrame(0), table()
{}


void BallTrajectoryDrawer::close()
{
	ReplayManager::Instance().clear();
}

void BallTrajectoryDrawer::loadFiles()
{
	points.clear();
	translations.clear();
	lengths.clear();
	angles.clear();
	anglesAxis.clear();

	points=ReplayManager::Instance().getPoints();

    int offset=5;
    for(size_t i=offset;i<points.size();i+=offset)
    {
        Point3d toConvert=points[i]-points[i-offset];
        double norm=toConvert.norm();

		if(norm>2) continue;

		toConvert/=norm;

        double angle=acos(toConvert.z())/M_PI*180.;
		Point3d axis=toConvert^Point3d(0,0,-1);
		//axis.normalize();

		//std::cout<<norm<<std::endl;
        
        translations.push_back(points[i-offset]);
        lengths.push_back(norm/0.02);
        angles.push_back(angle);
        anglesAxis.push_back(axis);
    }
}

void BallTrajectoryDrawer::draw(unsigned int shaderId)
{
	table.draw(shaderId);

	glPushMatrix();
    Point3d &position=points[currentFrame];
	glTranslated (position[0], position[1], position[2]);

	ball.draw(shaderId);

	glPopMatrix();
    
    
    
    for(size_t i=0;i<lengths.size();++i)
    {
        glPushMatrix();
        Point3d &trans=translations[i];
        double scale=lengths[i];
        double angle=angles[i];
        Point3d &axis=anglesAxis[i];
        
        
		
        glTranslated(trans.x(),trans.y(),trans.z());
        glRotatef(angle,axis.x(),axis.y(),axis.z());
		glScaled(0.5,0.5,scale);
        
        line.draw(shaderId);
        glPopMatrix();
    }
    
    
}



void BallTrajectoryDrawer::update(const double tau)
{
	table.update(tau);

    ++currentFrame;

	if(currentFrame>=points.size())
		currentFrame=0;
}