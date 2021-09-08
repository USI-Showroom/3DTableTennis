#include "Helpers.h"

double Helpers::sum(const double a[], const int n)
{
	double r = 0;
	for(int i = 0; i < n; i++){
		r += a[i];
	}
	return r;
}

double Helpers::sum(const double a[], const double val, const int n)
{
	double r = 0;
	for(int i = 0; i < n; i++){
		r += (a[i] - val)*(a[i] - val);
	}
	return r;
}

double Helpers::sum(const double a1[], const double val1, const double a2[], const double val2, const int n)
{
	double r = 0;
	for(int i = 0; i < n; i++){
		r += (a1[i] - val1)*(a2[i] - val2);
	}
	return r;
}

int Helpers::clamp(const int val, const int min, const int max)
{
	if(val > max)
		return max;
	else if(val < min)
		return min;
	else 
		return val;
}

Point3d Helpers::smooth(const std::deque<Point3d> &queue)
{
	static const double c1=2,c2=1,c3=1;
	static const double sum=c1+c2+c3;
	//return queue.front();
	const int size=queue.size();
    if(size > 2)
		return (c1*queue[0]+c2*queue[1]+c3*queue[2])/sum;
	
	return queue.front();
}