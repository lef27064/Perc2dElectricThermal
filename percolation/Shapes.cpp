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

#include <iostream>
#include "Shapes.h"
#include <algorithm>

using namespace std;

//initialize integer Point
iPoint::iPoint(int ix, int iy):x(ix), y(iy)
{}

//Range of integers
Range::Range(iPoint f, iPoint t) : from(f.x, f.y), to(t.x, t.y)
	{
	    //swap if from.x > to.x
		if (from.x > to.x)
		{
			iPoint hold(to);
			to.x = from.x;
			from.x = hold.x;
		}
		//same for the y
		if (from.y > to.y)
		{
			iPoint hold(to);
			to.y = from.y;
			from.y = hold.y;
		}
	}



Point::Point(double xPos, double yPos) : x(xPos), y(yPos)   //initialize
	{ }

	
/*
		Trasformation of coordinates
		Point original point
		Point transformCenter Center of trasformation
		x' = x*cos(theta) +y*sin(theta)
		y' = -x*sin(theta)+y*cos(theta)

	*/



	// General Case
 Point Point::transform(Point original, Point transformCenter, double theta, double cosTheta, double sinTheta)
{
	Point originalPoint(original.x - transformCenter.x, original.y - transformCenter.y);
	Point result(0, 0);
	if (theta == 0)
		return original;

	result.x = originalPoint.x *cosTheta + originalPoint.y*sinTheta;
	result.y = -originalPoint.x*sinTheta + originalPoint.y*cosTheta;

	result.x = result.x + transformCenter.x;
	result.y = result.y + transformCenter.y;

	return result;
}

// General Case
Point Point::transform(Point original, Point transformCenter, double theta)
	{
		Point originalPoint(original.x - transformCenter.x, original.y - transformCenter.y);
		Point result(0, 0);
		if (theta == 0)
			return original;

		double costheta = cos(theta);
		double sintheta = sin(theta);
		result.x = originalPoint.x *costheta + originalPoint.y*sintheta;
		result.y = -originalPoint.x*sintheta + originalPoint.y*costheta;

		result.x = result.x + transformCenter.x;
		result.y = result.y + transformCenter.y;

		return result;
	}

//Rotate around 0,0 origin
Point Point::transform(Point original, double theta)
	{
		Point transformCenter(0, 0);
		Point result = Point::transform(original, transformCenter, theta);
		return result;
	}


// General Case with double
Point Point::transform(double x, double y, double transformCenterX, double transformCenterY, double theta, double cosTheta,double sinTheta)
	{
		Point original(x, y);
		Point transformCenter(transformCenterX, transformCenterY);
		Point result= Point::transform(original, transformCenter, theta, cosTheta, sinTheta);
		return result;
	}

Point Point::transform(double x, double y, double transformCenterX, double transformCenterY, double theta)
{
	Point original(x, y);
	Point transformCenter(transformCenterX, transformCenterY);
	Point result = transform(original, transformCenter, theta);
	return result;
}


//move by dx dy 
void Point::move(double dx, double dy)
	{
		x += dx;
		y += dy;
	}

//round Coordinates
iPoint Point::round(void)
{
	iPoint result(0, 0);
	result.x = int(std::round(x));
	result.y = int(std::round(y));

	return result;
}

//initialize line
Line::Line(Point start, Point end) : start(start), end(end)
	{
	}


//initialize
Ellipse::Ellipse(Point c, double ia, double ib, double iSlope) : center(c), a(ia), b(ib), slope(iSlope)
	{
	}


//Area of ellipse  A=ð*a*b
double Ellipse::area()
	{
		return M_PI * a*b;
	}


//ôïString	
void Ellipse::toString(void)
{
		cout << "\n";
		cout << "--------------------------------------------------\n";
		cout << "Ellipse. Parameters:\n";
		cout << "Center/  X=" << center.x << " Y=" << center.y << "\n";
		cout << "a =" << a << " b =" << b << "\n";
		cout << "Slope" << slope << "\n";
		cout << "--------------------------------------------------\n";

}

//move by dx & dy
void Ellipse::move(double dx, double dy)
	{
		center.x += dx;
		center.y += dy;
	}


//max X
double Ellipse::maxX()
	{
		return center.x + sqrt(a*a *cos(slope)*cos(slope) + b * b *sin(slope)*sin(slope));
	}


//max Y
double Ellipse::maxY()
	{
		return center.y + sqrt(a*a *sin(slope)*sin(slope) + b * b *cos(slope)*cos(slope));
	}

//min X
double Ellipse::minX()
	{
		return center.x - sqrt(a*a *cos(slope)*cos(slope) + b * b *sin(slope)*sin(slope));
	}
	
//min Y
double Ellipse::minY()
	{
		return center.y - sqrt(a*a *sin(slope)*sin(slope) + b * b *cos(slope)*cos(slope));
	}



// Rectangle class, defined by lower left and upper right points
// Initialize
Rectangle::Rectangle(Point lowLeft, Point upRight): lowerLeft(lowLeft)
		, upperRight(upRight)
		, upperLeft(Point(lowLeft.x, upRight.y))
		, lowerRight(Point(upRight.x, lowLeft.y))
		, topLine(upperLeft, upperRight)
		, bottomLine(lowerLeft, lowerRight)
		, leftLine(lowerLeft, upperLeft)
		, rightLine(lowerRight, upperRight)
		, center((upRight.x + lowLeft.x) / 2, (upRight.y + lowLeft.y) / 2)
		, width(upRight.x - lowLeft.x)
	    , height(upRight.y - lowLeft.y)

	{ }

//Move
void Rectangle::move(double dx, double dy)
	{
		//Move all points
		center.move(dx, dy);
		upperRight.move(dx, dy);
		upperLeft.move(dx, dy);
		lowerRight.move(dx, dy);
		lowerLeft.move(dx, dy);
		topLine.start.move(dx, dy);
		topLine.end.move(dx, dy);
		bottomLine.start.move(dx, dy);
		bottomLine.end.move(dx, dy);
		leftLine.start.move(dx, dy);
		leftLine.end.move(dx, dy);
		rightLine.start.move(dx, dy);
		rightLine.end.move(dx, dy);

	}
	

//area of rectangle
double Rectangle::area()
	{
		return height * width;
	}


// Max - Min of X and Y
// Get maximum - minimum of all corners
//max X
double Rectangle::maxX()
	{
		return std::max(std::max(lowerLeft.x, upperLeft.x), std::max(lowerRight.x, upperRight.x));
	}

//maxY	
double Rectangle::maxY()
	{
		return std::max(std::max(lowerLeft.y, upperLeft.y), std::max(lowerRight.y, upperRight.y));
	}

//minX
double Rectangle::minX()
	{
		return std::min(std::min(lowerLeft.x, upperLeft.x), std::min(lowerRight.x, upperRight.x));
	}

//mimY
double Rectangle::minY()
	{
		return std::min(std::min(lowerLeft.y, upperLeft.y), std::min(lowerRight.y, upperRight.y));
	}


//ToString
void Rectangle::toString(void)
	{
		cout << "--------------------------------------------------\n";
		cout << "Rectangle parameters:\n";
		cout << "Upper Right/  X=" << upperRight.x << " Y=" << upperRight.y << "\n";
		cout << "Upper Left/  X=" << upperLeft.x << " Y=" << upperLeft.y << "\n";
		cout << "Lower Right/  X=" << lowerRight.x << " Y=" << lowerRight.y << "\n";
		cout << "Lower Left/  X=" << lowerLeft.x << " Y=" << lowerLeft.y << "\n";
		cout << "--------------------------------------------------\n";
	}


// initialize
// Same as Rectangle but different corners coordinates
SlopedRectangle::SlopedRectangle(Point dLeft, Point uRight, double Slope) :Rectangle(dLeft, uRight), slope(Slope)
{
	//find corners by rotate Points
	upperRight = Point((center.transform(center.x + (width / 2), center.y + (height / 2), center.x, center.y, Slope)));
	upperLeft = Point((center.transform(center.x - (width / 2), center.y + (height / 2), center.x, center.y, Slope)));
	lowerRight = Point((center.transform(center.x + (width / 2), center.y - (height / 2), center.x, center.y, Slope)));
	lowerLeft = Point((center.transform(center.x - (width / 2), center.y - (height / 2), center.x, center.y, Slope)));
}

SlopedRectangle::SlopedRectangle(Point Center, double Width, double Height, double Slope):Rectangle(Point(Center.x - Width / 2, Center.y - Height / 2), Point(Center.x + Width / 2, Center.y + Height / 2))	, slope(Slope)
{
	upperRight = Point((center.transform(center.x + (width / 2), center.y + (height / 2), center.x, center.y, Slope)));
	upperLeft = Point((center.transform(center.x - (width / 2), center.y + (height / 2), center.x, center.y, Slope)));
	lowerRight = Point((center.transform(center.x + (width / 2), center.y - (height / 2), center.x, center.y, Slope)));
	lowerLeft = Point((center.transform(center.x - (width / 2), center.y - (height / 2), center.x, center.y, Slope)));
}

//To String
void SlopedRectangle::toString(void)
{
	cout << "\n";
	cout << "--------------------------------------------------\n";
	cout << "Rectangle With Slope. Parameters:\n";
	cout << "Center/  X=" << center.x << " Y=" << center.y << "\n";
	cout << "Width =" << width << "\n" << "Height =" << height << "\n" << "Slope = " << slope << "\n" << "sin(Slope) = " << sin(slope) << "\n";
	cout << "Upper Right/  X=" << upperRight.x << " Y=" << upperRight.y << "\n";
	cout << "Upper Left/  X=" << upperLeft.x << " Y=" << upperLeft.y << "\n";
	cout << "Lower Right/  X=" << lowerRight.x << " Y=" << lowerRight.y << "\n";
	cout << "Lower Left/  X=" << lowerLeft.x << " Y=" << lowerLeft.y << "\n";
	cout << "--------------------------------------------------\n";

}




