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

#pragma once

#include <stdio.h>      // Standard input/output library (for C-style file operations if used).
#include <chrono>       // For time-related functionalities, useful for benchmarking or timestamping.
#include <iostream>     // Standard C++ input/output stream library (e.g., std::cout, std::cin).
#include <fstream>      // File stream operations (e.g., std::ifstream, std::ofstream).
#include <string>       // String manipulation.
#include <queue>        // Standard queue container.

using namespace std; // Using the standard namespace to avoid prefixing std::

// --- Structures for 2D Coordinates ---

// Structure to store integer 2D matrix cell coordinates.
struct point2d
{
	int x; // X-coordinate.
	int y; // Y-coordinate.
	point2d(int a, int b); // Constructor with x and y values.
	point2d(void);         // Default constructor.
};

// Structure to store double-precision 2D coordinates.
struct doublepoint2d
{
	double x; // X-coordinate.
	double y; // Y-coordinate.
	doublepoint2d(double a, double b); // Constructor with x and y values.
	doublepoint2d(void);           // Default constructor.
};

// Structure to store short integer 2D coordinates, possibly for memory efficiency with smaller grids.
struct point2dShort
{
	short int x; // X-coordinate.
	short int y; // Y-coordinate.
};

// --- Cluster Related Structures ---

// Structure representing a single cluster, containing various properties.
struct Cluster
{
	double radius;      // Radius of the cluster.
	doublepoint2d center; // Center coordinates of the cluster (double precision).
	point2d start;      // Starting point (e.g., seed point) of the cluster (integer coordinates).
	size_t totalPoints; // Total number of points (cells) belonging to this cluster.
	double inertia;     // Inertia of the cluster, likely a measure of its mass distribution.

	// Cluster(double a); // Possible constructor, currently commented out.
	// Cluster(double a, double b, double c); // Possible constructor, currently commented out.
	Cluster(void);      // Default constructor.
};


// Structure to hold statistical data for clusters, typically aggregated results.
struct clusterStatistics
{
	double radius;      // Statistical radius (e.g., mean, average).
	double inertia;     // Statistical inertia.
	size_t totalPoints; // Total points across a group of clusters or averaged.
};

/*
// --- Commented-out Class Definitions (Likely for Future or Previous Use) ---

// Class definition for a single Cluster, more elaborate than the struct above.
// It seems to be a more detailed representation with an ID, total points, center,
// starting point, and properties for mean radius calculations.
class Cluster {

public:

	int id;          // Unique identifier for the cluster.
	int totalPoints; // Total number of points in the cluster.
	doublepoint2d center; // Center of the cluster.
	point2d start;   // Starting point of the cluster.
	double meanRadius = 0; // Mean radius of points within the cluster.
	double sumRi_mi = 0;   // Sum for weighted radius calculation (Ri * mi).


	// Method to get the total weighted mean radius.
	double getMeanRadius(void) 	{return (meanRadius * totalPoints);};


	Cluster(int startX, int startY); // Constructor with starting X, Y.


	//Cluster(); // Default constructor.

	Cluster(int startX, int startY, double centerX, double centerY); // Constructor with start and center.

	Cluster(point2d istart, doublepoint2d icenter); // Constructor with point2d and doublepoint2d for start and center.


	~Cluster(); // Destructor.
};


// Class definition for MeanCluster, likely intended for managing and calculating
// statistics across multiple clusters or samples of radii.
class MeanCluster
{
public:
	double getMeanWeightedRadius(void); // Calculates mean weighted radius.
	double getMeanRadius(void);         // Calculates mean radius.
	double getMeanCluster(void);        // Calculates mean cluster property (general).
	double getMaxRadius(void);          // Gets the maximum radius encountered.
	int total;                          // Total count.
	double meanClusters;                // Mean of clusters.
	queue<radiusTotals> SampleMeanRadius; // Queue to store samples of radius totals.
	MeanCluster();                      // Default constructor.
	~MeanCluster();                     // Destructor.
	double meanRadius;                  // Member to store mean radius.
	double maxRadius;                   // Member to store max radius.
	double meanWeightedRadius;          // Member to store mean weighted radius.
	void addMeanRadius(radiusTotals cRadiusTotals); // Method to add radius totals to the sample.


private:
*/