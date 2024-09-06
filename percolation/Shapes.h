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

Theory of this is published in two papers:
1. E. Lambrou and L. N. Gergidis, “A computational method for calculating the electrical and thermal properties of random composite” ,
Physica A: Statistical Mechanics and its Applications, Volume 642, 2024, 129760, ISSN 0378-4371,
https://doi.org/10.1016/j.physa.2024.129760
2. E. Lambrou and L. N. Gergidis, “A particle digitization-based computational method for continuum percolation,” Physica A: Statistical Mechanics
and its Applications, vol. 590, p. 126738, 2022

if you use this programm and write a paper or report please cite above papers

*/ 

#pragma once
#include <cmath>
#include <iostream>


#define M_PI           3.14159265358979323846  /* pi */
// Integer point class, defined by x and y coordinates
// 

class iPoint 
{
	public:
		int x, y;
		iPoint(int ix, int iy);
		
};

//
//Range Class 
class Range
{
	public:
		iPoint from, to;
		Range(iPoint f, iPoint t);
};

//Point Class
class Point
{
	public:
		double x, y;   //x,y coordinates
		Point(double xPos, double yPos);

		static Point transform(Point original, Point transformCenter, double theta, double cosTheta, double sinTheta);

		// General Case
		static Point transform(Point original, Point transformCenter, double theta);

		//Rotate around 0,0 origin
		static Point transform(Point original, double theta);

		static Point transform(double x, double y, double transformCenterX, double transformCenterY, double theta, double cosTheta, double SinTheta);
	
		// General Case with double
		static Point transform(double x, double y, double transformCenterX, double transformCenterY, double theta);
	
		//move by dx dy 
		void move(double dx, double dy);

		iPoint round(void);
	
};


// Line class, defined by two endpoints
class Line 
{
	public:
		Point start, end;
		Line(Point start, Point end);
	
};


//Ellipse
class Ellipse {
public:
	double realArea;
	Point center;
	double a;       //larger semiaxe (x axe)
	double b;       //smaller
	double slope;   //slope 

	//initialize
	Ellipse(Point c, double ia, double ib, double iSlope);
	//Area of ellipse
	double area();
	
	//
	void toString(void);
	

	//move by dx & dy
	void move(double dx, double dy);
	
	//max X
	double maxX();
	

	//max Y
	double maxY();

	//min X
	double minX();
	
	//min Y
	double minY();
	
};



// Rectangle class, defined by lower left and upper right points
class Rectangle {
public:
	Point lowerLeft, upperRight;
	Point upperLeft, lowerRight;
	Point center;
	double width, height;
	double realArea;

private:
	Line topLine, bottomLine, leftLine, rightLine;

public:
	// Initialise object with corner points and edge lines
	Rectangle(Point lowLeft, Point upRight);
		
	void move(double dx, double dy);
	//area of rectangle
	double area();
	
	//max X
	double maxX();
	
	//maxY	
	double maxY();
	
	double minX();

	double minY();
	
	void toString(void);

};


class SlopedRectangle : public Rectangle
{
public:
	double	slope;   //slope

	//initialize
	SlopedRectangle(Point dLeft, Point uRight, double Slope);

	SlopedRectangle(Point Center, double Width, double Height, double Slope);

	//To String
	void toString(void);

};