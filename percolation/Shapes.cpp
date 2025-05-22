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

#include <iostream>   // For std::cout
#include "Shapes.h"   // Include the header file for class definitions
#include <algorithm>  // For std::min, std::max, std::swap
#include <cmath>      // For std::cos, std::sin, std::sqrt, std::round

// Removed 'using namespace std;' from .cpp to avoid polluting the global namespace.
// Prefer explicit std:: or scoped using declarations.

// Initialize integer Point
iPoint::iPoint(int ix, int iy) : x(ix), y(iy)
{
}

// Range of integers
Range::Range(iPoint f, iPoint t) : from(f.x, f.y), to(t.x, t.y)
{
    // Ensure 'from' represents the lower-left corner and 'to' the upper-right.
    // Swap x-coordinates if from.x > to.x
    if (from.x > to.x)
    {
        std::swap(from.x, to.x); // Use std::swap for clarity and safety
    }
    // Swap y-coordinates if from.y > to.y
    if (from.y > to.y)
    {
        std::swap(from.y, to.y); // Use std::swap
    }
}

// Point initialization
Point::Point(double xPos, double yPos) : x(xPos), y(yPos)
{
}

/*
Transformation of coordinates
Point original point
Point transformCenter Center of transformation
x' = x*cos(theta) + y*sin(theta)
y' = -x*sin(theta) + y*cos(theta)
*/

// General Case: Transforms an original point around a transformCenter with pre-calculated cos/sin values.
Point Point::transform(Point original, Point transformCenter, double theta, double cosTheta, double sinTheta)
{
    // Translate point so transformCenter becomes the origin
    double translated_x = original.x - transformCenter.x;
    double translated_y = original.y - transformCenter.y;

    // Apply rotation
    double rotated_x = translated_x * cosTheta + translated_y * sinTheta;
    double rotated_y = -translated_x * sinTheta + translated_y * cosTheta;

    // Translate back to original coordinate system
    Point result(rotated_x + transformCenter.x, rotated_y + transformCenter.y);
    return result;
}

// General Case: Transforms an original point around a transformCenter, calculating cos/sin internally.
Point Point::transform(Point original, Point transformCenter, double theta)
{
    // If no rotation is needed, return original point (optimization)
    if (theta == 0.0) // Compare double to 0.0 directly for optimization, but careful with floating point precision
        return original;

    double cosTheta = std::cos(theta);
    double sinTheta = std::sin(theta);

    // Reuse the overloaded transform function with pre-calculated cos/sin
    return transform(original, transformCenter, theta, cosTheta, sinTheta);
}

// Rotate around (0,0) origin
Point Point::transform(Point original, double theta)
{
    Point transformCenter(0.0, 0.0); // Origin as transformation center
    return Point::transform(original, transformCenter, theta);
}

// General Case with double coordinates: Transforms a point given its coordinates, transform center coordinates, angle, cos, and sin.
Point Point::transform(double x, double y, double transformCenterX, double transformCenterY, double theta, double cosTheta, double sinTheta)
{
    Point original(x, y);
    Point transformCenter(transformCenterX, transformCenterY);
    // Reuse the transform function that takes Point objects
    return Point::transform(original, transformCenter, theta, cosTheta, sinTheta);
}

// General Case with double coordinates: Transforms a point given its coordinates, transform center coordinates, and angle.
Point Point::transform(double x, double y, double transformCenterX, double transformCenterY, double theta)
{
    Point original(x, y);
    Point transformCenter(transformCenterX, transformCenterY);
    // Reuse the transform function that takes Point objects
    return transform(original, transformCenter, theta);
}

// Move by dx dy
void Point::move(double dx, double dy)
{
    x += dx;
    y += dy;
}

// Round Coordinates to iPoint
iPoint Point::round(void)
{
    // Use std::round for proper rounding to nearest integer
    return iPoint(static_cast<int>(std::round(x)), static_cast<int>(std::round(y)));
}

// Initialize line
Line::Line(Point start, Point end) : start(start), end(end)
{
}

// Initialize Ellipse
Ellipse::Ellipse(Point c, double ia, double ib, double iSlope) : center(c), a(ia), b(ib), slope(iSlope)
{
    // Optional: Add validation for a, b (e.g., must be positive)
    // if (a <= 0 || b <= 0) { /* handle error */ }
    // realArea = M_PI * a * b; // Calculate realArea here if it's meant to be stored
}

// Area of ellipse A=ð*a*b
double Ellipse::area()
{
    return M_PI * a * b;
}

// toString method for Ellipse
void Ellipse::toString(void)
{
    std::cout << "\n";
    std::cout << "--------------------------------------------------\n";
    std::cout << "Ellipse Parameters:\n";
    std::cout << "Center/  X=" << center.x << " Y=" << center.y << "\n";
    std::cout << "a =" << a << " b =" << b << "\n";
    std::cout << "Slope =" << slope << " radians (" << slope * 180.0 / M_PI << " degrees)\n";
    std::cout << "Area =" << area() << "\n"; // Display calculated area
    std::cout << "--------------------------------------------------\n";
}

// Move ellipse by dx & dy
void Ellipse::move(double dx, double dy)
{
    center.move(dx, dy); // Reuse Point::move
}

// Calculate max X of ellipse's bounding box
double Ellipse::maxX()
{
    // General formula for the maximum extent of a rotated ellipse
    double cos_s = std::cos(slope);
    double sin_s = std::sin(slope);
    return center.x + std::sqrt(a * a * cos_s * cos_s + b * b * sin_s * sin_s);
}

// Calculate max Y of ellipse's bounding box
double Ellipse::maxY()
{
    // General formula for the maximum extent of a rotated ellipse
    double cos_s = std::cos(slope);
    double sin_s = std::sin(slope);
    return center.y + std::sqrt(a * a * sin_s * sin_s + b * b * cos_s * cos_s);
}

// Calculate min X of ellipse's bounding box
double Ellipse::minX()
{
    // General formula for the minimum extent of a rotated ellipse
    double cos_s = std::cos(slope);
    double sin_s = std::sin(slope);
    return center.x - std::sqrt(a * a * cos_s * cos_s + b * b * sin_s * sin_s);
}

// Calculate min Y of ellipse's bounding box
double Ellipse::minY()
{
    // General formula for the minimum extent of a rotated ellipse
    double cos_s = std::cos(slope);
    double sin_s = std::sin(slope);
    return center.y - std::sqrt(a * a * sin_s * sin_s + b * b * cos_s * cos_s);
}

// Rectangle class, defined by lower left and upper right points
// Initialize
Rectangle::Rectangle(Point lowLeft, Point upRight)
    : lowerLeft(lowLeft), upperRight(upRight),
    upperLeft(lowLeft.x, upRight.y), // More direct initialization
    lowerRight(upRight.x, lowLeft.y), // More direct initialization
    center((upRight.x + lowLeft.x) / 2.0, (upRight.y + lowLeft.y) / 2.0), // Use 2.0 for double division
    width(upRight.x - lowLeft.x),
    height(upRight.y - lowLeft.y),
    // Initialize lines after all points are set
    topLine(upperLeft, upperRight),
    bottomLine(lowerLeft, lowerRight),
    leftLine(lowerLeft, upperLeft),
    rightLine(lowerRight, upperRight)
{
    // Optional: Add validation for width, height (e.g., must be positive)
    // if (width < 0 || height < 0) { /* handle error, potentially swap points */ }
    // realArea = width * height; // Calculate realArea here if it's meant to be stored
}

// Move Rectangle
void Rectangle::move(double dx, double dy)
{
    // Move all corners and center. Lines will implicitly update if they store references/pointers
    // or are re-initialized, but since they hold copies, we need to move them too.
    // A better design might make lines depend on points, or just calculate them on the fly.
    // Moving individual points:
    center.move(dx, dy);
    upperRight.move(dx, dy);
    upperLeft.move(dx, dy);
    lowerRight.move(dx, dy);
    lowerLeft.move(dx, dy);

    // Re-initialize lines based on moved points. This is repetitive.
    // Consider making Line members `const` and re-calculating in a helper `updateLines()` method
    // or having Line take Point references/pointers. For now, adhering to existing structure.
    topLine = Line(upperLeft, upperRight);
    bottomLine = Line(lowerLeft, lowerRight);
    leftLine = Line(lowerLeft, upperLeft);
    rightLine = Line(lowerRight, upperRight);
}

// Area of rectangle
double Rectangle::area()
{
    return width * height;
}

// Max - Min of X and Y
// Get maximum - minimum of all corners
// max X
double Rectangle::maxX()
{
    // Using std::max with an initializer list (C++11 and later) for conciseness
    return std::max({ lowerLeft.x, upperLeft.x, lowerRight.x, upperRight.x });
}

// maxY
double Rectangle::maxY()
{
    return std::max({ lowerLeft.y, upperLeft.y, lowerRight.y, upperRight.y });
}

// minX
double Rectangle::minX()
{
    return std::min({ lowerLeft.x, upperLeft.x, lowerRight.x, upperRight.x });
}

// minY
double Rectangle::minY()
{
    return std::min({ lowerLeft.y, upperLeft.y, lowerRight.y, upperRight.y });
}

// toString method for Rectangle
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
    std::cout << "Area =" << area() << "\n"; // Display calculated area
    std::cout << "--------------------------------------------------\n";
}

// initialize SlopedRectangle
// The base Rectangle constructor is called, then corner points are rotated around the center.
SlopedRectangle::SlopedRectangle(Point dLeft, Point uRight, double Slope)
    : Rectangle(dLeft, uRight), slope(Slope)
{
    // Recalculate corners by rotating the axis-aligned corners around the center.
    // The original constructor of Rectangle would set corners based on dLeft, uRight
    // as if it were an axis-aligned rectangle. Now we rotate them.
    upperRight = Point::transform(center.x + (width / 2.0), center.y + (height / 2.0), center.x, center.y, Slope);
    upperLeft = Point::transform(center.x - (width / 2.0), center.y + (height / 2.0), center.x, center.y, Slope);
    lowerRight = Point::transform(center.x + (width / 2.0), center.y - (height / 2.0), center.x, center.y, Slope);
    lowerLeft = Point::transform(center.x - (width / 2.0), center.y - (height / 2.0), center.x, center.y, Slope);

    // The base Rectangle::move() also updates lines implicitly.
    // For SlopedRectangle, lines would need to be re-calculated after point rotation.
    // This current design doesn't re-expose lines in SlopedRectangle, so they remain
    // the axis-aligned ones from the Rectangle base, which might be incorrect for a sloped rectangle.
    // If lines are needed for sloped rectangles, they should be members of SlopedRectangle
    // or recalculated upon creation/movement.
}

SlopedRectangle::SlopedRectangle(Point Center, double Width, double Height, double Slope)
    : Rectangle(Point(Center.x - Width / 2.0, Center.y - Height / 2.0), Point(Center.x + Width / 2.0, Center.y + Height / 2.0)),
    slope(Slope)
{
    // The base Rectangle constructor sets center, width, height, and initial corners.
    // Now rotate these corners to reflect the slope.
    // Note: 'center' here is the base class's center, which is correct.
    // 'width' and 'height' are also base class members.
    upperRight = Point::transform(center.x + (width / 2.0), center.y + (height / 2.0), center.x, center.y, Slope);
    upperLeft = Point::transform(center.x - (width / 2.0), center.y + (height / 2.0), center.x, center.y, Slope);
    lowerRight = Point::transform(center.x + (width / 2.0), center.y - (height / 2.0), center.x, center.y, Slope);
    lowerLeft = Point::transform(center.x - (width / 2.0), center.y - (height / 2.0), center.x, center.y, Slope);
}


// toString method for SlopedRectangle
void SlopedRectangle::toString(void)
{
    std::cout << "\n";
    std::cout << "--------------------------------------------------\n";
    std::cout << "Rectangle With Slope Parameters:\n";
    std::cout << "Center/  X=" << center.x << " Y=" << center.y << "\n";
    std::cout << "Width =" << width << "\n";
    std::cout << "Height =" << height << "\n";
    std::cout << "Slope = " << slope << " radians (" << slope * 180.0 / M_PI << " degrees)\n"; // Display in degrees too
    std::cout << "Upper Right/  X=" << upperRight.x << " Y=" << upperRight.y << "\n";
    std::cout << "Upper Left/  X=" << upperLeft.x << " Y=" << upperLeft.y << "\n";
    std::cout << "Lower Right/  X=" << lowerRight.x << " Y=" << lowerRight.y << "\n";
    std::cout << "Lower Left/  X=" << lowerLeft.x << " Y=" << lowerLeft.y << "\n";
    std::cout << "Area =" << area() << "\n"; // Display calculated area (base class area is valid for sloped rect)
    std::cout << "--------------------------------------------------\n";
}