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

#include <iostream>   // For standard input/output operations, like std::cout.
#include "Shapes.h"   // Includes the header file that declares the geometric shape classes (iPoint, Point, Ellipse, Rectangle, etc.).
#include <algorithm>  // Provides functions like std::min, std::max, and std::swap for numerical operations.
#include <cmath>      // Provides mathematical functions such as std::cos, std::sin, std::sqrt, and std::round.

// It's generally good practice to avoid 'using namespace std;' in header files
// and often in .cpp files too, to prevent naming conflicts. Explicitly using 'std::' is preferred.

// --- iPoint Class Implementation ---

// Constructor for iPoint: Initializes an integer point with given x and y coordinates.
iPoint::iPoint(int ix, int iy) : x(ix), y(iy)
{
}

// --- Range Class Implementation ---

// Constructor for Range: Initializes a rectangular range defined by two iPoints (from and to).
// It ensures that 'from' always represents the lower-left corner and 'to' the upper-right corner
// by swapping coordinates if they are out of order.
Range::Range(iPoint f, iPoint t) : from(f.x, f.y), to(t.x, t.y)
{
    // Ensure 'from.x' is less than or equal to 'to.x'.
    if (from.x > to.x)
    {
        std::swap(from.x, to.x); // Swaps the x-coordinates if 'from.x' is greater than 'to.x'.
    }
    // Ensure 'from.y' is less than or equal to 'to.y'.
    if (from.y > to.y)
    {
        std::swap(from.y, to.y); // Swaps the y-coordinates if 'from.y' is greater than 'to.y'.
    }
}

// --- Point Class Implementation ---

// Constructor for Point: Initializes a floating-point point with given x and y coordinates.
Point::Point(double xPos, double yPos) : x(xPos), y(yPos)
{
}

/*
 * Transformation of coordinates for a point around a center.
 * The formulas for rotation are:
 * x' = x*cos(theta) + y*sin(theta)
 * y' = -x*sin(theta) + y*cos(theta)
 * where (x,y) are the translated coordinates relative to the center.
*/

// Static method: Transforms an 'original' point by rotating it around a 'transformCenter'.
// It takes pre-calculated cosine and sine values for efficiency.
Point Point::transform(Point original, Point transformCenter, double theta, double cosTheta, double sinTheta)
{
    // 1. Translate the original point so that the transformCenter becomes the origin (0,0).
    double translated_x = original.x - transformCenter.x;
    double translated_y = original.y - transformCenter.y;

    // 2. Apply the 2D rotation formulas to the translated point.
    double rotated_x = translated_x * cosTheta + translated_y * sinTheta;
    double rotated_y = -translated_x * sinTheta + translated_y * cosTheta;

    // 3. Translate the rotated point back to its original coordinate system by adding the transformCenter's coordinates.
    Point result(rotated_x + transformCenter.x, rotated_y + transformCenter.y);
    return result;
}

// Static method: Transforms an 'original' point around a 'transformCenter'.
// This overload calculates the cosine and sine of the angle internally.
Point Point::transform(Point original, Point transformCenter, double theta)
{
    // Optimization: If the rotation angle is 0, no transformation is needed.
    // Direct comparison of doubles to 0.0 can be problematic due to floating-point precision;
    // a small epsilon comparison (e.g., fabs(theta) < epsilon) would be safer for general use.
    if (theta == 0.0)
        return original;

    // Calculate cosine and sine of the angle.
    double cosTheta = std::cos(theta);
    double sinTheta = std::sin(theta);

    // Reuse the more efficient overloaded transform function that accepts pre-calculated cos/sin.
    return transform(original, transformCenter, theta, cosTheta, sinTheta);
}

// Static method: Rotates an 'original' point around the global origin (0,0).
Point Point::transform(Point original, double theta)
{
    // Define the origin (0,0) as the transformation center.
    Point transformCenter(0.0, 0.0);
    // Reuse the general transform function.
    return Point::transform(original, transformCenter, theta);
}

// Static method: Transforms a point given its individual x, y coordinates,
// a transform center's x, y coordinates, angle, and pre-calculated cos/sin.
Point Point::transform(double x, double y, double transformCenterX, double transformCenterY, double theta, double cosTheta, double sinTheta)
{
    // Create Point objects from the individual coordinates for convenience.
    Point original(x, y);
    Point transformCenter(transformCenterX, transformCenterY);
    // Reuse the transform function that operates on Point objects.
    return Point::transform(original, transformCenter, theta, cosTheta, sinTheta);
}

// Static method: Transforms a point given its individual x, y coordinates,
// a transform center's x, y coordinates, and the angle.
// This overload calculates cos/sin internally.
Point Point::transform(double x, double y, double transformCenterX, double transformCenterY, double theta)
{
    // Create Point objects from the individual coordinates.
    Point original(x, y);
    Point transformCenter(transformCenterX, transformCenterY);
    // Reuse the transform function that operates on Point objects.
    return transform(original, transformCenter, theta);
}

// Moves the current point by adding 'dx' to its x-coordinate and 'dy' to its y-coordinate.
void Point::move(double dx, double dy)
{
    x += dx;
    y += dy;
}

// Rounds the double-precision coordinates of the point to the nearest integers,
// returning a new iPoint object.
iPoint Point::round(void)
{
    // Uses std::round to round to the nearest integer. static_cast<int> truncates.
    return iPoint(static_cast<int>(std::round(x)), static_cast<int>(std::round(y)));
}

// --- Line Class Implementation ---

// Constructor for Line: Initializes a line segment with a given start and end point.
Line::Line(Point start, Point end) : start(start), end(end)
{
}

// --- Ellipse Class Implementation ---

// Constructor for Ellipse: Initializes an ellipse with its center, semi-axes lengths (a and b), and slope.
Ellipse::Ellipse(Point c, double ia, double ib, double iSlope) : center(c), a(ia), b(ib), slope(iSlope)
{
    // Optional: Add validation for 'a' and 'b' to ensure they are positive values.
    // For example: if (a <= 0 || b <= 0) { /* handle error: throw exception or set default */ }
    // The 'realArea' member is not updated here, suggesting it might be calculated on demand by the area() method.
}

// Calculates and returns the area of the ellipse using the formula A = ð * a * b.
double Ellipse::area()
{
    return M_PI * a * b;
}

// Prints the ellipse's parameters (center, semi-axes, slope in radians and degrees, and area) to the console.
void Ellipse::toString(void)
{
    std::cout << "\n";
    std::cout << "--------------------------------------------------\n";
    std::cout << "Ellipse Parameters:\n";
    std::cout << "Center/  X=" << center.x << " Y=" << center.y << "\n";
    std::cout << "a =" << a << " b =" << b << "\n";
    std::cout << "Slope =" << slope << " radians (" << slope * 180.0 / M_PI << " degrees)\n"; // Converts radians to degrees for readability.
    std::cout << "Area =" << area() << "\n"; // Calls the area() method to get the current area.
    std::cout << "--------------------------------------------------\n";
}

// Moves the ellipse by translating its center by 'dx' in the x-direction and 'dy' in the y-direction.
void Ellipse::move(double dx, double dy)
{
    center.move(dx, dy); // Reuses the Point::move method for the center.
}

// Calculates and returns the maximum X-coordinate of the ellipse's bounding box.
// This uses a general formula for the extent of a rotated ellipse.
double Ellipse::maxX()
{
    double cos_s = std::cos(slope);
    double sin_s = std::sin(slope);
    return center.x + std::sqrt(a * a * cos_s * cos_s + b * b * sin_s * sin_s);
}

// Calculates and returns the maximum Y-coordinate of the ellipse's bounding box.
// This uses a general formula for the extent of a rotated ellipse.
double Ellipse::maxY()
{
    double cos_s = std::cos(slope);
    double sin_s = std::sin(slope);
    return center.y + std::sqrt(a * a * sin_s * sin_s + b * b * cos_s * cos_s);
}

// Calculates and returns the minimum X-coordinate of the ellipse's bounding box.
// This uses a general formula for the extent of a rotated ellipse.
double Ellipse::minX()
{
    double cos_s = std::cos(slope);
    double sin_s = std::sin(slope);
    return center.x - std::sqrt(a * a * cos_s * cos_s + b * b * sin_s * sin_s);
}

// Calculates and returns the minimum Y-coordinate of the ellipse's bounding box.
// This uses a general formula for the extent of a rotated ellipse.
double Ellipse::minY()
{
    double cos_s = std::cos(slope);
    double sin_s = std::sin(slope);
    return center.y - std::sqrt(a * a * sin_s * sin_s + b * b * cos_s * cos_s);
}

// --- Rectangle Class Implementation ---

// Constructor for Rectangle: Initializes an axis-aligned rectangle.
// It takes the lower-left and upper-right points and automatically calculates
// the other corners, center, width, height, and defines its boundary lines.
Rectangle::Rectangle(Point lowLeft, Point upRight)
    : lowerLeft(lowLeft), upperRight(upRight),
    upperLeft(lowLeft.x, upRight.y),       // Derived: same x as lowerLeft, same y as upperRight.
    lowerRight(upRight.x, lowLeft.y),      // Derived: same x as upperRight, same y as lowerLeft.
    center((upRight.x + lowLeft.x) / 2.0, (upRight.y + lowLeft.y) / 2.0), // Center is the midpoint.
    width(upRight.x - lowLeft.x),          // Width is the difference in x-coordinates.
    height(upRight.y - lowLeft.y),         // Height is the difference in y-coordinates.
    // Initialize boundary lines using the calculated corner points.
    topLine(upperLeft, upperRight),
    bottomLine(lowerLeft, lowerRight),
    leftLine(lowerLeft, upperLeft),
    rightLine(lowerRight, upperRight)
{
    // Optional: Add validation to ensure width and height are non-negative.
    // If (width < 0 || height < 0) { /* handle error or swap points if out of order */ }
    // The 'realArea' member is not explicitly initialized here, suggesting it might be calculated by the area() method.
}

// Moves the entire rectangle by translating all its defining points (corners, center)
// and then re-initializing its boundary lines.
void Rectangle::move(double dx, double dy)
{
    // Move each individual corner point and the center.
    center.move(dx, dy);
    upperRight.move(dx, dy);
    upperLeft.move(dx, dy);
    lowerRight.move(dx, dy);
    lowerLeft.move(dx, dy);

    // Re-initialize the boundary lines to reflect the new positions of the points.
    // This is necessary because Line members store copies of Points, not references.
    topLine = Line(upperLeft, upperRight);
    bottomLine = Line(lowerLeft, lowerRight);
    leftLine = Line(lowerLeft, upperLeft);
    rightLine = Line(lowerRight, upperRight);
}

// Calculates and returns the area of the rectangle.
double Rectangle::area()
{
    return width * height;
}

// Calculates and returns the maximum X-coordinate among all four corners of the rectangle.
// Uses std::max with an initializer list for conciseness (C++11 and later).
double Rectangle::maxX()
{
    return std::max({ lowerLeft.x, upperLeft.x, lowerRight.x, upperRight.x });
}

// Calculates and returns the maximum Y-coordinate among all four corners of the rectangle.
double Rectangle::maxY()
{
    return std::max({ lowerLeft.y, upperLeft.y, lowerRight.y, upperRight.y });
}

// Calculates and returns the minimum X-coordinate among all four corners of the rectangle.
double Rectangle::minX()
{
    return std::min({ lowerLeft.x, upperLeft.x, lowerRight.x, upperRight.x });
}

// Calculates and returns the minimum Y-coordinate among all four corners of the rectangle.
double Rectangle::minY()
{
    return std::min({ lowerLeft.y, upperLeft.y, lowerRight.y, upperRight.y });
}

// Prints the rectangle's parameters (center, width, height, and all four corner coordinates) to the console.
void Rectangle::toString(void)
{
    std::cout << "--------------------------------------------------\n";
    std::cout << "Rectangle parameters:\n";
    std::cout << "Center/  X=" << center.x << " Y=" << center.y << "\n";
    std::cout << "Width =" << width << " Height =" << height << "\n";
    std::cout << "Upper Right/ X=" << upperRight.x << " Y=" << upperRight.y << "\n";
    std::cout << "Upper Left/  X=" << upperLeft.x << " Y=" << upperLeft.y << "\n";
    std::cout << "Lower Right/ X=" << lowerRight.x << " Y=" << lowerRight.y << "\n";
    std::cout << "Lower Left/  X=" << lowerLeft.x << " Y=" << lowerLeft.y << "\n";
    std::cout << "Area =" << area() << "\n"; // Calls the area() method to get the current area.
    std::cout << "--------------------------------------------------\n";
}

// --- SlopedRectangle Class Implementation ---

// Constructor for SlopedRectangle: Initializes a rectangle with a given slope (rotation).
// It takes the conceptual lower-left and upper-right points *before* rotation, and the slope.
SlopedRectangle::SlopedRectangle(Point dLeft, Point uRight, double Slope)
// First, call the base Rectangle constructor with the un-rotated defining points.
    : Rectangle(dLeft, uRight), slope(Slope)
{
    // After the base Rectangle constructor has set up its axis-aligned properties,
    // we recalculate the corners by rotating them around the rectangle's center.
    // The 'center', 'width', and 'height' members here are from the base Rectangle class,
    // correctly initialized based on the un-rotated dLeft and uRight.
    upperRight = Point::transform(center.x + (width / 2.0), center.y + (height / 2.0), center.x, center.y, Slope);
    upperLeft = Point::transform(center.x - (width / 2.0), center.y + (height / 2.0), center.x, center.y, Slope);
    lowerRight = Point::transform(center.x + (width / 2.0), center.y - (height / 2.0), center.x, center.y, Slope);
    lowerLeft = Point::transform(center.x - (width / 2.0), center.y - (height / 2.0), center.x, center.y, Slope);

    // Note: The 'Line' members inherited from Rectangle (topLine, bottomLine, etc.)
    // still represent the axis-aligned lines. If sloped lines are needed, they
    // should either be members of SlopedRectangle or be calculated on demand.
}

// Constructor for SlopedRectangle: Initializes a sloped rectangle using its center,
// un-rotated width and height, and the slope.
SlopedRectangle::SlopedRectangle(Point Center, double Width, double Height, double Slope)
// First, call the base Rectangle constructor by calculating its un-rotated lower-left
// and upper-right points from the provided center, width, and height.
    : Rectangle(Point(Center.x - Width / 2.0, Center.y - Height / 2.0), Point(Center.x + Width / 2.0, Center.y + Height / 2.0)),
    slope(Slope)
{
    // The base Rectangle constructor has already set 'center', 'width', and 'height'
    // based on the provided parameters. Now, rotate the corner points around this center.
    upperRight = Point::transform(center.x + (width / 2.0), center.y + (height / 2.0), center.x, center.y, Slope);
    upperLeft = Point::transform(center.x - (width / 2.0), center.y + (height / 2.0), center.x, center.y, Slope);
    lowerRight = Point::transform(center.x + (width / 2.0), center.y - (height / 2.0), center.x, center.y, Slope);
    lowerLeft = Point::transform(center.x - (width / 2.0), center.y - (height / 2.0), center.x, center.y, Slope);
}


// Prints the sloped rectangle's parameters (center, width, height, slope in radians and degrees)
// and the coordinates of its transformed (rotated) corner points to the console.
void SlopedRectangle::toString(void)
{
    std::cout << "\n";
    std::cout << "--------------------------------------------------\n";
    std::cout << "Rectangle With Slope Parameters:\n";
    std::cout << "Center/  X=" << center.x << " Y=" << center.y << "\n";
    std::cout << "Width =" << width << "\n";
    std::cout << "Height =" << height << "\n";
    std::cout << "Slope = " << slope << " radians (" << slope * 180.0 / M_PI << " degrees)\n"; // Converts radians to degrees.
    std::cout << "Upper Right/  X=" << upperRight.x << " Y=" << upperRight.y << "\n";
    std::cout << "Upper Left/  X=" << upperLeft.x << " Y=" << upperLeft.y << "\n";
    std::cout << "Lower Right/  X=" << lowerRight.x << " Y=" << lowerRight.y << "\n";
    std::cout << "Lower Left/  X=" << lowerLeft.x << " Y=" << lowerLeft.y << "\n";
    std::cout << "Area =" << area() << "\n"; // Calls the base class's area() method, which correctly calculates the area (rotation doesn't change area).
    std::cout << "--------------------------------------------------\n";
}