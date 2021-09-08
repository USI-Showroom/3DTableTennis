#ifndef HELPERS_H
#define HELPERS_H

#include <list>
#include "Base.h"
#include <deque>

class Helpers
{
public:
	static double sum(const double a[], const int n);
	static double sum(const double a[], const double val, const int n);	
	static double sum(const double a1[], const double val1, const double a2[], const double val2, const int n);

	static int clamp(const int val, const int min, const int max);

	static Point3d smooth(const std::deque<Point3d> &queue);
};
#endif