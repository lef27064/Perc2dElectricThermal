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

#include "Grid.h"     // Includes the header file for the Grid class.
#include <cstring>    // For C-style string manipulation functions like `memset`.
#include <string.h>   // Another C-style string header, often redundant with <cstring>.
#include <stdio.h>    // For standard input/output functions like `printf`.
#include <stdlib.h>   // For general utilities like `malloc`, `free`, `exit`, and `rand`.
#include <cmath>      // For mathematical functions like `pow` and `sqrt`.
#include <numeric>    // For `std::accumulate` (though not directly used in the provided functions, often useful with `std::vector`).
#include <algorithm>  // For `std::fill_n`, `std::round`, `std::floor`.

using namespace std; // Brings all names from the `std` namespace into the current scope.


/* These arrays are used to get row and column
// numbers of 4 neighbours of a given cell
//
// As published in paper with left preference, the order is Left, Down, Up, Right.
*/
// The x-coordinate offsets for checking neighbors.
int xNum[] = { -1, 0 ,0,  1 };
// The y-coordinate offsets for checking neighbors.
int yNum[] = { 0,  1 ,-1, 0 };


// Constructor for the Grid class.
// Initializes a grid of specified width `x` and height `y`.
Grid::Grid(int x, int y) : width(x), height(y)
{
	total = (size_t)width * height; // Calculates the total number of cells in the grid.

#pragma omp parallel sections // Directs the compiler to execute the enclosed sections in parallel.
	{
#pragma omp section // Defines a block of code to be executed by one thread.
		{
			// Allocates memory for the `cell` array, which stores the state of each cell.
			start = (char*)malloc(total + 1);
			assert(start); // Asserts that memory allocation was successful.
			cell = start; // `cell` points to the beginning of the allocated memory.
			size_t lastLine = width * (height - 1); // Calculates the starting index of the last row.
			end = start + lastLine; // `end` points to the beginning of the last row.
			// Initializes all cells to `CellState::EMPTY`.
			std::fill_n(start, total, CellState::EMPTY);
		}
#pragma omp section // Defines another block of code for parallel execution.
		{
			// Allocates memory for the `visited` array, used in pathfinding algorithms.
			visited = (bool*)malloc(total + 1);
			assert(visited); // Asserts successful memory allocation.
			// Initializes all `visited` flags to `false`.
			std::fill_n(visited, total, false);
		}
#pragma omp section // Defines another block of code for parallel execution.
		{
			// Allocates memory for the `ingadients` array (likely stores material types or gradient values).
			ingadients = (unsigned char*)malloc(total + 1);
			assert(ingadients); // Asserts successful memory allocation.
			// Initializes all `ingadients` values to 0.
			std::fill_n(ingadients, total, 0);
		}

#pragma omp section // Defines another block of code for parallel execution.
		{
			// Allocates memory for the `cluster` array (likely stores cluster IDs or visual representation).
			cluster = (unsigned char*)malloc(total + 1);
			assert(cluster); // Asserts successful memory allocation.
			// Initializes all `cluster` values to 0x0.
			std::fill_n(cluster, total, 0x0);
		}
#pragma omp section // Defines another block of code for parallel execution.
		{
			// Allocates memory for the `clusterVisited` array, used during cluster identification.
			clusterVisited = (bool*)malloc(total + 1);
			assert(clusterVisited); // Asserts successful memory allocation.
			// Initializes all `clusterVisited` flags to `false`.
			std::fill_n(clusterVisited, total, false);
		}

	}
}


// `clear` method: Resets all values in the `cell`, `visited`, `ingadients`, `cluster`, and `clusterVisited` arrays.
void Grid::clear(void)
{
#pragma omp parallel sections // Executes the following sections in parallel.
	{
#pragma omp section // Clears the `cell` array to `CellState::EMPTY`.
		std::fill_n(cell, total, CellState::EMPTY);
#pragma omp section // Clears the `visited` array to `false`.
		std::fill_n(visited, total, false);
#pragma omp section // Clears the `ingadients` array to 0.
		std::fill_n(ingadients, total, 0);
#pragma omp section // Clears the `cluster` array to 0.
		std::fill_n(cluster, total, 0);
#pragma omp section // Clears the `clusterVisited` array to 0.
		std::fill_n(clusterVisited, total, 0);
	}

}

// `get` method: Reads the cell state at the specified `x` (column) and `y` (line) coordinates.
char Grid::get(int x, int y)
{
	return (cell[(size_t)y * width + x]); // Returns the character representing the cell's state.
}


// `inverse` method: Inverts the cell states (e.g., `EMPTY` to `HARD`, `HARD` to `EMPTY`).
// This is typically used to create a "Swiss cheese" like microstructure.
void Grid::inverse(void)
{
	for (int y = 0; y < height; y++) // Iterates through each row.
	{
		for (int x = 0; x < width; x++) // Iterates through each column.
		{
			switch (get(x, y)) // Checks the current cell's state.
			{
			case CellState::EMPTY: set(x, y, CellState::HARD); break; // If `EMPTY`, sets to `HARD`.
			case CellState::HARD:  set(x, y, CellState::EMPTY); break; // If `HARD`, sets to `EMPTY`.
			}
		}
	}
}

// `countArea` method: Counts the number of cells marked as `CellState::HARD`.
int Grid::countArea()
{
	int result = 0; // Initializes the count to 0.
	for (int i = 0; i < width; i++) // Iterates through columns.
		for (int j = 0; j < height; j++) // Iterates through rows.
			if (get(i, j) == CellState::HARD) // If the cell is `HARD`, increment the count.
				result++;
	return result; // Returns the total count of `HARD` cells.
}

// `set` method: Sets the state of the cell at `x, y` to `what` (character).
void Grid::set(int x, int y, char what)
{
	cell[(size_t)y * width + x] = what; // Directly sets the cell's state.
}


// `set` method (overloaded): Sets the state of the cell at `x, y` to `what` (integer, cast to char).
void Grid::set(int x, int y, int what)
{
	cell[(size_t)y * width + x] = (char)what; // Casts the integer `what` to a character and sets the cell's state.
}

// `set` method (overloaded): Sets the value in a given `data` array at `x, y` to `what` (unsigned char).
void Grid::set(unsigned char* data, int x, int y, unsigned char what)
{
	data[(size_t)y * width + x] = what; // Sets the value in the provided `data` array.
}

// `get` method (overloaded): Gets the value from a given `data` array at `x, y`.
unsigned char Grid::get(unsigned char* data, int x, int y)
{
	return data[(size_t)y * width + x]; // Returns the unsigned char value from the provided `data` array.
}

// `toLocaldimensions` method: Converts double coordinates to integer grid coordinates by rounding.
iPoint Grid::toLocaldimensions(double x, double y)
{
	return iPoint(int(std::round(x)), int(std::round(y))); // Returns an `iPoint` with rounded integer coordinates.
}

// `toLocaldimensionsDown` method: Converts double coordinates to integer grid coordinates by flooring.
iPoint Grid::toLocaldimensionsDown(double x, double y)
{
	return iPoint(int(std::floor(x)), int(std::floor(y))); // Returns an `iPoint` with floored integer coordinates.
}

// `toLocaldimensionsUp` method: Converts double coordinates to integer grid coordinates by rounding up.
iPoint Grid::toLocaldimensionsUp(double x, double y)
{
	return iPoint(int(std::round(x)), int(std::round(y))); // Returns an `iPoint` with rounded integer coordinates (effectively rounding up for positive numbers due to `round`).
}

// `fromRectangleToRange` method: Converts a continuous rectangle defined by (x1, y1) and (x2, y2)
// into a grid `Range` using `toLocaldimensionsDown` and `toLocaldimensionsUp`.
Range Grid::fromRectangleToRange(double x1, double y1, double x2, double y2)
{
	// Creates a `Range` object from the lower-left to the upper-right corner.
	Range result(toLocaldimensionsDown(x1, y1), toLocaldimensionsUp(x2, y2));
	return result; // Returns the calculated `Range`.
}


// `isClearSlopedRectangle` method: Checks if a sloped rectangle contains any `HARD` pixels.
// The rectangle is defined by its center (x, y), dimensions (dx, dy), and `slope` (angle).
bool Grid::isClearSlopedRectangle(int x, int y, int dx, int dy, double slope)
{
	// Calculates the transformed upper-right and lower-left points of the rectangle.
	Point upperRight = Point((Point::transform(x + int(dx / 2), y + int(dy / 2), x, y, slope)));
	Point lowerLeft = Point((Point::transform(x - int(dx / 2), y - int(dy / 2), x, y, slope)));

	// Iterates through the bounding box of the rotated rectangle.
	for (int i = int(std::floor(lowerLeft.x)); i <= int(std::round(upperRight.x)); i++)
	{
		for (int j = int(std::floor(lowerLeft.y)); j <= int(std::round(upperRight.y)); j++)
		{
			// Transforms the current grid cell's center back to the original coordinate system.
			Point rotated = Point::transform(i + 0.5, j + 0.5, x, y, -slope);
			// Checks if the rotated point is within the bounds of the original (axis-aligned) rectangle.
			if ((rotated.x <= (x + int(dx / 2))) && (rotated.x >= (x - int(dx / 2))) && ((rotated.y <= (y + int(dy / 2))) && (rotated.y >= (y - int(dy / 2)))))
			{
				// If the cell is `HARD`, the rectangle is not clear.
				if (get(i, j) == CellState::HARD)
					return false;
			}
		}
	}
	return true; // If no `HARD` pixels are found, the rectangle is clear.
}

// `isClearEllipse` method: Checks if an ellipse contains any `HARD` pixels.
// The ellipse is defined by its center (x, y), semi-axes (a, b), and `slope` (angle).
bool Grid::isClearEllipse(int x, int y, int a, int b, double slope)
{
	Ellipse iEllipse(Point(x, y), a, b, slope); // Creates an Ellipse object.
	Point upperRight = Point(iEllipse.maxX(), iEllipse.maxY()); // Gets the maximum X and Y coordinates of the ellipse's bounding box.
	Point lowerLeft = Point(iEllipse.minX(), iEllipse.minY()); // Gets the minimum X and Y coordinates.

	// Iterates through the bounding box of the rotated ellipse.
	for (int i = int(std::floor(lowerLeft.x)); i <= int(std::round(upperRight.x)); i++)
	{
		for (int j = int(std::floor(lowerLeft.y)); j <= int(std::round(upperRight.y)); j++)
		{
			// Transforms the current grid cell's center back to the original coordinate system.
			Point rotated = Point::transform(i + 0.5, j + 0.5, x, y, -slope);
			// Checks if the rotated point is inside the ellipse equation.
			if (((rotated.x - x) * (rotated.x - x) / (a * a)) + ((rotated.y - y) * (rotated.y - y) / (b * b)) <= 1)
			{
				// If the cell is `HARD`, the ellipse is not clear.
				if (get(i, j) == CellState::HARD)
					return false;
			}
		}
	}
	return true; // If no `HARD` pixels are found, the ellipse is clear.
}

// `countPixelsInsidelopedRectangeMultiple` method: Counts pixels within a sloped rectangle that
// have a state greater than or equal to `state`.
int Grid::countPixelsInsidelopedRectangeMultiple(int x, int y, int dx, int dy, double slope, CellState state)
{
	int result = 0; // Initializes pixel count.
	SlopedRectangle iSlopedRect(Point(x, y), dx, dy, slope); // Creates a SlopedRectangle object.
	// Defines a `Range` for the bounding box of the sloped rectangle, with a small buffer.
	Range region(toLocaldimensionsDown(iSlopedRect.minX() - 1, iSlopedRect.minY() - 1), toLocaldimensionsUp(iSlopedRect.maxX() + 1, iSlopedRect.maxY() + 1));

	// Adjusts the region to stay within grid boundaries.
	if (region.from.x < 0) region.from.x = 0;
	if (region.from.y < 0) region.from.y = 0;
	if (region.to.x > width) region.to.x = width;
	if (region.to.y > height) region.to.y = height;

	// Iterates through the cells within the adjusted bounding box.
	for (int i = region.from.x; i < region.to.x; i++)
	{
		for (int j = region.from.y; j < region.to.y; j++)
		{
			// Transforms the current cell's center back to the rectangle's local coordinate system.
			Point rotated = Point::transform(i + 0.5, j + 0.5, x, y, -slope);
			// Checks if the rotated point is within the rectangle's dimensions.
			if ((1.0 * rotated.x <= 1.0 * (x + (dx / 2.0))) && (1.0 * rotated.x >= 1.0 * (x - (dx / 2.0))) && ((1.0 * rotated.y <= 1.0 * (y + (dy / 2.0))) && (1.0 * rotated.y >= 1.0 * (y - (dy / 2.0)))))
			{
				if (get(i, j) >= state) // If the cell's state meets the condition, increment result.
					result++;
			}
		}
	}
	return result; // Returns the total count of qualifying pixels.
}

// `countPixelsInsideEllipseMultiple` method: Counts pixels within an ellipse that
// have a state greater than or equal to `state`.
int Grid::countPixelsInsideEllipseMultiple(int x, int y, int a, int b, double slope, CellState state)
{
	int result = 0; // Initializes pixel count.
	Ellipse iEllipse(Point(x, y), a, b, slope); // Creates an Ellipse object.
	// Defines a `Range` for the bounding box of the ellipse, with a small buffer.
	Range region(toLocaldimensionsDown(iEllipse.minX() - 1, iEllipse.minY() - 1), toLocaldimensionsUp(iEllipse.maxX() + 1, iEllipse.maxY() + 1));

	// Adjusts the region to stay within grid boundaries.
	if (region.from.x < 0) region.from.x = 0;
	if (region.from.y < 0) region.from.y = 0;
	if (region.to.x > width) region.to.x = width;
	if (region.to.y > height) region.to.y = height;

	// Iterates through the cells within the adjusted bounding box.
	for (int i = region.from.x; i < region.to.x; i++)
	{
		for (int j = region.from.y; j < region.to.y; j++)
		{
			// Transforms the current cell's center back to the ellipse's local coordinate system.
			Point rotated = Point::transform(i + 0.5, j + 0.5, x, y, -slope);
			// Calculates the squared distance from the ellipse center in its own coordinate system.
			double distance = pow(1.0 * (rotated.x - x) / (1.0 * a), 2) + pow(1.0 * (rotated.y - y) / (1.0 * b), 2);
			if (distance <= 1) // If the distance is less than or equal to 1, the point is inside the ellipse.
			{
				if (get(i, j) >= state) // If the cell's state meets the condition, increment result.
					result++;
			}
		}
	}
	return result; // Returns the total count.
}

// `countPixelsInsidelopedRectange` method: Counts pixels within a sloped rectangle that
// have an exact state of `state`.
int Grid::countPixelsInsidelopedRectange(int x, int y, int dx, int dy, double slope, CellState state)
{
	int result = 0; // Initializes pixel count.
	SlopedRectangle iSlopedRect(Point(x, y), dx, dy, slope); // Creates a SlopedRectangle object.
	// Defines a `Range` for the bounding box of the sloped rectangle, with a small buffer.
	Range region(toLocaldimensionsDown(iSlopedRect.minX() - 1, iSlopedRect.minY() - 1), toLocaldimensionsUp(iSlopedRect.maxX() + 1, iSlopedRect.maxY() + 1));

	// Adjusts the region to stay within grid boundaries.
	if (region.from.x < 0) region.from.x = 0;
	if (region.from.y < 0) region.from.y = 0;
	if (region.to.x > width) region.to.x = width;
	if (region.to.y > height) region.to.y = height;

	// Iterates through the cells within the adjusted bounding box.
	for (int i = region.from.x; i < region.to.x; i++)
	{
		for (int j = region.from.y; j < region.to.y; j++)
		{
			// Transforms the current cell's center back to the rectangle's local coordinate system.
			Point rotated = Point::transform(i + 0.5, j + 0.5, x, y, -slope);
			// Checks if the rotated point is within the rectangle's dimensions.
			if ((1.0 * rotated.x <= 1.0 * (x + (dx / 2.0))) && (1.0 * rotated.x >= 1.0 * (x - (dx / 2.0))) && ((1.0 * rotated.y <= 1.0 * (y + (dy / 2.0))) && (1.0 * rotated.y >= 1.0 * (y - (dy / 2.0)))))
			{
				if (get(i, j) == state) // If the cell's state is exactly `state`, increment result.
					result++;
			}
		}
	}
	return result; // Returns the total count.
}

// `countPixelsInsideEllipse` method: Counts pixels within an ellipse that
// have an exact state of `state`.
int Grid::countPixelsInsideEllipse(int x, int y, int a, int b, double slope, CellState state)
{
	int result = 0; // Initializes pixel count.
	Ellipse iEllipse(Point(x, y), a, b, slope); // Creates an Ellipse object.
	// Defines a `Range` for the bounding box of the ellipse, with a small buffer.
	Range region(toLocaldimensionsDown(iEllipse.minX() - 1, iEllipse.minY() - 1), toLocaldimensionsUp(iEllipse.maxX() + 1, iEllipse.maxY() + 1));

	// Adjusts the region to stay within grid boundaries.
	if (region.from.x < 0) region.from.x = 0;
	if (region.from.y < 0) region.from.y = 0;
	if (region.to.x > width) region.to.x = width;
	if (region.to.y > height) region.to.y = height;

	// Iterates through the cells within the adjusted bounding box.
	for (int i = region.from.x; i < region.to.x; i++)
	{
		for (int j = region.from.y; j < region.to.y; j++)
		{
			// Transforms the current cell's center back to the ellipse's local coordinate system.
			Point rotated = Point::transform(i + 0.5, j + 0.5, x, y, -slope);
			// Calculates the squared distance from the ellipse center in its own coordinate system.
			double distance = pow(1.0 * (rotated.x - x) / (1.0 * a), 2) + pow(1.0 * (rotated.y - y) / (1.0 * b), 2);
			if (distance <= 1) // If the distance is less than or equal to 1, the point is inside the ellipse.
			{
				if (get(i, j) == state) // If the cell's state is exactly `state`, increment result.
					result++;
			}
		}
	}
	return result; // Returns the total count.
}


// `isValid` method: Checks if the given coordinates (x, y) are within the grid boundaries.
bool Grid::isValid(int x, int y)
{
	// Returns true if x is within [0, width) and y is within [0, height).
	return (x >= 0) && (x < width) &&
		(y >= 0) && (y < height);
}


// `floodFill` method: Performs a flood fill algorithm starting from `src`.
// It marks connected `HARD` cells as `PERCOLATE`.
int Grid::floodFill(char* mat, bool* ivisited, point2d src)
{
	// Checks if the source cell is walkable.
	if (!mat[src.y * width + src.x])
	{
		cout << "* \n***Error bad destination abord path*** \n"; // Error message.
		return -1; // Returns -1 if the source is not valid.
	}

	// Marks the source cell as visited.
	ivisited[src.y * width + src.x] = true;

	std::stack<point2d> q; // Creates a stack for DFS-like traversal.
	point2d s = src;       // Copies the source point.
	q.push(s);             // Pushes the source cell onto the stack.

	// Performs a traversal starting from the source cell.
	while (!q.empty())
	{
		point2d pt = q.top(); // Gets the top element from the stack.

		if (pt.y >= height - 1) // If the bottom row is reached, a percolation path is found.
		{
			return 1; // Returns 1 indicating percolation.
		}
		mat[(size_t)pt.y * width + pt.x] = PERCOLATE; // Marks the current cell as `PERCOLATE`.

		q.pop(); // Removes the current cell from the stack.
		// Iterates through the 4 neighbors (Left, Down, Up, Right).
		for (int i = 3; i > -1; i--)
		{
			int x = pt.x + xNum[i]; // Calculates neighbor's x-coordinate.
			int y = pt.y + yNum[i]; // Calculates neighbor's y-coordinate.

			// If the neighbor is valid and is a `HARD` cell, push it to the stack.
			if (isValid(x, y) && (mat[(size_t)y * width + x] == CellState::HARD))
			{
				q.push({ x,y }); // Pushes the neighbor onto the stack.
			}
		}
	}
	return -1; // Returns -1 if the destination (bottom row) cannot be reached.
}


// `DFS` method: Finds if a percolative path exists from `src` to the bottom row using DFS.
// It marks the path as `PERCOLATE` and returns the `target` point and path `distance`.
int Grid::DFS(char* mat, bool* ivisited, point2dShort src, point2d* target, int* distance)
{
	int sCounter = 0; // Initializes path length counter.
	size_t position = (size_t)src.y * width + src.x; // Calculates the linear index of the source.
	if (!mat[position]) // Checks if the source cell is valid for traversal.
	{
		cout << "Warning:Bad source point at BFS\n"; // Warning message.
		return false; // Returns false if source is invalid.
	}

	ivisited[position] = true; // Marks the source cell as visited.
	std::stack<point2dShort> s; // Creates a stack for DFS.
	s.push(src); // Pushes the source cell onto the stack.

	// Performs DFS traversal.
	while (!s.empty())
	{
		point2dShort curr = s.top(); // Gets the top cell from the stack.
		mat[(size_t)curr.y * width + curr.x] = PERCOLATE; // Marks the current cell as `PERCOLATE`.
		s.pop(); // Removes the current cell from the stack.
		sCounter++; // Increments path length.

		if (curr.y >= (height - 1)) // If the bottom row is reached.
		{
			target->x = curr.x; // Sets the target x-coordinate.
			target->y = curr.y; // Sets the target y-coordinate.
			*distance = sCounter; // Sets the total distance.
			return true; // Returns true indicating percolation.
		}

		// Iterates through the 4 neighbors.
		for (int i = 3; i > -1; i--)
		{
			int x = curr.x + xNum[i]; // Calculates neighbor's x-coordinate.
			int y = curr.y + yNum[i]; // Calculates neighbor's y-coordinate.

			position = (size_t)y * width + x; // Calculates neighbor's linear index.
			// If neighbor is valid and is `HARD` or `SOFT` (percolable materials), push it to the stack.
			if (isValid(x, y) && (mat[position] == CellState::HARD || mat[position] == CellState::SOFT))
				s.push({ short(x), short(y) }); // Pushes the neighbor to the stack.
		}
	}
	return false; // Returns false if percolation path is not found.
}


// `countPathPixels` method: Counts the number of pixels marked as `PATH` in the grid.
size_t Grid::countPathPixels(char* mat)
{
	int result = 0; // Initializes counter.
	for (int j = 0; j < height; j++) // Iterates through rows.
		for (int i = 0; i < width; i++) // Iterates through columns.
			if (mat[j * width + i] == PATH) // If the cell is `PATH`, increment count.
				result++;
	return result; // Returns total `PATH` pixels.
}


// `DFSRestore` method: Restores cells marked `PERCOLATE` or `PATH` back to `HARD` and `visited` to `false`.
int Grid::DFSRestore(char* mat, bool* ivisited, point2dShort src)
{
	std::stack<point2dShort> q; // Creates a stack for traversal.
	size_t position = (size_t)src.y * width + src.x; // Calculates linear index.
	if (!mat[position]) // Checks if the source cell is valid.
	{
		cout << "Warning:Bad source point at BFS\n"; // Warning message.
		return false; // Returns false if source is invalid.
	}

	q.push(src); // Pushes the source cell to the stack.

	// Performs traversal.
	while (!q.empty())
	{
		point2dShort curr = q.top(); // Gets the top element.
		position = (size_t)curr.y * width + curr.x; // Calculates linear index.

		ivisited[position] = false; // Marks cell as unvisited.
		mat[position] = HARD;       // Sets cell state back to `HARD`.

		q.pop(); // Removes the current cell from stack.

		// Iterates through neighbors.
		for (int i = 3; i >= 0; i--)
		{
			int x = curr.x + xNum[i]; // Neighbor x-coordinate.
			int y = curr.y + yNum[i]; // Neighbor y-coordinate.

			position = (size_t)y * width + x; // Neighbor linear index.
			// If neighbor is valid and was part of a `PERCOLATE` or `PATH`, push it to stack.
			if ((isValid(x, y) && (mat[position] == PERCOLATE)) || ((isValid(x, y) && (mat[position] == PATH))))
			{
				q.push({ short(x), short(y) }); // Pushes neighbor to stack.
			}
		}
	}
	return true; // Returns true upon completion.
}


// `CalcPropetriesAtPath` method: Calculates electrical, thermal, and mechanical properties along a given path.
void Grid::CalcPropetriesAtPath(char* mat, bool* ivisited, list<smallQueueNode> path, double* resistance, double* ielectricConductivities, double* thermalResistance, double* ithermalConductivities, double* YoungModulus, double* iYoungModulus, double* PoissonRatio, double* iPoissonRatio)
{
	queueNode currentPoint; // Declares a `queueNode` to hold current point data and properties.
	smallQueueNode smallCurrentPoint; // Declares a `smallQueueNode` for path traversal.
	double inSeriesYoungModulus = 0; // Initializes Young's Modulus for series calculation.
	double inSeriesPoissonRatio = 0; // Initializes Poisson's Ratio for series calculation.

	*resistance = 0;        // Initializes total resistance.
	*thermalResistance = 0; // Initializes total thermal resistance.
	*YoungModulus = 0;      // Initializes total Young's Modulus.
	*PoissonRatio = 0;      // Initializes total Poisson's Ratio.

	std::list<smallQueueNode> backUp; // A list to temporarily store path nodes.
	size_t position;                  // Variable for linear index.
	int material;                     // Variable for material type.

	// Iterates through the provided path.
	while (!path.empty())
	{
		smallCurrentPoint = path.front(); // Gets the first node in the path.
		path.pop_front();                 // Removes the first node.

		// Transfers data from `smallQueueNode` to `queueNode`.
		currentPoint.pt = smallCurrentPoint.pt;
		currentPoint.previous = smallCurrentPoint.previous;
		currentPoint.dist = smallCurrentPoint.dist;

		position = (size_t)currentPoint.pt.y * width + currentPoint.pt.x; // Calculates linear index.
		material = ingadients[position]; // Gets the material type of the current cell.

		// Assigns initial resistance, thermal resistance, Young's Modulus, and Poisson's Ratio based on material.
		currentPoint.resistance = ielectricConductivities[material];
		currentPoint.thermalResistance = ithermalConductivities[material];
		currentPoint.YoungModulus = iYoungModulus[material];
		currentPoint.PoissonRatio = iPoissonRatio[material];

		// Calls `calcPropertiesAtPoint` to calculate properties considering side paths.
		calcPropertiesAtPoint(mat, ivisited, &currentPoint, ielectricConductivities, ithermalConductivities, iYoungModulus, iPoissonRatio);
		backUp.push_back(currentPoint); // Stores the processed point in `backUp`.

		*resistance += currentPoint.resistance;       // Accumulates total resistance.
		*thermalResistance += currentPoint.thermalResistance; // Accumulates total thermal resistance.
	}

	// Calculates Young's Modulus and Poisson's Ratio for sections in series.
	while (!backUp.empty())
	{
		smallCurrentPoint = backUp.front(); // Gets the first node from `backUp`.
		backUp.pop_front();                 // Removes it.

		// Transfers data to `currentPoint`.
		currentPoint.pt = smallCurrentPoint.pt;
		currentPoint.previous = smallCurrentPoint.previous;
		currentPoint.dist = smallCurrentPoint.dist;

		inSeriesYoungModulus = currentPoint.YoungModulus;   // Initializes series Young's Modulus for the current segment.
		inSeriesPoissonRatio = currentPoint.PoissonRatio;   // Initializes series Poisson's Ratio for the current segment.

		// Combines properties for horizontally connected segments (same y-coordinate).
		while ((currentPoint.previous.y == currentPoint.pt.y) && (!backUp.empty()))
		{
			smallCurrentPoint = backUp.front(); // Gets next node if same line.
			// Updates currentPoint data.
			currentPoint.pt = smallCurrentPoint.pt;
			currentPoint.previous = smallCurrentPoint.previous;
			currentPoint.dist = smallCurrentPoint.dist;

			backUp.pop_front(); // Removes it.
			inSeriesYoungModulus += currentPoint.YoungModulus; // Accumulates Young's Modulus.
			inSeriesPoissonRatio += currentPoint.PoissonRatio; // Accumulates Poisson's Ratio.
		}
		(*YoungModulus) += (1. / inSeriesYoungModulus); // Adds reciprocal of series Young's Modulus.
		(*PoissonRatio) += (1. / inSeriesPoissonRatio); // Adds reciprocal of series Poisson's Ratio.
	}

	// Final calculations for average properties.
	(*YoungModulus) = (1 / *YoungModulus);   // Inverts Young's Modulus.
	(*PoissonRatio) = (1 / *PoissonRatio);   // Inverts Poisson's Ratio.
	(*YoungModulus) = (*YoungModulus / width); // Adjusts Young's Modulus by width.

	return; // Function completes.
}

// `calcPropertiesAtPoint` method: Calculates properties for a specific point, considering
// parallel contributions from neighboring cells.
void Grid::calcPropertiesAtPoint(char* mat, bool* ivisited, queueNode* currentPoint, double* ielectricConductivities, double* ithermalConductivities, double* iYoungModulus, double* iPoissonRatio)
{
	Direction pathDirectionAtPoint; // Determines if the path segment is horizontal or vertical.
	size_t position = (size_t)currentPoint->pt.y * width + currentPoint->pt.x; // Linear index of the current point.
	int x, y;                                                                  // Temporary variables for coordinates.
	int material = ingadients[position];                                       // Material type of the current cell.
	bool sameline = false;                                                     // Flag to check if neighbors are on the same line.

	// Initial resistance and thermal resistance based on the current cell's material.
	double currResistance = ielectricConductivities[material];
	double currThermalResistance = ithermalConductivities[material];

	currentPoint->resistance = currResistance;       // Sets initial resistance.
	currentPoint->thermalResistance = currThermalResistance; // Sets initial thermal resistance.

	x = currentPoint->pt.x; // Current cell's x-coordinate.
	y = currentPoint->pt.y; // Current cell's y-coordinate.

	// Determines the direction of the path segment based on the previous point.
	if ((currentPoint->pt.x - currentPoint->previous.x) == 0)
		pathDirectionAtPoint = VERTICAL;   // Vertical movement (x unchanged).
	else
		pathDirectionAtPoint = HORIZONTAL; // Horizontal movement (y unchanged).

	// Explores "left" or "up" side-paths (perpendicular to main path direction).
	do
	{
		if (pathDirectionAtPoint == HORIZONTAL)
			y--; // Move "up" for a horizontal main path.
		else
			x--; // Move "left" for a vertical main path.

		position = (size_t)y * width + x; // Linear index of the neighbor.
		// Check if neighbor is valid and `HARD` (part of the material).
		if (isValid(x, y) && (mat[position] == HARD))
			material = ingadients[position]; // Get material of neighbor.
		else
			break; // Stop if invalid or not `HARD`.

		// Add reciprocal resistance/thermal resistance of the neighbor in parallel.
		currResistance = 1.0 / ielectricConductivities[ingadients[position]];
		currThermalResistance = 1.0 / ithermalConductivities[ingadients[position]];
		currentPoint->resistance += currResistance;
		currentPoint->thermalResistance += currThermalResistance;

		sameline = (mat[position] == HARD); // Check if the current neighbor is still `HARD`.
		ivisited[position] = true;         // Mark neighbor as visited.
		mat[position] = SIDEPATH;          // Mark neighbor as `SIDEPATH`.

	} while ((material != 0) && (sameline)); // Continue as long as material is not 0 and on the same line.

	x = currentPoint->pt.x; // Reset x to current point's x.
	y = currentPoint->pt.y; // Reset y to current point's y.

	// Explores "right" or "down" side-paths (perpendicular to main path direction).
	do
	{
		if (pathDirectionAtPoint == HORIZONTAL)
			y++; // Move "down" for a horizontal main path.
		else
			x++; // Move "right" for a vertical main path.
		position = (size_t)y * width + x; // Linear index of the neighbor.

		// Check if neighbor is valid and not already part of the main `PATH`.
		if (isValid(x, y) && (mat[position] != PATH))
			material = ingadients[position]; // Get material of neighbor.
		else
			break; // Stop if invalid or is `PATH`.

		// Add reciprocal resistance/thermal resistance of the neighbor in parallel.
		currResistance = 1.0 / ielectricConductivities[ingadients[position]];
		currThermalResistance = 1.0 / ithermalConductivities[ingadients[position]];
		currentPoint->resistance += currResistance;
		currentPoint->thermalResistance += currThermalResistance;

		sameline = (mat[position] == HARD); // Check if the current neighbor is still `HARD`.
		ivisited[position] = true;         // Mark neighbor as visited.
		mat[position] = SIDEPATH;          // Mark neighbor as `SIDEPATH`.

	} while ((material != 0) && sameline); // Continue as long as material is not 0 and on the same line.

	currentPoint->resistance = 1.0 / currentPoint->resistance;       // Invert total resistance (parallel combination).
	currentPoint->thermalResistance = 1.0 / currentPoint->thermalResistance; // Invert total thermal resistance (parallel combination).
}


// `MarkMinimumPath` method: Reconstructs and marks the minimum path found by BFS/DFS.
void Grid::MarkMinimumPath(char* mat, bool* ivisited, stack<smallQueueNode>* clusterStack, list<smallQueueNode>* listPath)
{
	smallQueueNode last = clusterStack->top(); // Gets the last node from the stack.
	clusterStack->pop();                       // Removes it.
	listPath->push_front(last);                // Adds it to the front of `listPath` (reconstructing in reverse).
	smallQueueNode currentPoint;               // Temporary variable for current point.

	// Traverses the stack to reconstruct the path.
	while (!clusterStack->empty())
	{
		currentPoint = clusterStack->top(); // Gets the next node.
		clusterStack->pop();                // Removes it.

		size_t index = ((size_t)currentPoint.pt.y * width) + currentPoint.pt.x; // Linear index.
		ivisited[index] = false; // Marks cell as unvisited.
		mat[index] = HARD;       // Sets cell state back to `HARD`.

		// If the current node's previous matches the `last` node's current, it's part of the path.
		if ((last.previous.x == currentPoint.pt.x) && (last.previous.y == currentPoint.pt.y))
		{
			last = currentPoint;        // Updates `last` to current.
			listPath->push_front(last); // Adds `last` to `listPath`.
		}
	}
}


// `drawPath` method: Draws a single path onto the grid `mat`.
int Grid::drawPath(char* mat, bool* ivisited, list<smallQueueNode> cpath)
{
	std::list<smallQueueNode>::iterator it; // Iterator for the path list.

	for (it = cpath.begin(); it != cpath.end(); ++it) // Iterates through each node in the path.
	{
		size_t position = (size_t)it->pt.y * width + it->pt.x; // Linear index.
		mat[position] = PATH;      // Marks cell as `PATH`.
		ivisited[position] = true; // Marks cell as visited.
	}
	return 0; // Returns 0 upon completion.
}


// `drawPaths` method: Draws all stored paths from `pathsList` onto the grid `mat`.
int Grid::drawPaths(char* mat, bool* ivisited)
{
	std::list<std::list<smallQueueNode>>::iterator it; // Iterator for the list of paths.

	for (it = pathsList.begin(); it != pathsList.end(); ++it) // Iterates through each path in `pathsList`.
		drawPath(mat, ivisited, *it); // Calls `drawPath` for each individual path.

	return 0; // Returns 0 upon completion.
}


// `BFS` method: Performs a Breadth-First Search to find a percolation path and calculate properties.
int Grid::BFS(char* mat, bool* ivisited, point2d src, int* distance, double* ielectricConductivities, double* resistance, double* ithermalConductivities, double* thermalResistance, double* iYoungModulus, double* YoungModulus, double* iPoissonRatio, double* PoissonRatio, point2d* finalPoint)
{
	*distance = 0;        // Initializes distance.
	*resistance = 0;      // Initializes resistance.
	*thermalResistance = 0; // Initializes thermal resistance.
	*YoungModulus = 0;    // Initializes Young's Modulus.
	*PoissonRatio = 0;    // Initializes Poisson's Ratio.

	size_t position = (size_t)src.y * width + src.x; // Linear index of source.
	if (!mat[position]) // Checks if source is valid.
	{
		cout << "Warning:Bad source point at BFS\n"; // Warning message.
		return false; // Returns false if source is invalid.
	}

	ivisited[position] = true; // Marks source as visited.

	std::queue<smallQueueNode> q;    // Queue for BFS traversal.
	std::stack<smallQueueNode> backup; // Stack to store path for reconstruction.
	std::list<smallQueueNode> singlePath; // List to store the reconstructed single path.

	smallQueueNode s = { src, {0,0},0 }; // Initializes source node.
	q.push(s); // Pushes source to queue.
	smallQueueNode curr; // Current node variable.

	// Performs BFS traversal.
	while (!q.empty())
	{
		curr = q.front(); // Gets current node from queue.
		point2d pt = curr.pt; // Current point coordinates.

		if (pt.y >= (height - 1)) // If bottom row is reached.
		{
			*distance = curr.dist + 1; // Sets total distance.
			finalPoint->x = pt.x;     // Sets final point x.
			finalPoint->y = pt.y;     // Sets final point y.
			break; // Exit loop.
		}

		q.pop(); // Removes current node from queue.

		// Iterates through neighbors.
		for (int i = 3; i > -1; i--)
		{
			int x = pt.x + xNum[i]; // Neighbor x-coordinate.
			int y = pt.y + yNum[i]; // Neighbor y-coordinate.
			position = (size_t)y * width + x; // Neighbor linear index.

			// If neighbor is valid, `PERCOLATE`, and not visited.
			if ((isValid(x, y)) && (mat[position] == PERCOLATE) && (ivisited[position] == false))
			{
				smallQueueNode Adjcell = { {x, y},{pt.x,pt.y},curr.dist + 1 }; // Creates new node for neighbor.
				q.push(Adjcell);    // Pushes neighbor to queue.
				backup.push(Adjcell); // Pushes neighbor to backup stack for path reconstruction.
				ivisited[position] = true; // Marks neighbor as visited.
			}
		}
	}

	this->pathsList.push_front(singlePath); // Adds an empty path to `pathsList` to be filled.
	MarkMinimumPath(mat, ivisited, &backup, &pathsList.front()); // Reconstructs the minimum path.
	drawPath(cell, visited, pathsList.front()); // Draws the found path on the main `cell` grid.

	return true; // Returns true upon completion.
}


// `clusterCenter` method: Finds the center of mass and total points of a cluster starting from `src`.
int Grid::clusterCenter(point2d src, doublepoint2d* center, size_t* totalPoints)
{
	size_t id = (size_t)(src.y) * width + src.x; // Linear index of source.
	double centerX = 0;    // Initializes sum of x-coordinates.
	double centerY = 0;    // Initializes sum of y-coordinates.
	size_t totalP = 0;     // Initializes total points in cluster.

	std::queue<point2d> q; // Queue for BFS traversal.

	clusterVisited[id] = true; // Marks source as visited for cluster traversal.
	q.push(src); // Pushes source to queue.

	// Performs BFS traversal to find all points in the cluster.
	while (!q.empty())
	{
		point2d pt = q.front(); // Gets current point.
		totalP++;              // Increments total points.

		centerX = centerX + pt.x; // Accumulates x-coordinates.
		centerY = centerY + pt.y; // Accumulates y-coordinates.

		q.pop(); // Removes current point from queue.

		// Iterates through neighbors.
		for (int i = 0; i < 4; i++)
		{
			int x = pt.x + xNum[i]; // Neighbor x-coordinate.
			int y = pt.y + yNum[i]; // Neighbor y-coordinate.

			id = (size_t)(y)*width + x; // Neighbor linear index.

			// If neighbor is valid, is a material (`HARD`, `PERCOLATE`, `SOFT`), and not visited for clustering.
			if ((isValid(x, y) && ((cell[id] == CellState::HARD) || (cell[id] == PERCOLATE) || (cell[id] == CellState::SOFT)) && (clusterVisited[id] == false)))
			{
				clusterVisited[id] = true; // Marks neighbor as visited for clustering.
				q.push({ x,y });         // Pushes neighbor to queue.
			}
		}
	}

	center->x = centerX / totalP; // Calculates average x-coordinate.
	center->y = centerY / totalP; // Calculates average y-coordinate.
	*totalPoints = totalP;       // Sets total points.

	return 0; // Returns 0 upon completion.
}


// `clusterInertia` method: Calculates the inertia and radius of gyration of a cluster.
int Grid::clusterInertia(char* mat, unsigned char* clustersImage, point2d src, doublepoint2d center, size_t totalPoints, double* inertia, double* radiusOfGyration)
{
	double sum = 0; // Initializes sum for inertia calculation.
	int x, y;       // Temporary variables for coordinates.
	double distance; // Squared distance from center.
	std::queue<point2d> q; // Queue for BFS traversal.
	char color = rand() % 0xC0 + 0x2F; // Generates a random color for visualization.
	size_t id = (size_t)src.y * width + src.x; // Linear index of source.

	clusterVisited[id] = false; // Resets visited flag for this cluster calculation.
	clustersImage[id] = color; // Assigns color to the source cell in `clustersImage`.

	q.push(src); // Pushes source to queue.

	// Performs BFS traversal.
	while (!q.empty())
	{
		point2d pt = q.front(); // Gets current point.
		q.pop();               // Removes current point.

		// Calculates squared Euclidean distance from the cluster's center of mass.
		distance = ((center.x - pt.x) * (center.x - pt.x)) + ((center.y - pt.y) * (center.y - pt.y));
		sum = sum + distance; // Accumulates sum of squared distances.

		// Iterates through neighbors.
		for (int i = 0; i < 4; i++)
		{
			x = pt.x + xNum[i]; // Neighbor x-coordinate.
			y = pt.y + yNum[i]; // Neighbor y-coordinate.

			id = (size_t)(y)*width + x; // Neighbor linear index.

			// If neighbor is valid and was marked as visited (meaning it belongs to the current cluster).
			if ((isValid(x, y) && (clusterVisited[id] == true)))
			{
				clustersImage[id] = color; // Assigns the cluster color.
				clusterVisited[id] = false; // Marks as processed (unvisited for next cluster search).
				q.push({ x,y });          // Pushes neighbor to queue.
			}
		}
	}

	*inertia = sum; // Sets inertia.
	*radiusOfGyration = sqrt(sum / totalPoints); // Calculates radius of gyration.

	return 0; // Returns 0 upon completion.
}


// `markClusters` method: Identifies and marks all distinct clusters in the grid.
Cluster Grid::markClusters()
{
	int i, j; // Loop counters.
	size_t id; // Linear index.
	doublepoint2d center(0.0, 0.0); // Initializes cluster center.
	Cluster currCluster; // Current cluster object.
	point2d src; // Source point for cluster discovery.

	std::queue<Cluster> cClusters; // Queue to hold discovered clusters.
	double maxRadius = 0; // Tracks the maximum cluster radius found.

	// Iterates through all cells in the grid.
	for (i = 0; i < width; i++)
	{
		for (j = 0; j < height; j++)
		{
			src.x = i; // Sets source x.
			src.y = j; // Sets source y.
			id = (size_t) int(j) * width + i; // Linear index.

			// If cell is `HARD` and not yet visited as part of a cluster.
			if ((cell[id] == CellState::HARD) && (clusterVisited[id] == false))
			{
				// Finds the center of mass and total points for the cluster.
				clusterCenter(src, &currCluster.center, &currCluster.totalPoints);
				currCluster.start = src; // Sets the starting point of the cluster.
				cClusters.push(currCluster); // Adds the discovered cluster to the queue.
			}
		}
	}

	// Processes each cluster in the queue.
	while (!cClusters.empty())
	{
		currCluster = cClusters.front(); // Gets the next cluster.
		cClusters.pop();                 // Removes it.
		// Calculates inertia and radius of gyration for the current cluster.
		clusterInertia(cell, cluster, currCluster.start, currCluster.center, currCluster.totalPoints, &currCluster.inertia, &currCluster.radius);
		// Stores the cluster statistics.
		Clusters.push_back({ currCluster.radius,currCluster.inertia,currCluster.totalPoints });

		if (currCluster.radius > maxRadius) // Updates maximum radius if current is larger.
			maxRadius = currCluster.radius;
	}

	cout << "Max Radius = " << maxRadius << "\n"; // Prints the maximum radius.
	cMaxClusterRadius.push_back(maxRadius); // Stores the maximum cluster radius.
	return currCluster; // Returns the last processed cluster (or a default if no clusters were found).
}


// `findCorrelationLength` method: Calculates the correlation length based on cluster statistics.
double Grid::findCorrelationLength(std::vector<clusterStatistics> iClusters)
{
	double numerator = 0;   // Initializes numerator for correlation length formula.
	double denominator = 0; // Initializes denominator.
	double weight = 0;      // Weight factor.

	for (size_t i = 0; i < iClusters.size(); i++) // Iterates through all cluster statistics.
	{
		weight = double(iClusters[i].totalPoints) * double(iClusters[i].totalPoints); // Weight is square of total points.
		numerator = numerator + ((iClusters[i].radius * iClusters[i].radius) * weight); // Accumulates radius squared times weight.
		denominator = denominator + weight; // Accumulates weight.
	}

	return (double)(numerator / denominator); // Returns the correlation length.
}


// `percolate` method (private): Implements the core percolation logic using flood fill.
// This version is parallelized with OpenMP sections.
int  Grid::percolate(void)
{
	int iter; // Loop counter.
	int step = width / 8; // Divides the width into 8 steps for parallel processing.
	// Array to store results from parallel flood fill operations.
	int result[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };

	// Iterates through a portion of the grid's width.
	for (iter = 0; iter < width / 8; iter++)
	{
#pragma omp parallel sections // Defines parallel sections.
		{
#pragma omp section // Each section performs a flood fill from a different starting column.
			result[0] = floodFill(cell, visited, { iter,0 });
#pragma omp section
			result[1] = floodFill(cell, visited, { step + iter,0 });
#pragma omp section
			result[2] = floodFill(cell, visited, { 2 * step + iter,0 });
#pragma omp section
			result[3] = floodFill(cell, visited, { 3 * step + iter,0 });
#pragma omp section
			result[4] = floodFill(cell, visited, { 4 * step + iter,0 });
#pragma omp section
			result[5] = floodFill(cell, visited, { 5 * step + iter,0 });
#pragma omp section
			result[6] = floodFill(cell, visited, { 6 * step + iter,0 });
#pragma omp section
			result[7] = floodFill(cell, visited, { 7 * step + iter,0 });
		}

		// Checks if any of the parallel flood fills found a percolation path.
		if ((result[0] > -1) || (result[1] > -1) || (result[2] > -1) || (result[3] > -1) ||
			(result[4] > -1) || (result[5] > -1) || (result[6] > -1) || (result[7] > -1))
			break; // If a path is found, break the loop.
	}

	// If a percolation path was found within the iterations.
	if (iter < (width / 8) - 1)
		// Marks the entire last row as `PERCOLATE` (likely indicating the successful connection).
		memset(end, PERCOLATE, width);

	// Returns true if percolation occurred (iter did not complete all width/8 steps).
	return  iter < (width / 8);
}


// `percolate` method (public): Calls the private `percolate` method and measures its execution time.
int Grid::percolate(double* proccessTime)
{
	int result; // Stores the result of percolation.
	clock_t istart = clock(); // Records start time.
	result = percolate(); // Calls the core percolation logic.
	clock_t iend = clock(); // Records end time.
	// Calculates processing time in seconds.
	*proccessTime = ((double)(iend - istart)) / CLOCKS_PER_SEC;
	return result; // Returns the percolation result.
}


// `percolateWithRealPathLength` method: Performs percolation, identifies real paths,
// and calculates electrical, thermal, and mechanical properties.
int  Grid::percolateWithRealPathLength(double* totalPaths, double* meanLength, double* meanCalculatedLength, double* ielectricConductivities, double* ithermalConductivities, double* iYoungModulus, double* iPoissonRatio, double* meanRVEResistance, double* meanRVEThermalResistance, double* meanRVEYoungModulus, double* MeanRVEPoissonRatio)
{
	int i; // Loop counter.
	point2dShort startPoint; // Starting point for DFS.
	point2d endPoint = { 0,0 }; // Ending point of a path.

	// Arrays to store properties for parallel processing (though current loop is sequential).
	double pelectricResistance[4] = { 0,0,0,0 };
	double pthermalResistance[4] = { 0,0,0,0 };
	double pYoungModulus[4] = { 0,0,0,0 };
	double pPoissonRatio[4] = { 0,0,0,0 };
	int realLength[4] = { 0,0,0,0 };
	int pathsLength[4] = { 0,0,0,0 };

	double sumResistance = 0;     // Accumulator for reciprocal of resistance.
	double sumThermalResistance = 0; // Accumulator for reciprocal of thermal resistance.
	double sumRealLength = 0;     // Accumulator for real path lengths.

	*totalPaths = 0;            // Initializes total number of paths.
	*meanLength = 0;            // Initializes mean path length.
	*meanCalculatedLength = 0;  // Initializes mean calculated path length.

	bool isPercolate = false; // Flag for percolation.

	// Iterates through each column at the top row.
	for (i = 0; i < width; i++)
	{
		isPercolate = false; // Reset percolation flag for each column.
		startPoint = { (short)i,(short)0 }; // Sets the starting point (top row, current column).
		// If the starting cell is `HARD` or `SOFT`, attempt DFS.
		if (cell[i] == CellState::HARD || cell[i] == CellState::SOFT)
			isPercolate = DFS(cell, visited, startPoint, &endPoint, &pathsLength[0]); // Performs DFS to find a path.

		if (isPercolate) // If a percolation path is found.
		{
			// Performs BFS to find the shortest path and calculate properties along it.
			BFS(cell, visited, { i,0 }, &realLength[0], ielectricConductivities, &pelectricResistance[0], ithermalConductivities, &pthermalResistance[0], iYoungModulus, &pYoungModulus[0], iPoissonRatio, &pPoissonRatio[0], &endPoint);
			*totalPaths = *totalPaths + 1; // Increments total paths found.
			sumRealLength = sumRealLength + realLength[0]; // Accumulates real path lengths.
		}
	}

	sumResistance = 0; // Reset sum of resistances.
	// Iterates through all found paths to calculate detailed properties.
	std::list<std::list<smallQueueNode>>::iterator it;
	for (it = pathsList.begin(); it != pathsList.end(); ++it)
	{
		// Calculates properties along the current path.
		CalcPropetriesAtPath(cell, visited, *it, pelectricResistance, ielectricConductivities, pthermalResistance, ithermalConductivities,
			pYoungModulus, iYoungModulus, pPoissonRatio, iPoissonRatio);

		sumResistance += 1 / pelectricResistance[0];       // Accumulates reciprocal of resistance.
		sumThermalResistance += 1 / pthermalResistance[0]; // Accumulates reciprocal of thermal resistance.
	}

	// Final calculations for mean properties if paths were found.
	if ((*totalPaths) > 0)
	{
		memset(end, PERCOLATE, width); // Marks the bottom row as `PERCOLATE`.

		(*meanLength) = (sumRealLength) / (*totalPaths); // Calculates mean path length.
		(*meanCalculatedLength) = (sumRealLength) / (*totalPaths); // Also mean calculated length.

		(*meanRVEResistance) = 1 / sumResistance;       // Calculates mean RVE electrical resistance.
		(*meanRVEThermalResistance) = 1 / sumThermalResistance; // Calculates mean RVE thermal resistance.

		// Calculates mean RVE path width for mechanical properties.
		double meanRVEPathWidth = (sumResistance / ielectricConductivities[1]) * (*meanLength / (*totalPaths));
		// Calculates mean RVE Young's Modulus.
		(*meanRVEYoungModulus) = pow((iYoungModulus[1] * (*totalPaths) * meanRVEPathWidth), (width / (*meanCalculatedLength))) + (iYoungModulus[0] * (width - ((*totalPaths) * meanRVEPathWidth)) / width);
		// Calculates mean RVE Poisson's Ratio.
		(*MeanRVEPoissonRatio) = (iPoissonRatio[1] * (*totalPaths) * meanRVEPathWidth / width) + (iPoissonRatio[0] * (width - ((*totalPaths) * meanRVEPathWidth)) / width);
	}
	else // If no paths were found, properties are set to default or initial values.
	{
		(*meanLength) = 0;
		*meanCalculatedLength = 0;
		*meanRVEResistance = 0;
		*meanRVEThermalResistance = 0;
		*meanRVEYoungModulus = iYoungModulus[0];
		*MeanRVEPoissonRatio = iPoissonRatio[0];
	}

	pathsList.clear(); // Clears the list of paths.
	return (*totalPaths > 0); // Returns true if any paths were found, false otherwise.
}

// `percolateWithRealPathLength` method (overloaded): Calls the detailed percolation
// method and prints the results and processing time.
int  Grid::percolateWithRealPathLength(double* totalpaths, double* meanlength, double* meanRealLength, double* proccessTime, double* ielectricConductivities, double* omeanRVEResistance, double* ithermalConductivities, double* omeanRVEThermalResistance, double* iYoungModulus, double* omeanRVEYoungModulus, double* iPoissonRatio, double* omeanRVEPoissonRatio)
{
	int result; // Stores the result.
	clock_t istart = clock(); // Records start time.
	// Calls the detailed percolation method.
	result = percolateWithRealPathLength(totalpaths, meanlength, meanRealLength, ielectricConductivities, ithermalConductivities, iYoungModulus, iPoissonRatio, omeanRVEResistance, omeanRVEThermalResistance, omeanRVEYoungModulus, omeanRVEPoissonRatio);

	clock_t iend = clock(); // Records end time.
	*proccessTime = ((double)(iend - istart)) / CLOCKS_PER_SEC; // Calculates processing time.
	cout << "mean Paths per RVE=" << *totalpaths << "\n";          // Prints total paths.
	cout << "mean RVE electric resistance=" << *omeanRVEResistance << "\n"; // Prints mean electric resistance.
	cout << "mean RVE Thermal resistance=" << *omeanRVEThermalResistance << "\n"; // Prints mean thermal resistance.
	cout << "mean RVE Young Modulus=" << *omeanRVEYoungModulus << "\n";       // Prints mean Young's Modulus.
	cout << "mean RVE Poissons Ratio=" << *omeanRVEPoissonRatio << "\n";       // Prints mean Poisson's Ratio.

	return result; // Returns the percolation result.
}

// `show` method: Prints the grid content to the console for debugging purposes.
void  Grid::show() const
{
	cout << "\n"; // Newline for formatting.

	for (int i = 0; i < height; i++) // Iterates through rows.
	{
		for (int j = 0; j < width; j++) // Iterates through columns.
		{
			std::cout << cell[i * width + j]; // Prints the character representation of each cell.
		}
		cout << "\n"; // Newline after each row.
	}
	cout << "\n"; // Extra newline at the end.
}

// `saveToDisk` method: Saves the grid as an image file (BMP or PGM).
void  Grid::saveToDisk(char* imageFileName, bool saveAsBMP)
{
	if (saveAsBMP) // If `saveAsBMP` is true, generate a BMP image.
	{
		generateBitmapImage(cell, height, width, imageFileName); // Calls function to create BMP.
	}
	else // Otherwise, generate a PGM image.
	{
		generatePGMImage(cell, height, width, imageFileName); // Calls function to create PGM.
	}
}


// `saveToDisk` method (overloaded): Saves the grid to a specified `path` with `imageFileName`.
void  Grid::saveToDisk(char* path, char* imageFileName, bool saveAsBMP)
{
	// Allocates memory for the full filename (path + filename + null terminator).
	char* filename{ new char[strlen(path) + strlen(imageFileName) + 1] };

	strcpy(filename, path);    // Copies the path.
	strcat(filename, imageFileName); // Concatenates the image filename.
	saveToDisk(filename, saveAsBMP); // Calls the other `saveToDisk` method.
}


// Destructor for the Grid class: Frees all dynamically allocated memory.
Grid::~Grid() {
	delete[] start;         // Frees memory for `start` (and `cell`).
	delete[] ingadients;     // Frees memory for `ingadients`.
	delete[] visited;        // Frees memory for `visited`.
	delete[] clusterVisited; // Frees memory for `clusterVisited`.
	delete[] cluster;        // Frees memory for `cluster`.

	Clusters.~vector();       // Explicitly calls the destructor for `Clusters` vector.
	cMaxClusterRadius.~vector(); // Explicitly calls the destructor for `cMaxClusterRadius` vector.

	start = 0; // Sets pointers to null to prevent dangling pointers.
	end = 0;   // Sets pointers to null.
}