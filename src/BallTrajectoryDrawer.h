#ifndef BALL_DRAWER_H
#define BALL_DRAWER_H

#include "Mesh.h"
#include "Base.h"
#include "SoundEngine.h"
#include "Configuration.h"
#include "Table.h"
#include <vector>


class BallTrajectoryDrawer{
public:
	BallTrajectoryDrawer();
	virtual ~BallTrajectoryDrawer(){}
	void draw(unsigned int shaderId);
	void update(const double tau);
	void loadFiles();
	void close();

private:
	size_t currentFrame;
    std::vector<Point3d> points, translations, anglesAxis;
    std::vector<double> angles, lengths;
	Mesh ball, line;
	Table table;
};

#endif
