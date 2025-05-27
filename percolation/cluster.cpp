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
1. E. Lambrou and L. N. Gergidis, “A computational method for calculating the electrical and thermal properties of random composite” ,
Physica A: Statistical Mechanics and its Applications, Volume 642, 2024, 129760, ISSN 0378-4371,
https://doi.org/10.1016/j.physa.2024.129760
2. E. Lambrou and L. N. Gergidis, “A particle digitization-based computational method for continuum percolation,” Physica A: Statistical Mechanics
and its Applications, vol. 590, p. 126738, 2022

if you use this programm and write a paper or report please cite above papers

*/

#include "cluster.h" // Includes the header file that defines the 'point2d', 'doublepoint2d', and 'Cluster' structures.

// --- point2d Struct Implementations ---

// Constructor for 'point2d': Initializes an integer 2D point with provided 'a' for x and 'b' for y.
point2d::point2d(int a, int b)
{
	x = a; // Assigns the value of 'a' to the 'x' coordinate.
	y = b; // Assigns the value of 'b' to the 'y' coordinate.
}

// Default constructor for 'point2d': Initializes an integer 2D point with both x and y set to 0.
// This uses a member initializer list for efficiency and clarity.
point2d::point2d(void) :x(0), y(0)
{

}

// --- doublepoint2d Struct Implementations ---

// Default constructor for 'doublepoint2d': Initializes a double-precision 2D point with both x and y set to 0.0.
doublepoint2d::doublepoint2d(void)
{
	x = 0.0; // Assigns 0.0 to the 'x' coordinate.
	y = 0.0; // Assigns 0.0 to the 'y' coordinate.
}

// Constructor for 'doublepoint2d': Initializes a double-precision 2D point with provided 'a' for x and 'b' for y.
doublepoint2d::doublepoint2d(double a, double b)
{
	x = a; // Assigns the value of 'a' to the 'x' coordinate.
	y = b; // Assigns the value of 'b' to the 'y' coordinate.
}

// --- Cluster Struct Implementation ---

// Default constructor for 'Cluster': Initializes the members of a Cluster structure.
// 'radius' and 'inertia' are set to 0.0 (double), and 'totalPoints' to 0 (size_t).
Cluster::Cluster(void)
{
	radius = 0.0;      // Initializes the cluster's radius to zero.
	totalPoints = 0;   // Initializes the count of points in the cluster to zero.
	inertia = 0.0;     // Initializes the cluster's inertia to zero.
}