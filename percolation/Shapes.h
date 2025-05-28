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
#include <cmath>    // For mathematical functions like std::cos, std::sin, std::sqrt, std::round
#include <iostream> // For input/output operations, specifically std::cout used in toString methods

// Define M_PI if it's not already defined to ensure portability.
// M_PI represents the mathematical constant Pi (approximately 3.14159265358979323846).
#ifndef M_PI
#define M_PI 3.14159265358979323846 /* pi */
#endif

// Represents an integer point in a 2D coordinate system.
class iPoint
{
public:
    int x, y; // X and Y coordinates of the point.
    // Constructor to initialize an iPoint with given x and y integer coordinates.
    iPoint(int ix, int iy);
};

// Represents a rectangular range or bounding box using two iPoint objects.
class Range
{
public:
    iPoint from, to; // 'from' typically represents the lower-left corner, 'to' the upper-right.
    // Constructor to initialize a Range. It ensures 'from' has the minimum coordinates
    // and 'to' has the maximum, effectively defining the bounding box.
    Range(iPoint f, iPoint t);
};

// Represents a floating-point point in a 2D coordinate system.
class Point
{
public:
    double x, y; // X and Y coordinates of the point using double precision.
    // Constructor to initialize a Point with given x and y double coordinates.
    Point(double xPos, double yPos);

    // Static method to transform (rotate and translate) a point.
    // It takes the original point, a transformation center, an angle (theta),
    // and pre-calculated cosine and sine of the angle for efficiency.
    static Point transform(Point original, Point transformCenter, double theta, double cosTheta, double sinTheta);

    // Static method to transform (rotate and translate) a point.
    // This overload calculates the cosine and sine of the angle internally.
    static Point transform(Point original, Point transformCenter, double theta);

    // Static method to rotate a point around the (0,0) origin.
    // @param original The point to be rotated.
    // @param theta The angle of rotation in radians.
    // @return The new rotated Point.
    static Point transform(Point original, double theta);

    // Static method to transform point coordinates (x,y) given a transformation center
    // and pre-calculated cosine and sine of the angle.
    static Point transform(double x, double y, double transformCenterX, double transformCenterY, double theta, double cosTheta, double SinTheta);

    // Static method to transform point coordinates (x,y) given a transformation center.
    // This overload calculates cosine and sine of the angle internally.
    static Point transform(double x, double y, double transformCenterX, double transformCenterY, double theta);

    // Moves the current point by adding dx to its x-coordinate and dy to its y-coordinate.
    void move(double dx, double dy);

    // Rounds the double precision coordinates of the point to integer coordinates,
    // returning a new iPoint object.
    iPoint round(void);
};

// Represents a line segment defined by two Point objects.
class Line
{
public:
    Point start, end; // The starting and ending points of the line.
    // Constructor to initialize a Line with a given start and end point.
    Line(Point start, Point end);
};

// Represents an ellipse with a center, semi-axes lengths, and an angle of rotation (slope).
class Ellipse {
public:
    double realArea; // Potentially redundant member to store the area, as area() method calculates it.
    Point center;    // The center point of the ellipse.
    double a;        // The length of the larger semi-axis.
    double b;        // The length of the smaller semi-axis.
    double slope;    // The angle of rotation of the ellipse in radians, relative to the x-axis.

    // Constructor to initialize an Ellipse with its center, semi-axes lengths, and slope.
    Ellipse(Point c, double ia, double ib, double iSlope);

    // Calculates and returns the area of the ellipse ($ \pi \cdot a \cdot b $).
    double area();

    // Prints the ellipse's parameters (center, semi-axes, slope) to the console.
    void toString(void);

    // Moves the ellipse by translating its center by dx and dy.
    void move(double dx, double dy);

    // Calculates and returns the maximum X-coordinate of the ellipse's bounding box.
    double maxX();

    // Calculates and returns the maximum Y-coordinate of the ellipse's bounding box.
    double maxY();

    // Calculates and returns the minimum X-coordinate of the ellipse's bounding box.
    double minX();

    // Calculates and returns the minimum Y-coordinate of the ellipse's bounding box.
    double minY();
};

// Represents an axis-aligned rectangle defined by its lower-left and upper-right corners.
// It also calculates and stores other derived properties like other corners, center, width, and height.
class Rectangle {
public:
    Point lowerLeft, upperRight;     // Essential points defining the rectangle.
    Point upperLeft, lowerRight;     // Derived corner points for convenience.
    Point center;                    // The center point of the rectangle.
    double width, height;            // Dimensions of the rectangle.
    double realArea;                 // Potentially redundant member to store the area.

private:
    // Internal members representing the four boundary lines of the rectangle.
    // These are private as they are internal details of the rectangle's definition.
    Line topLine, bottomLine, leftLine, rightLine;

public:
    // Constructor to initialize a Rectangle using its lower-left and upper-right corner points.
    // It also calculates and sets all derived properties like other corners, center, width, height, and boundary lines.
    Rectangle(Point lowLeft, Point upRight);

    // Moves the entire rectangle by translating all its constituent points (corners, center) and lines by dx and dy.
    void move(double dx, double dy);

    // Calculates and returns the area of the rectangle ($ \text{width} \cdot \text{height} $).
    double area();

    // Calculates and returns the maximum X-coordinate among all corners of the rectangle.
    double maxX();

    // Calculates and returns the maximum Y-coordinate among all corners of the rectangle.
    double maxY();

    // Calculates and returns the minimum X-coordinate among all corners of the rectangle.
    double minX();

    // Calculates and returns the minimum Y-coordinate among all corners of the rectangle.
    double minY();

    // Prints the coordinates of all four corners of the rectangle to the console.
    void toString(void);
};

// Represents a rectangle that can be rotated (sloped), inheriting properties from the base Rectangle class.
class SlopedRectangle : public Rectangle
{
public:
    double slope; // The angle of rotation of the sloped rectangle in radians.

    // Constructor to initialize a SlopedRectangle using its lower-left and upper-right points
    // *before* rotation, and a specified slope. The base Rectangle constructor is called
    // first, and then the corner points are adjusted based on the slope.
    SlopedRectangle(Point dLeft, Point uRight, double Slope);

    // Constructor to initialize a SlopedRectangle using its center point, width, height
    // (dimensions before rotation), and a specified slope. The base Rectangle constructor
    // is implicitly called, and then the corner points are calculated and adjusted for the slope.
    SlopedRectangle(Point Center, double Width, double Height, double Slope);

    // Prints the sloped rectangle's parameters (center, width, height, slope)
    // and the transformed (rotated) coordinates of its corner points to the console.
    void toString(void);
};