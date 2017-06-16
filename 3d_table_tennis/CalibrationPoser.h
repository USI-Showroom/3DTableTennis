#ifndef CALIBRATION_POSER_H
#define CALIBRATION_POSER_H

#include "Mesh.h"

class CalibrationPoser
{
private:

	Mesh mesh;
	float angle;

public:
	CalibrationPoser();
	virtual ~CalibrationPoser(){}

	void update(const double tau);
	void draw(unsigned int shaderId);
};

#endif
