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
1. E. Lambrou and L. N. Gergidis, “A computational method for calculat ing the electrical and thermal properties of random composite” ,
Physica A: Statistical Mechanics and its Applications, Volume 642, 2024, 129760, ISSN 0378-4371,
https://doi.org/10.1016/j.physa.2024.129760
2. E. Lambrou and L. N. Gergidis, “A particle digitization-based computa tional method for continuum percolation,” Physica A: Statistical Mechanics
and its Applications, vol. 590, p. 126738, 2022

if you use this programm and write a paper or report please cite above papers

*/

#pragma once
#include <cmath>    // For std::cos, std::sin, std::sqrt, std::round
#include <iostream> // For std::cout in toString methods

// Define M_PI if it's not defined by <cmath> or another common header
#ifndef M_PI
#define M_PI 3.14159265358979323846 /* pi */
#endif

// Integer point class, defined by x and y coordinates
class iPoint
{
public:
    int x, y;
    // Constructor initializes x and y coordinates.
    iPoint(int ix, int iy);
};

// Range Class, representing a bounding box with integer coordinates
class Range
{
public:
    iPoint from, to;
    // Constructor initializes 'from' and 'to' points, ensuring 'from' is always
    // the lower-left and 'to' is the upper-right corner.
    Range(iPoint f, iPoint t);
};

// Point Class, representing a point with double precision coordinates
class Point
{
public:
    double x, y; // x,y coordinates
    // Constructor initializes x and y coordinates.
    Point(double xPos, double yPos);

    // Static method to transform a point given transformation center, angle, cos, and sin.
    static Point transform(Point original, Point transformCenter, double theta, double cosTheta, double sinTheta);

    // Static method to transform a point given transformation center and angle (calculates cos/sin internally).
    static Point transform(Point original, Point transformCenter, double theta);

    // Static method to rotate a point around the (0,0) origin.
    static Point transform(Point original, double theta);

    // Static method to transform point coordinates given transformation center coordinates, angle, cos, and sin.
    static Point transform(double x, double y, double transformCenterX, double transformCenterY, double theta, double cosTheta, double SinTheta);

    // Static method to transform point coordinates given transformation center coordinates and angle (calculates cos/sin internally).
    static Point transform(double x, double y, double transformCenterX, double transformCenterY, double theta);

    // Moves the point by dx and dy.
    void move(double dx, double dy);

    // Rounds the double coordinates of the point to integer coordinates.
    iPoint round(void);
};

// Line class, defined by two endpoints
class Line
{
public:
    Point start, end;
    // Constructor initializes the start and end points of the line.
    Line(Point start, Point end);
};

// Ellipse class, representing an ellipse with a center, semi-axes, and slope
class Ellipse {
public:
    double realArea; // This member seems redundant if area() calculates it. Consider removing.
    Point center;
    double a;       // Larger semi-axis (corresponds to x-axis before rotation)
    double b;       // Smaller semi-axis (corresponds to y-axis before rotation)
    double slope;   // Angle of rotation in radians

    // Constructor initializes the ellipse's properties.
    Ellipse(Point c, double ia, double ib, double iSlope);

    // Calculates the area of the ellipse.
    double area();

    // Prints the ellipse's parameters to the console.
    void toString(void);

    // Moves the ellipse by dx and dy.
    void move(double dx, double dy);

    // Calculates the maximum X-coordinate of the ellipse's bounding box.
    double maxX();

    // Calculates the maximum Y-coordinate of the ellipse's bounding box.
    double maxY();

    // Calculates the minimum X-coordinate of the ellipse's bounding box.
    double minX();

    // Calculates the minimum Y-coordinate of the ellipse's bounding box.
    double minY();
};

// Rectangle class, defined by lower left and upper right points,
// and derived properties like other corners, center, width, height, and boundary lines.
class Rectangle {
public:
    Point lowerLeft, upperRight;
    Point upperLeft, lowerRight; // Derived corner points
    Point center;
    double width, height;
    double realArea; // This member seems redundant if area() calculates it. Consider removing.

private:
    // Boundary lines of the rectangle. These are internal details.
    Line topLine, bottomLine, leftLine, rightLine;

public:
    // Constructor initializes the rectangle using its lower-left and upper-right corners.
    Rectangle(Point lowLeft, Point upRight);

    // Moves the rectangle by dx and dy, updating all its constituent points and lines.
    void move(double dx, double dy);

    // Calculates the area of the rectangle.
    double area();

    // Calculates the maximum X-coordinate of the rectangle's bounding box.
    double maxX();

    // Calculates the maximum Y-coordinate of the rectangle's bounding box.
    double maxY();

    // Calculates the minimum X-coordinate of the rectangle's bounding box.
    double minX();

    // Calculates the minimum Y-coordinate of the rectangle's bounding box.
    double minY();

    // Prints the rectangle's corner coordinates to the console.
    void toString(void);
};

// SlopedRectangle class, derived from Rectangle, representing a rectangle with an orientation (slope).
class SlopedRectangle : public Rectangle
{
public:
    double slope; // Angle of rotation in radians

    // Constructor initializes a sloped rectangle using its lower-left and upper-right points (before rotation), and a slope.
    // The base Rectangle constructor is called first, then corners are adjusted for slope.
    SlopedRectangle(Point dLeft, Point uRight, double Slope);

    // Constructor initializes a sloped rectangle using its center, width, height, and slope.
    // The base Rectangle constructor is called first, then corners are adjusted for slope.
    SlopedRectangle(Point Center, double Width, double Height, double Slope);

    // Prints the sloped rectangle's parameters and transformed corner coordinates to the console.
    void toString(void);
};