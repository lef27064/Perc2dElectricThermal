#include "cluster.h"

//initialize


point2d::point2d(int a, int b) 
{
	x = a; y = b;
}
point2d::point2d(void) :x(0), y(0)
{
	
}

doublepoint2d::doublepoint2d(void)
{
	x = 0.0;
	y = 0.0;
}

doublepoint2d::doublepoint2d(double a, double b)
{
	x = a;
	y = b;
}


Cluster::Cluster(void)
{
	radius = 0.0;
	totalPoints = 0;
	inertia = 0.0;

}

