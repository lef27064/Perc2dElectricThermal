/*This file is part of Electric,Thermal, Mechanical Properties
Estimation With Percolation Theory (ETMPEWPT) (2D version) program.

Created from Eleftherios Lamprou lef27064@otenet.gr during PhD thesis (2017-2024)

ETMPEWPT is free software : you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free software Foundation, either version 3 of the License, or
(at your option) any later version.

ETMPEWPT is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Foobar.If not, see < https://www.gnu.org/licenses/>.
*/

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

