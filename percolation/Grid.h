/*
This file is part of Electric,Thermal, Mechanical Properties
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

#pragma once // Ensures this header file is included only once in a compilation unit.

#include <assert.h> // For assert() macro, useful for debugging by checking assumptions.
#include <iostream> // For standard input/output operations (e.g., std::cout, std::cerr).
#include <stdlib.h> // For general utilities, like malloc, free, exit.
#include <stdio.h>  // For standard input/output functions (e.g., printf, scanf).
#include <time.h>   // For time-related functions (e.g., time for seeding random numbers).
#include <chrono>   // For high-resolution time measurements (e.g., for performance timing).
#include <queue>    // For std::queue, a container adapter that provides FIFO functionality.
#include <stack>    // For std::stack, a container adapter that provides LIFO functionality.
#include <list>     // For std::list, a doubly-linked list container.
#include "Shapes.h" // Custom header for shape definitions (e.g., point2d, Rectangle, iPoint, Range).
#include "image.h"  // Custom header for image manipulation functionalities.
#include "general.h" // Custom header for general utility functions (e.g., delay, NowToString) and enums.
#include "cluster.h" // Custom header for cluster-related definitions and structures (e.g., clusterStatistics).
#include <omp.h>    // For OpenMP directives, enabling parallel programming.
#include <vector>   // For std::vector, a dynamic array.

#define maxDimension 30000 // Defines a maximum dimension for grid-related arrays, potentially for pre-allocation or limits.

// Structure representing a node in a grid with state information,
// typically used for pathfinding or property calculation.
struct nodeWithState
{
	point2d pt;             // The coordinates (x, y) of a cell in the grid.
	point2d previous;       // The coordinates of the previous cell in a path, useful for path reconstruction.
	int dist;               // The cell's distance (number of steps) from a source point.
	float resistance;       // Electrical resistance property of the cell from the source.
	float thermalResistance; // Thermal resistance property of the cell from the source.
	float YoungModulus;     // Young's Modulus (mechanical property) of the cell.
	float PoissonRatio;     // Poisson's Ratio (mechanical property) of the cell.
	char state;             // The state of the cell, potentially from CellState enum (e.g., '1' for percolating).
};

// Structure representing a small node in a queue, typically for basic pathfinding
// where only coordinates, previous point, and distance are needed.
struct smallQueueNode
{
	point2d pt;         // The coordinates (x, y) of a cell.
	point2d previous;   // The coordinates of the previous cell.
	int dist = 0;       // The cell's distance from the source, initialized to 0.
};

// Structure representing a queue node with additional physical properties,
// inheriting from `smallQueueNode`. Used when detailed property calculations are needed.
struct queueNode : smallQueueNode {
	// point2d pt;         // Inherited from smallQueueNode
	// point2d previous;   // Inherited from smallQueueNode
	// int dist;           // Inherited from smallQueueNode

	float resistance = 0.0;        // Electrical resistance from the source, initialized to 0.0.
	float thermalResistance = 0.0; // Thermal resistance from the source, initialized to 0.0.
	float YoungModulus = 0.0;      // Young's Modulus, initialized to 0.0.
	float PoissonRatio = 0.0;      // Poisson's Ratio, initialized to 0.0.
};

// Definition of the Grid class, which manages the 2D grid structure
// and performs various simulation operations related to percolation,
// cluster analysis, and property estimation.
class Grid {
	// Private helper methods:
	// Checks if the given coordinates (x, y) are within the valid bounds of the grid.
	bool isValid(int x, int y);
	// Performs a flood-fill algorithm starting from `src` on the grid `mat`.
	// Returns the size of the flooded area.
	int floodFill(char* mat, bool* ivisited, point2d src);

	// Initiates the percolation process within the grid.
	// The return type 'int' might indicate a status code or a calculated value.
	int percolate(void);
	size_t total; // Total number of cells in the grid (width * height).

public:
	std::list<std::list<smallQueueNode>> pathsList; // Stores lists of paths found during percolation or other analyses.
	char* cell;     // Pointer to the 2D grid data representing cell states (e.g., '0', '1', '2').
	char* start;    // Pointer to a grid or region representing the start boundary for percolation.
	char* end;      // Pointer to a grid or region representing the end boundary for percolation.
	unsigned char* cluster; // Pointer to a grid storing cluster IDs or states for visualization/analysis.
	int totalClusters = 0; // Counts the total number of distinct clusters identified.
	int width, height;     // Dimensions of the grid.
	unsigned char* ingadients; // Likely stores gradient information or similar visual data.
	bool* visited;         // Boolean array to keep track of visited cells during traversals (e.g., BFS, DFS).
	bool* clusterVisited;  // Boolean array to keep track of visited cells during cluster identification.

	// Constructor: Initializes a Grid object with specified width and height.
	Grid(int x, int y);

	// Clears the grid, resetting all cell states or visited flags.
	void clear(void);
	//double VerticalYoungModulus[maxDimension]; // Potentially for storing Young's Modulus values for vertical paths.
	std::vector<clusterStatistics> Clusters; // Stores statistics for each identified cluster.

	std::vector<double> cMaxClusterRadius; // Stores maximum cluster radii.

	// Grid manipulation methods:

	// Gets the state of the cell at (line, column).
	char get(int line, int column);
	// Inverts the states of cells in the grid (e.g., '0' to '1', '1' to '0').
	void inverse(void);
	// Counts the total area (number of specific cells) in the grid.
	int countArea();
	// Sets the state of the cell at (line, column) to 'what'.
	void set(int line, int column, char what);
	// Overloaded set function: sets the state of the cell at (x, y) to 'what' (integer representation).
	void set(int x, int y, int what);
	// Sets the value of a specific data array (`data`) at (x, y) to `what`.
	void set(unsigned char* data, int x, int y, unsigned char what);
	// Gets the value from a specific data array (`data`) at (x, y).
	unsigned char get(unsigned char* data, int x, int y);

	// Coordinate transformation and range methods:

	// Rectangle cellCoordinates(int x, int y); // (Commented out) Likely a method to get the bounding box of a cell.
	// Converts real-world coordinates (x, y) to grid (local) integer dimensions.
	iPoint toLocaldimensions(double x, double y);
	// Converts real-world coordinates (x, y) to grid dimensions, rounding up.
	iPoint toLocaldimensionsUp(double x, double y);
	// Converts real-world coordinates (x, y) to grid dimensions, rounding down.
	iPoint toLocaldimensionsDown(double x, double y);
	// Converts a rectangle defined by two points (x1, y1, x2, y2) to a grid `Range`.
	Range fromRectangleToRange(double x1, double y1, double x2, double y2);

	// Shape checking methods:

	// Checks if a sloped rectangle defined by (x, y, dx, dy, slope) is clear (e.g., contains no obstacles).
	bool isClearSlopedRectangle(int x, int y, int dx, int dy, double slope);
	// Checks if an ellipse defined by (x, y, a, b, slope) is clear.
	bool isClearEllipse(int x, int y, int a, int b, double slope);

	// Pathfinding and property calculation methods:

	// Performs a Depth-First Search (DFS) from `src` to `target` on `mat`,
	// updating `distance` and returning a status or path length.
	int DFS(char* mat, bool* ivisited, point2dShort src, point2d* target, int* distance);
	// Counts the number of pixels marked as part of a path in `mat`.
	size_t countPathPixels(char* mat);
	// Restores the grid state after a DFS traversal (potentially unmarking visited cells).
	int DFSRestore(char* mat, bool* ivisited, point2dShort src);
	// Calculates electrical, thermal, and mechanical properties along a given `path`.
	void CalcPropetriesAtPath(char* mat, bool* ivisited, std::list<smallQueueNode> path,
		double* resistance, double* ielectricConductivities,
		double* thermalResistance, double* ithermalConductivities,
		double* YoungModulus, double* iYoungModulus,
		double* PoissonRatio, double* iPoissonRatio);
	// Calculates properties at a single `currentPoint` in the grid.
	void calcPropertiesAtPoint(char* mat, bool* ivisited, queueNode* currentPoint,
		double* ielectricConductivities, double* ithermalConductivities,
		double* iYoungModulus, double* iPoissonRatio);
	// Marks the minimum path found (e.g., by BFS) onto the grid.
	void MarkMinimumPath(char* mat, bool* ivisited, std::stack<smallQueueNode>* clusterStack, std::list<smallQueueNode>* listPath);
	// Draws a single path (`cpath`) onto the grid `mat`. Returns the number of pixels drawn.
	int drawPath(char* mat, bool* ivisited, std::list<smallQueueNode> cpath);
	// Draws all stored paths from `pathsList` onto the grid `mat`. Returns total pixels drawn.
	int drawPaths(char* mat, bool* ivisited);
	// Performs a Breadth-First Search (BFS) from `src` to find paths and calculate properties.
	// Returns a status code or a calculated value.
	int BFS(char* mat, bool* ivisited, point2d src,
		int* distance, double* ielectricConductivities, double* resistance,
		double* ithermalConductivities, double* thermalResistance,
		double* iYoungModulus, double* YoungModulus,
		double* iPoissonRatio, double* PoissonRatio, point2d* finalPoint);

	// Cluster analysis methods:

	// Finds the center of a cluster starting from `src`.
	int clusterCenter(point2d src, doublepoint2d* center, size_t* totalPoints);
	// Calculates the inertia and radius of gyration for a cluster.
	int clusterInertia(char* mat, unsigned char* clustersImage, point2d src, doublepoint2d center, size_t totalPoints, double* inertia, double* radiusOfGyration);
	// Marks and identifies all distinct clusters in the grid. Returns a `Cluster` object.
	Cluster markClusters();
	// Calculates the correlation length based on statistics of identified clusters.
	double findCorrelationLength(std::vector<clusterStatistics> iClusters);

	// Pixel counting methods for shapes (likely for determining overlap or coverage):

	// Counts pixels inside a sloped rectangle that match a given `CellState`. Handles multiple states.
	int countPixelsInsidelopedRectangeMultiple(int x, int y, int dx, int dy, double slope, CellState state);
	// Counts pixels inside an ellipse that match a given `CellState`. Handles multiple states.
	int countPixelsInsideEllipseMultiple(int x, int y, int a, int b, double slope, CellState state);
	// Counts pixels inside a sloped rectangle that match a given `CellState`.
	int countPixelsInsidelopedRectange(int x, int y, int dx, int dy, double slope, CellState state);
	// Counts pixels inside an ellipse that match a given `CellState`.
	int countPixelsInsideEllipse(int x, int y, int a, int b, double slope, CellState state);

	// Percolation simulation methods:

	// Performs the core percolation simulation and measures processing time.
	int percolate(double* proccessTime);
	// Performs percolation and calculates real path lengths and various material properties.
	int percolateWithRealPathLength(double* totalPaths, double* meanLength, double* meanCalculatedLength,
		double* ielectricConductivities, double* ithermalConductivities,
		double* iYoungModulus, double* iPoissonRatio,
		double* meanResistance, double* meanThermalResistance,
		double* meanYoungModulus, double* MeanPoissonRatio);
	// Overloaded version of percolateWithRealPathLength, also returning process time.
	int percolateWithRealPathLength(double* totalpaths, double* meanlength, double* meanRealLength, double* proccessTime,
		double* ielectricConductivities, double* omeanResistance,
		double* ithermalConductivities, double* omeanThermalResistance,
		double* iYoungModulus, double* omeanYoungModulus,
		double* iPoissonRatio, double* omeanPoissonRatio);

	// Display and saving methods:

	// Displays the grid state (e.g., prints to console or visualizes).
	void show() const;
	// Saves the current grid state to a disk file.
	void saveToDisk(char* imageFileName, bool saveAsBMP);
	// Saves the current grid state to a disk file at a specified path.
	void saveToDisk(char* path, char* imageFileName, bool saveAsBMP);

	// Destructor: Frees dynamically allocated memory.
	~Grid();
};