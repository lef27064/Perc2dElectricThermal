/*This file is part of Electric Percolation (eperc2d) program.

Created from Lefteris Lamprou lef27064@otenet.gr during PhD thesis

eperc2d is free software : you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free software Foundation, either version 3 of the License, or
(at your option) any later version.

eperc2d is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even tFhe implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Foobar.If not, see < https://www.gnu.org/licenses/>.
*/


#include "Grid.h"
#include <cstring>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

//initialize
Grid::Grid(int x, int y) : width(x), height(y)
{


	total = (size_t)width * height;
	//int clustersSize = int(0.5*sqrt(x * x + y * y));

#pragma omp parallel sections
	{
#pragma omp section
		{
			start = (char*)malloc(total + 1);
			assert(start);
			cell = start;
			size_t lastLine = width * (height - 1);
			end = start + lastLine;
			std::fill_n(start, total, CellState::EMPTY);
		}
#pragma omp section
		{
			visited = (bool*)malloc(total + 1);
			assert(visited);
			std::fill_n(visited, total, false);
		}
#pragma omp section
		{
			ingadients = (unsigned char*)malloc(total + 1);
			assert(ingadients);
			std::fill_n(ingadients, total, 0);
		}

#pragma omp section
		{
			cluster = (unsigned char*)malloc(total + 1);
			assert(cluster);
			std::fill_n(cluster, total, 0x0);
		}
#pragma omp section
		{
			clusterVisited = (bool*)malloc(total + 1);
			assert(clusterVisited);
			std::fill_n(clusterVisited, total, false);
		}

	}
};



//clear all values in cell
void Grid::clear(void)
{
#pragma omp parallel sections
	{
#pragma omp section
		std::fill_n(cell, total, CellState::EMPTY);
#pragma omp section
		std::fill_n(visited, total, false);
#pragma omp section
		std::fill_n(ingadients, total, 0);
#pragma omp section
		std::fill_n(cluster, total, 0);
#pragma omp section
		std::fill_n(clusterVisited, total, 0);
	}

}

//read value from line, column
char Grid::get(int x, int y)
{
	//if ((line < height) && (column < width) && (line >= 0) && (column >= 0))//check bounds
	return (cell[(size_t)y * width + x]);
	//else
	//	return 0;



}


//Make Swiss cheese
void Grid::inverse(void)
{
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{

			switch (get(x, y))
			{
			case CellState::EMPTY: set(x, y, CellState::HARD); break;
			case CellState::HARD:  set(x, y, CellState::EMPTY); break;
			}
		}
	}
}

int Grid::countArea()
{
	int result = 0;
	for (int i = 0; i < width; i++)
		for (int j = 0; j < height; j++)
			if (get(i, j) == CellState::HARD)
				result++;

	return result;
}

void Grid::set(int x, int y, char what)
{

	//if ((line < height) && (column < width ) && (line >= 0) && (column >= 0)) //check bounds
	cell[(size_t)y * width + x] = what;

}


void Grid::set(int x, int y, int what)
{

	//if ((line < height) && (column < width ) && (line >= 0) && (column >= 0)) //check bounds
	cell[(size_t)y * width + x] = (char)what;

}
// ------------------------------------------------------
/*
Rectangle Grid::cellCoordinates(int x, int y)
	{

		Rectangle ccell(Point(x, y), Point(x + 1, y + 1));
		return ccell;
	}
	*/


	//line = y, column = x, layer = z
void Grid::set(unsigned char* data, int x, int y, unsigned char what)
{

	data[(size_t)y * width + x] = what;

}


//line = y, column = x, layer = z
unsigned char  Grid::get(unsigned char* data, int x, int y)
{

	return data[(size_t)y * width + x];

}

iPoint Grid::toLocaldimensions(double x, double y)
{
	return iPoint(int(std::round(x)), int(std::round(y)));
}

iPoint Grid::toLocaldimensionsDown(double x, double y)
{
	//return iPoint(int(std::round(x)), int(std::round(y)));
	return iPoint(int(std::floor(x)), int(std::floor(y)));
}

iPoint Grid::toLocaldimensionsUp(double x, double y)
{
	return iPoint(int(std::round(x)), int(std::round(y)));
	//return iPoint(int(std::floor(x)), int(std::floor(y)));
}

Range Grid::fromRectangleToRange(double x1, double y1, double x2, double y2)
{
	Range result(toLocaldimensionsDown(x1, y1), toLocaldimensionsUp(x2, y2));
	return result;
}


//Check if a sloped Rectangle  have inside pixels on
bool Grid::isClearSlopedRectangle(int x, int y, int dx, int dy, double slope)
{
	Point upperRight = Point((Point::transform(x + int(dx / 2), y + int(dy / 2), x, y, slope)));
	Point lowerLeft = Point((Point::transform(x - int(dx / 2), y - int(dy / 2), x, y, slope)));

	for (int i = int(std::floor(lowerLeft.x)); i <= int(std::round(upperRight.x)); i++)
	{
		for (int j = int(std::floor(lowerLeft.y)); j <= int(std::round(upperRight.y)); j++)
		{
			Point rotated = Point::transform(i + 0.5, j + 0.5, x, y, -slope);
			if ((rotated.x <= (x + int(dx / 2))) && (rotated.x >= (x - int(dx / 2))) && ((rotated.y <= (y + int(dy / 2))) && (rotated.y >= (y - int(dy / 2)))))
			{
				if (get(i, j) == CellState::HARD)
					return false;
			}
		}
	}

	return true;
}

//Check if ellipse  have inside pixels on
bool Grid::isClearEllipse(int x, int y, int a, int b, double slope)
{
	Ellipse iEllipse(Point(x, y), a, b, slope);
	Point upperRight = Point(iEllipse.maxX(), iEllipse.maxY());
	Point lowerLeft = Point(iEllipse.minX(), iEllipse.minY());

	for (int i = int(std::floor(lowerLeft.x)); i <= int(std::round(upperRight.x)); i++)
	{
		for (int j = int(std::floor(lowerLeft.y)); j <= int(std::round(upperRight.y)); j++)
		{
			Point rotated = Point::transform(i + 0.5, j + 0.5, x, y, -slope);
			if (((rotated.x - x) * (rotated.x - x) / (a * a)) + ((rotated.y - y) * (rotated.y - y) / (b * b)) <= 1)
			{
				if (get(i, j) == CellState::HARD)
					return false;
			}
		}
	}

	return true;
}

//Count pixels with value state, inside Sloped Rectangle with Center x, y, semiaxes a and b and slope
int Grid::countPixelsInsidelopedRectangeMultiple(int x, int y, int dx, int dy, double slope, CellState state)
{

	int result = 0;
	SlopedRectangle iSlopedRect(Point(x, y), dx, dy, slope);
	Range region(toLocaldimensionsDown(iSlopedRect.minX() - 1, iSlopedRect.minY() - 1), toLocaldimensionsUp(iSlopedRect.maxX() + 1, iSlopedRect.maxY() + 1));



	//fix region from (0,0) to (width-1,height-1)
	if (region.from.x < 0)
		region.from.x = 0;
	if (region.from.y < 0)
		region.from.y = 0;
	if (region.to.x > width)
		region.to.x = width;
	if (region.to.y > height)
		region.to.y = height;

	for (int i = region.from.x; i < region.to.x; i++)
	{
		for (int j = region.from.y; j < region.to.y; j++)
		{
			Point rotated = Point::transform(i + 0.5, j + 0.5, x, y, -slope);
			if ((1.0 * rotated.x <= 1.0 * (x + (dx / 2.0))) && (1.0 * rotated.x >= 1.0 * (x - (dx / 2.0))) && ((1.0 * rotated.y <= 1.0 * (y + (dy / 2.0))) && (1.0 * rotated.y >= 1.0 * (y - (dy / 2.0)))))
			{
				if (get(i, j) >= state)
					result++;
			}
		}
	}

	return result;
}

//Count pixels with value state, inside Ellipse with Center x, y, semiaxes a and b and slope
int Grid::countPixelsInsideEllipseMultiple(int x, int y, int a, int b, double slope, CellState state)
{
	int result = 0;
	Ellipse iEllipse(Point(x, y), a, b, slope);
	Range region(toLocaldimensionsDown(iEllipse.minX() - 1, iEllipse.minY() - 1), toLocaldimensionsUp(iEllipse.maxX() + 1, iEllipse.maxY() + 1));

	//fix region from (0,0) to (width-1,height-1)
	if (region.from.x < 0)
		region.from.x = 0;
	if (region.from.y < 0)
		region.from.y = 0;
	if (region.to.x > width)
		region.to.x = width;
	if (region.to.y > height)
		region.to.y = height;



	for (int i = region.from.x; i < region.to.x; i++)
	{
		for (int j = region.from.y; j < region.to.y; j++)
		{
			Point rotated = Point::transform(i + 0.5, j + 0.5, x, y, -slope);
			double distance = pow(1.0 * (rotated.x - x) / (1.0 * a), 2) + pow(1.0 * (rotated.y - y) / (1.0 * b), 2);
			if (distance <= 1)
			{
				if (get(i, j) >= state)
					result++;
			}
		}
	}

	return result;
}




//Count pixels with value state, inside Sloped Rectangle with Center x, y, semiaxes a and b and slope
int Grid::countPixelsInsidelopedRectange(int x, int y, int dx, int dy, double slope, CellState state)
{

	int result = 0;
	SlopedRectangle iSlopedRect(Point(x, y), dx, dy, slope);
	Range region(toLocaldimensionsDown(iSlopedRect.minX() - 1, iSlopedRect.minY() - 1), toLocaldimensionsUp(iSlopedRect.maxX() + 1, iSlopedRect.maxY() + 1));



	//fix region from (0,0) to (width-1,height-1)
	if (region.from.x < 0)
		region.from.x = 0;
	if (region.from.y < 0)
		region.from.y = 0;
	if (region.to.x > width)
		region.to.x = width;
	if (region.to.y > height)
		region.to.y = height;

	for (int i = region.from.x; i < region.to.x; i++)
	{
		for (int j = region.from.y; j < region.to.y; j++)
		{
			Point rotated = Point::transform(i + 0.5, j + 0.5, x, y, -slope);
			if ((1.0 * rotated.x <= 1.0 * (x + (dx / 2.0))) && (1.0 * rotated.x >= 1.0 * (x - (dx / 2.0))) && ((1.0 * rotated.y <= 1.0 * (y + (dy / 2.0))) && (1.0 * rotated.y >= 1.0 * (y - (dy / 2.0)))))
			{
				if (get(i, j) == state)
					result++;
			}
		}
	}

	return result;
}

//Count pixels with value state, inside Ellipse with Center x, y, semiaxes a and b and slope
int Grid::countPixelsInsideEllipse(int x, int y, int a, int b, double slope, CellState state)
{
	int result = 0;
	Ellipse iEllipse(Point(x, y), a, b, slope);
	Range region(toLocaldimensionsDown(iEllipse.minX() - 1, iEllipse.minY() - 1), toLocaldimensionsUp(iEllipse.maxX() + 1, iEllipse.maxY() + 1));

	//fix region from (0,0) to (width-1,height-1)
	if (region.from.x < 0)
		region.from.x = 0;
	if (region.from.y < 0)
		region.from.y = 0;
	if (region.to.x > width)
		region.to.x = width;
	if (region.to.y > height)
		region.to.y = height;



	for (int i = region.from.x; i < region.to.x; i++)
	{
		for (int j = region.from.y; j < region.to.y; j++)
		{
			Point rotated = Point::transform(i + 0.5, j + 0.5, x, y, -slope);
			double distance = pow(1.0 * (rotated.x - x) / (1.0 * a), 2) + pow(1.0 * (rotated.y - y) / (1.0 * b), 2);
			if (distance <= 1)
			{
				if (get(i, j) == state)
					result++;
			}
		}
	}

	return result;
}


// check whether given cell (x, y,z) is a valid
// cell or not.

bool Grid::isValid(int x, int y)
{
	// return true if row number column number and depth number
	// is in range
	return (x >= 0) && (x < width) &&
		(y >= 0) && (y < height);

}

// These arrays are used to get row and column
// numbers of 4 neighbours of a given cell
/*
int xNum[] = { 0, -1 ,1, 0 };
int yNum[] = { 1,  0 ,0, -1 };

best results  *** *** as published in paper
int xNum[] = { -1, 0 ,0,  1 };
int yNum[] = { 0,  1 ,-1, 0 };

int xNum[] = { -1, 0 ,1,  0 };
int yNum[] = { 0,  1 ,0, -1 };

very good optical results
int xNum[] = {-1, 0 ,1,  0 };
int yNum[] = { 0, 1 ,0, -1 };
*/
int xNum[] = { -1, 0 ,0,  1 }; 
int yNum[] = { 0,  1 ,-1, 0 };


// DFS
int Grid::floodFill(char* mat, bool* ivisited, point2d src)
{
	// check source and destination cell
	// of the matrix have value 1

	if (!mat[src.y * width + src.x]) //|| !mat[dest.y*width*depth + dest.z*width + dest.x])
	{
		cout << "* \n***Error bad destination abord path*** \n";
		return -1;
	}

	// Mark the source cell as visited
	ivisited[src.y * width + src.x] = true;


	// Create a queue for BFS
	stack<point2d> q;

	// Distance of source cell is 0

	point2d s = src;

	s.x = src.x;
	s.y = src.y;

	q.push(s); // Enqueue source cell

	// Do a BFS starting from source cell
	while (!q.empty())
	{

		point2d pt = q.top();

		// 1f we have reached the destination cell,
		// we are done
		if (pt.y >= height - 1)
		{
			return 1;
		}
		mat[(size_t)pt.y * width + pt.x] = PERCOLATE;

		// Otherwise dequeue the front cell in the queue
		// and enqueue its adjacent cells
		q.pop();
		for (int i = 3; i > -1; i--)
		{
			int x = pt.x + xNum[i];
			int y = pt.y + yNum[i];

			// if adjacent cell is valid, has path and
			// not visited yet, enqueue it.
			if (isValid(x, y) && (mat[(size_t)y * width + x] == CellState::HARD))// &&	!ivisited[(y*width) +  x])
			{
				// mark cell as visited and enqueue it
				//ivisited[y*width + x] = true;
				//mat[(size_t )y*width + x] = PERCOLATE;

				q.push({ x,y });
			}
		}
	}
	// Return -1 if destination cannot be reached
	return -1;
}


/*Finds if a Percolative Path exists from src
* in mat and ivisited.
* and returns target point and distance
* mat: microstructure array
* ivisited: array with visited sites
* src: source point
* target: last point if reach bottom
* distance: total hoops
*/
int Grid::DFS(char* mat, bool* ivisited, point2dShort src, point2d* target, int* distance)
{
	{

		// check source and destination cell
		// of the matrix have value 1

		int sCounter = 0;
		size_t position = (size_t)src.y * width + src.x;
		if (!mat[position])
		{
			cout << "Warning:Bad source point at BFS\n";
			return false;
		}

		// Mark the source cell as visited
		ivisited[position] = true;

		// Create a queue for DFS
		stack<point2dShort> s;

		s.push(src); // Enqueue source cell

		// Do a DFS starting from source cell
		while (!s.empty())
		{
			point2dShort curr = s.top();
			mat[(size_t)curr.y * width + curr.x] = PERCOLATE;
			s.pop();
			sCounter++;

			// If we have reached the destination cell,
			// we are done

			if (curr.y >= (height - 1))
			{
				target->x = curr.x;
				target->y = curr.y;
				*distance = sCounter;
				return true;
			}

			// Otherwise stack the front cell
			// and its adjacent cells

			for (int i = 3; i > -1; i--)
			{
				int x = curr.x + xNum[i];
				int y = curr.y + yNum[i];

				position = (size_t)y * width + x;
				if (isValid(x, y) && (mat[position] == CellState::HARD || mat[position] == CellState::SOFT))
					s.push({ short(x), short(y) });

			}
		}

		// Return false if destination cannot be reached
		return false;
	}
}


size_t Grid::countPathPixels(char* mat)
{
	int result = 0;
	for (int j = 0; j < height; j++)
		for (int i = 0; i < width; i++)
			if (mat[j * width + i] == PATH)
				result++;
	return result;
}


int Grid::DFSRestore(char* mat, bool* ivisited, point2dShort src)
{
	// Create a queue for DFS
	stack<point2dShort> q;

	// check source and destination cell
	// of the matrix have value 1

	size_t position = (size_t)src.y * width + src.x;
	if (!mat[position])
	{
		cout << "Warning:Bad source point at BFS\n";
		return false;
	}

	// Mark the source cell as visited

	q.push(src); // Enqueue source cell

	// Do a DFS starting from source cell
	while (!q.empty())
	{
		point2dShort curr = q.top();
		position = (size_t)curr.y * width + curr.x;

		ivisited[position] = false;
		mat[position] = HARD;

		q.pop();

		for (int i = 3; i >= 0; i--)
		{
			int x = curr.x + xNum[i];
			int y = curr.y + yNum[i];

			position = (size_t)y * width + x;
			if ((isValid(x, y) && (mat[position] == PERCOLATE)) || ((isValid(x, y) && (mat[position] == PATH))))
			{
				q.push({ short(x), short(y) });
			}

		}
	}
	return true;
}




void Grid::CalcPropetriesAtPath(char* mat, bool* ivisited, list<smallQueueNode> path, double* resistance, double* ielectricConductivities, double* thermalResistance, double* ithermalConductivities, double* YoungModulus, double* iYoungModulus, double* PoissonRatio, double* iPoissonRatio)
{

	queueNode currentPoint;              // = { {0,0},{0,0},0,0,0,0 };
	smallQueueNode smallCurrentPoint;
	double inSeriesYoungModulus = 0;
	double inSeriesPoissonRatio = 0;

	*resistance = 0;
	*thermalResistance = 0;
	*YoungModulus = 0;
	*PoissonRatio = 0;
	list<smallQueueNode> backUp;
	size_t position;
	int material;



//	int count = 0;
	while (!path.empty())
	{
		smallCurrentPoint = path.front();

		path.pop_front();
		currentPoint.pt = smallCurrentPoint.pt;
		currentPoint.previous = smallCurrentPoint.previous;
		currentPoint.dist = smallCurrentPoint.dist;
		position = (size_t)currentPoint.pt.y * width + currentPoint.pt.x;
		material = ingadients[position];

		currentPoint.resistance = ielectricConductivities[material];
		currentPoint.thermalResistance = ithermalConductivities[material];
		currentPoint.YoungModulus= iYoungModulus[material];
		currentPoint.PoissonRatio = iPoissonRatio[material];



		calcPropertiesAtPoint(mat, ivisited, &currentPoint, ielectricConductivities, ithermalConductivities, iYoungModulus, iPoissonRatio);
		backUp.push_back(currentPoint);
		*resistance += currentPoint.resistance;
		*thermalResistance += currentPoint.thermalResistance;

	}

	while (!backUp.empty())
	{
		smallCurrentPoint = backUp.front();
		backUp.pop_front();
		currentPoint.pt = smallCurrentPoint.pt;
		currentPoint.previous = smallCurrentPoint.previous;
		currentPoint.dist = smallCurrentPoint.dist;

		inSeriesYoungModulus = currentPoint.YoungModulus;
		inSeriesPoissonRatio = currentPoint.PoissonRatio;

		while ((currentPoint.previous.y == currentPoint.pt.y) && (!backUp.empty()))
		{
			smallCurrentPoint = backUp.front();
			currentPoint.pt = smallCurrentPoint.pt;
			currentPoint.previous = smallCurrentPoint.previous;
			currentPoint.dist = smallCurrentPoint.dist;

			backUp.pop_front();
			inSeriesYoungModulus += currentPoint.YoungModulus;
			inSeriesPoissonRatio += currentPoint.PoissonRatio;

		}

		(*YoungModulus) += (1. / inSeriesYoungModulus);
		(*PoissonRatio) += (1. / inSeriesPoissonRatio);
	}


	//debug: cout << "Resistance of Path with length ( " << count << ")= " << *resistance << "\n";
	(*YoungModulus) = (1 / *YoungModulus);
	(*PoissonRatio) = (1 / *PoissonRatio);

	(*YoungModulus) = (*YoungModulus / width);

	return;
}

void Grid::calcPropertiesAtPoint(char* mat, bool* ivisited, queueNode* currentPoint, double* ielectricConductivities, double* ithermalConductivities, double* iYoungModulus, double* iPoissonRatio)
{
	Direction pathDirectionAtPoint;
	size_t position = (size_t)currentPoint->pt.y * width + currentPoint->pt.x;
	int x, y;
	int material = ingadients[position];
	bool sameline = false;
	double currResistance = ielectricConductivities[material];
	double currThermalResistance = ithermalConductivities[material];
	//double currYoungModulus = iYoungModulus[material] * height;
	//double currPoissonRatio = iPoissonRatio[material] * height;

	currentPoint->resistance = currResistance;
	currentPoint->thermalResistance = currThermalResistance;
	//currentPoint->YoungModulus = currYoungModulus;
	//currentPoint->PoissonRatio = currPoissonRatio;

	x = currentPoint->pt.x;
	y = currentPoint->pt.y;

	if ((currentPoint->pt.x - currentPoint->previous.x) == 0)
		pathDirectionAtPoint = VERTICAL;
	else
		pathDirectionAtPoint = HORIZONTAL;

	do
	{
		if (pathDirectionAtPoint == HORIZONTAL)
			y--;
		else
			x--;

		position = (size_t)y * width + x;
		if (isValid(x, y) && (mat[position] == HARD))
			material = ingadients[position];
		else
			break;

		currResistance = 1.0 / ielectricConductivities[ingadients[position]];
		currThermalResistance = 1.0 / ithermalConductivities[ingadients[position]];
		currentPoint->resistance += currResistance;
		currentPoint->thermalResistance += currThermalResistance;
      /*
		if (pathDirectionAtPoint != HORIZONTAL)
		{
			currYoungModulus = iYoungModulus[ingadients[position]] * height;
			currPoissonRatio = iPoissonRatio[ingadients[position]] * height;
			currentPoint->YoungModulus += currYoungModulus;
			currentPoint->PoissonRatio += currPoissonRatio;
		}
		*/
		sameline = (mat[position] == HARD);
		ivisited[position] = true;

		mat[position] = SIDEPATH;

	} while ((material != 0) && (sameline));

	x = currentPoint->pt.x;
	y = currentPoint->pt.y;

	do
	{
		if (pathDirectionAtPoint == HORIZONTAL)
			y++;
		else
			x++;
		position = (size_t)y * width + x;

		if (isValid(x, y) && (mat[position] != PATH))
			material = ingadients[position];
		else
			break;


		currResistance = 1.0 / ielectricConductivities[ingadients[position]];
		currThermalResistance = 1.0 / ithermalConductivities[ingadients[position]];
		currentPoint->resistance += currResistance;
		currentPoint->thermalResistance += currThermalResistance;
		/*
		if (pathDirectionAtPoint != HORIZONTAL)
		{
			currYoungModulus = iYoungModulus[ingadients[position]] * height;
			currPoissonRatio = iPoissonRatio[ingadients[position]] * height;
			currentPoint->YoungModulus += currYoungModulus;
			currentPoint->PoissonRatio += currPoissonRatio;
		}
		*/
		sameline = (mat[position] == HARD);
		ivisited[position] = true;

		mat[position] = SIDEPATH;




	} while ((material != 0) && sameline);

	currentPoint->resistance = 1.0 / currentPoint->resistance;
	currentPoint->thermalResistance = 1.0 / currentPoint->thermalResistance;
}

/*
Back up 11/10/22
try to solve mechanical
void Grid::calcPropertiesAtPoint(char* mat, bool* ivisited, queueNode* currentPoint, double* ielectricConductivities, double* ithermalConductivities, double* iYoungModulus, double* iPoissonRatio)
{
Direction pathDirectionAtPoint;
size_t position = (size_t)currentPoint->pt.y * width + currentPoint->pt.x;
int x, y;
int material = ingadients[position];

double currResistance = 1/ielectricConductivities[material];
double currThermalResistance = 1/ithermalConductivities[material];
double currYoungModulus = iYoungModulus[material];
double currPoissonRatio = iPoissonRatio[material];

currentPoint->resistance = currResistance;
currentPoint->thermalResistance = currThermalResistance;
currentPoint->YoungModulus = currYoungModulus;
currentPoint->PoissonRatio = currPoissonRatio;

x = currentPoint->pt.x;
y = currentPoint->pt.y;

if ((currentPoint->pt.x - currentPoint->previous.x) == 0)
pathDirectionAtPoint = VERTICAL;
else
pathDirectionAtPoint = HORIZONTAL;

do
{
	if (pathDirectionAtPoint == HORIZONTAL)
		y--;
	else
		x--;

	position = (size_t)y * width + x;
	if (isValid(x, y) && (mat[position] == HARD))
		material = ingadients[position];
	else
		break;

	currResistance = 1.0 / ielectricConductivities[ingadients[position]];
	currThermalResistance = 1.0 / ithermalConductivities[ingadients[position]];
	currentPoint->resistance += currResistance;
	currentPoint->thermalResistance += currThermalResistance;

	if (pathDirectionAtPoint != HORIZONTAL)
	{
		currYoungModulus = iYoungModulus[ingadients[position]];
		currPoissonRatio = iPoissonRatio[ingadients[position]];
		currentPoint->YoungModulus += currYoungModulus;
		currentPoint->PoissonRatio += currPoissonRatio;
	}

	// Debug cout << "--material" << material << "x=" << x << "y=" << y << "\n";

	ivisited[position] = true;
	mat[position] = PATH;

} while (material != 0);

x = currentPoint->pt.x;
y = currentPoint->pt.y;

do
{
	if (pathDirectionAtPoint == HORIZONTAL)
		y++;
	else
		x++;
	position = (size_t)y * width + x;

	if (isValid(x, y) && (mat[position] != PATH))
		material = ingadients[position];
	else
		break;


	currResistance = 1.0 / ielectricConductivities[ingadients[position]];
	currThermalResistance = 1.0 / ithermalConductivities[ingadients[position]];
	currentPoint->resistance += currResistance;
	currentPoint->thermalResistance += currThermalResistance;

	if (pathDirectionAtPoint != HORIZONTAL)
	{
		currYoungModulus = iYoungModulus[ingadients[position]];
		currPoissonRatio = iPoissonRatio[ingadients[position]];
		currentPoint->YoungModulus += currYoungModulus;
		currentPoint->PoissonRatio += currPoissonRatio;
	}
	//debug: cout << "mat[position]=" << mat[position] << "x=" << x << "y=" << y << "\n";
	//debug: cout << "--material" << material << "x=" << x << "y=" << y << "\n";

	ivisited[position] = true;
	mat[position] = PATH;


} while ((material != 0));

currentPoint->resistance = 1.0 / currentPoint->resistance;
currentPoint->thermalResistance = 1.0 / currentPoint->thermalResistance;


}
*/

/*
*
*
*bacu up 16/2/2022
void Grid::calcPropertiesAtPoint(char* mat, bool* ivisited, queueNode* currentPoint, double* ielectricConductivities, double* ithermalConductivities, double* iYoungModulus, double* iPoissonRatio)
{
Direction pathDirectionAtPoint;
size_t position = (size_t)currentPoint->pt.y * width + currentPoint->pt.x;
int x, y;
int material = ingadients[position];

double currResistance = ielectricConductivities[material];
double currThermalResistance = ithermalConductivities[material];
double currYoungModulus = iYoungModulus[material];
double currPoissonRatio = iPoissonRatio[material];

currentPoint->resistance = currResistance;
currentPoint->thermalResistance = currThermalResistance;
currentPoint->YoungModulus = currYoungModulus;
currentPoint->PoissonRatio = currPoissonRatio;

x = currentPoint->pt.x;
y = currentPoint->pt.y;

if ((currentPoint->pt.x - currentPoint->previous.x) == 0)
pathDirectionAtPoint = VERTICAL;
else
pathDirectionAtPoint = HORIZONTAL;

do
{
	if (pathDirectionAtPoint == HORIZONTAL)
		y--;
	else
		x--;

	position = (size_t)y * width + x;
	if (isValid(x, y) && (mat[position] == HARD))
		material = ingadients[position];
	else
		break;

	currResistance = 1.0 / ielectricConductivities[ingadients[position]];
	currThermalResistance = 1.0 / ithermalConductivities[ingadients[position]];


	currentPoint->resistance += currResistance;
	currentPoint->thermalResistance += currThermalResistance;

	if (pathDirectionAtPoint != HORIZONTAL)
	{
		currYoungModulus =  iYoungModulus[ingadients[position]];
		currPoissonRatio =  iPoissonRatio[ingadients[position]];
		currentPoint->YoungModulus += currYoungModulus;
		currentPoint->PoissonRatio += currPoissonRatio;
	}
	// Debug cout << "--material" << material << "x=" << x << "y=" << y << "\n";

	ivisited[position] = true;
	mat[position] = PATH;

} while (material != 0);

x = currentPoint->pt.x;
y = currentPoint->pt.y;

do
{
	if (pathDirectionAtPoint == HORIZONTAL)
		y++;
	else
		x++;
	position = (size_t)y * width + x;

	if (isValid(x, y) && (mat[position] != PATH))
		material = ingadients[position];
	else
		break;


	currResistance = 1.0 / ielectricConductivities[ingadients[position]];
	currThermalResistance = 1.0 / ithermalConductivities[ingadients[position]];


	currentPoint->resistance += currResistance;
	currentPoint->thermalResistance += currThermalResistance;
	if (pathDirectionAtPoint != HORIZONTAL)
	{
		currYoungModulus =  iYoungModulus[ingadients[position]];
		currPoissonRatio =  iPoissonRatio[ingadients[position]];
		currentPoint->YoungModulus += currYoungModulus;
		currentPoint->PoissonRatio += currPoissonRatio;
	}
	//debug: cout << "mat[position]=" << mat[position] << "x=" << x << "y=" << y << "\n";
	//debug: cout << "--material" << material << "x=" << x << "y=" << y << "\n";

	ivisited[position] = true;
	mat[position] = PATH;


} while ((material != 0));

currentPoint->resistance = 1.0 / currentPoint->resistance;
currentPoint->thermalResistance = 1.0 / currentPoint->thermalResistance;


}

void Grid::calcPropertiesAtPoint(char* mat, bool* ivisited, queueNode* currentPoint, double* ielectricConductivities, double* ithermalConductivities, double* iYoungModulus, double* iPoissonRatio)
{
	Direction pathDirectionAtPoint;
	size_t position = (size_t)currentPoint->pt.y * width + currentPoint->pt.x;
	int x, y;
	int material = ingadients[position];

	double currResistance =  ielectricConductivities[material];
	double currThermalResistance = ithermalConductivities[material];
	double currYoungModulus = iYoungModulus[material];
	double currPoissonRatio = iPoissonRatio[material];

	currentPoint->resistance = currResistance;
	currentPoint->thermalResistance = currThermalResistance;
	currentPoint->YoungModulus = currYoungModulus;
	currentPoint->PoissonRatio = currPoissonRatio;

	x = currentPoint->pt.x;
	y = currentPoint->pt.y;

	if ((currentPoint->pt.x - currentPoint->previous.x) == 0)
		pathDirectionAtPoint = VERTICAL;
	else
		pathDirectionAtPoint = HORIZONTAL;

	do
	{
		if (pathDirectionAtPoint == HORIZONTAL)
			y--;
		else
			x--;

		position = (size_t)y * width + x;
		if (isValid(x, y) && (mat[position] == HARD))
			material = ingadients[position];
		else
			break;

		currResistance =  1.0/ielectricConductivities[ingadients[position]];
		currThermalResistance = 1.0/ithermalConductivities[ingadients[position]];
		currYoungModulus = 1.0/iYoungModulus[ingadients[position]];
		currPoissonRatio = 1.0/iPoissonRatio[ingadients[position]];

		currentPoint->resistance += currResistance;
		currentPoint->thermalResistance += currThermalResistance;
		currentPoint->YoungModulus += currYoungModulus;
		currentPoint->PoissonRatio += currPoissonRatio;
		// Debug cout << "--material" << material << "x=" << x << "y=" << y << "\n";

		ivisited[position] = true;
		mat[position] = PATH;

	} while (material != 0) ;

	x = currentPoint->pt.x;
	y = currentPoint->pt.y;

	do
	{
		if (pathDirectionAtPoint == HORIZONTAL)
			y++;
		else
			x++;
		position = (size_t)y * width + x;

		if (isValid(x, y) && (mat[position] != PATH))
			material = ingadients[position];
		else
			break;


		currResistance = 1.0/ielectricConductivities[ingadients[position]];
		currThermalResistance = 1.0/ ithermalConductivities[ingadients[position]];
		currYoungModulus = 1.0/iYoungModulus[ingadients[position]];
		currPoissonRatio = 1.0/iPoissonRatio[ingadients[position]];

		currentPoint->resistance += currResistance;
		currentPoint->thermalResistance += currThermalResistance;
		currentPoint->YoungModulus += currYoungModulus;
		currentPoint->PoissonRatio += currPoissonRatio;

		//debug: cout << "mat[position]=" << mat[position] << "x=" << x << "y=" << y << "\n";
		//debug: cout << "--material" << material << "x=" << x << "y=" << y << "\n";

		ivisited[position] = true;
		mat[position] = PATH;


	} while ((material != 0) );

	currentPoint->resistance = 1.0 / currentPoint->resistance;
	currentPoint->thermalResistance = 1.0 /currentPoint->thermalResistance;
	currentPoint->YoungModulus = 1.0 / currentPoint->YoungModulus;
	currentPoint->PoissonRatio = 1.0 / currentPoint->PoissonRatio;

}*/


void Grid::MarkMinimumPath(char* mat, bool* ivisited, stack<smallQueueNode>* clusterStack, list<smallQueueNode>* listPath)
{

	smallQueueNode last = clusterStack->top();

	clusterStack->pop();
	listPath->push_front(last);
	smallQueueNode currentPoint;

	while (!clusterStack->empty())
	{
		currentPoint = clusterStack->top();
		clusterStack->pop();

		size_t index = ((size_t)currentPoint.pt.y * width) + currentPoint.pt.x;
		ivisited[index] = false;
		mat[index] = HARD;

		if ((last.previous.x == currentPoint.pt.x) && (last.previous.y == currentPoint.pt.y))
		{
			last = currentPoint;
			listPath->push_front(last);

		}

	}
}

/*
backup 7/7/2023
void Grid::MarkMinimumPath(char* mat, bool* ivisited, stack<queueNode>* clusterStack, list<queueNode>* listPath)
{

	queueNode last = clusterStack->top();

	clusterStack->pop();
	listPath->push_front(last);
	queueNode currentPoint;

	while (!clusterStack->empty())
	{
		currentPoint = clusterStack->top();
		clusterStack->pop();

		size_t index = ((size_t)currentPoint.pt.y * width) + currentPoint.pt.x;
		ivisited[index] = false;
		mat[index] = HARD;

		if ((last.previous.x == currentPoint.pt.x) && (last.previous.y == currentPoint.pt.y))
		{
			last = currentPoint;
			listPath->push_front(last);

		}

	}
}

*/


int Grid::drawPath(char* mat, bool* ivisited, list<smallQueueNode> cpath)
{
	std::list<smallQueueNode>::iterator it;

	for (it = cpath.begin(); it != cpath.end(); ++it)
	{

		size_t position = (size_t)it->pt.y * width + it->pt.x;
		mat[position] = PATH;
		ivisited[position] = true;

	}


	return 0;
}


int Grid::drawPaths(char* mat, bool* ivisited)
{
	std::list<std::list<smallQueueNode>>::iterator it;

	for (it = pathsList.begin(); it != pathsList.end(); ++it)
		drawPath(mat, ivisited, *it);


	return 0;
}


int Grid::BFS(char* mat, bool* ivisited, point2d src, int* distance, double* ielectricConductivities, double* resistance, double* ithermalConductivities, double* thermalResistance, double* iYoungModulus, double* YoungModulus, double* iPoissonRatio, double* PoissonRatio, point2d* finalPoint)
{


	*distance = 0;
	*resistance = 0;
	*thermalResistance = 0;
	*YoungModulus = 0;
	*PoissonRatio = 0;

	size_t position = (size_t)src.y * width + src.x;
	if (!mat[position])
	{
		cout << "Warning:Bad source point at BFS\n";
		return false;
	}

	// Mark the source cell as visited
	ivisited[position] = true;

	// Create a queue for BFS
	std::queue<smallQueueNode> q;
	std::stack<smallQueueNode> backup;
	std::list<smallQueueNode> singlePath;

	//int material = ingadients[position];
	//float currResistance = 1. / ielectricConductivities[material];
	//float currThermalResistance = 1. / ithermalConductivities[material];
	//float currYoungModulus = iYoungModulus[material];
	//float currPoissonRatio = iPoissonRatio[material];


	smallQueueNode s = { src, {0,0},0 }; // , currResistance, currThermalResistance, currYoungModulus, currPoissonRatio


	q.push(s); // Enqueue source cell
	smallQueueNode curr;
	// Do a BFS starting from source cell
	while (!q.empty())
	{
		curr = q.front();
		point2d pt = curr.pt;
//		position = (size_t)pt.y * width + pt.x;



		// If we have reached the destination cell,
		// we are done

		if (pt.y >= (height - 1))
		{
			*distance = curr.dist + 1;
			finalPoint->x = pt.x;
			finalPoint->y = pt.y;
			break;
		}

		// Otherwise dequeue the front cell in the queue and enqueue its adjacent cells

		q.pop();


		for (int i = 3; i > -1; i--)
		{
			int x = pt.x + xNum[i];
			int y = pt.y + yNum[i];
			position = (size_t)y * width + x;

			if ((isValid(x, y)) && (mat[position] == PERCOLATE) && (ivisited[position] == false))
			{
				// mark cell as visited and enqueue it

				smallQueueNode Adjcell = { {x, y},{pt.x,pt.y},curr.dist + 1 };// , 0.0, 0.0, 0.0, 0.0


				q.push(Adjcell);
				backup.push(Adjcell);
				ivisited[position] = true;

			}


		}
	}


	this->pathsList.push_front(singlePath);
	MarkMinimumPath(mat, ivisited, &backup, &pathsList.front());
	drawPath(cell, visited, pathsList.front());

	// Return false if destination cannot be reached
	// Debug cout << "fault\n";
	return true;
}

/*
backup 7/7/2023
int Grid::BFS(char* mat, bool* ivisited, point2d src, int* distance, double* ielectricConductivities, double* resistance, double* ithermalConductivities, double* thermalResistance, double* iYoungModulus, double* YoungModulus, double* iPoissonRatio, double* PoissonRatio, point2d* finalPoint)
{


	*distance = 0;
	*resistance = 0;
	*thermalResistance = 0;
	*YoungModulus = 0;
	*PoissonRatio = 0;

	size_t position = (size_t)src.y * width + src.x;
	if (!mat[position])
	{
		cout << "Warning:Bad source point at BFS\n";
		return false;
	}

	// Mark the source cell as visited
	ivisited[position] = true;

	// Create a queue for BFS
	std::queue<queueNode> q;
	std::stack<queueNode> backup;
	std::list<queueNode> singlePath;

	int material = ingadients[position];
	float currResistance = 1. / ielectricConductivities[material];
	float currThermalResistance = 1. / ithermalConductivities[material];
	float currYoungModulus = iYoungModulus[material];
	float currPoissonRatio = iPoissonRatio[material];


	queueNode s = { src, {0,0},0,currResistance,currThermalResistance, currYoungModulus, currPoissonRatio };

	q.push(s); // Enqueue source cell
	queueNode curr;
	// Do a BFS starting from source cell
	while (!q.empty())
	{
		curr = q.front();
		point2d pt = curr.pt;
//		position = (size_t)pt.y * width + pt.x;



		// If we have reached the destination cell,
		// we are done

		if (pt.y >= (height - 1))
		{
			*distance = curr.dist + 1;
			finalPoint->x = pt.x;
			finalPoint->y = pt.y;
			break;
		}

		// Otherwise dequeue the front cell in the queue and enqueue its adjacent cells

		q.pop();


		for (int i = 3; i > -1; i--)
		{
			int x = pt.x + xNum[i];
			int y = pt.y + yNum[i];
			position = (size_t)y * width + x;

			if ((isValid(x, y)) && (mat[position] == PERCOLATE) && (ivisited[position] == false))
			{
				// mark cell as visited and enqueue it

				queueNode Adjcell = { {x, y},{pt.x,pt.y},curr.dist + 1,0.0,0.0,0.0,0.0 };

				q.push(Adjcell);
				backup.push(Adjcell);
				ivisited[position] = true;

			}


		}
	}


	this->pathsList.push_front(singlePath);
	MarkMinimumPath(mat, ivisited, &backup, &pathsList.front());
	drawPath(cell, visited, pathsList.front());

	// Return false if destination cannot be reached
	// Debug cout << "fault\n";
	return true;
}

*/


int Grid::clusterCenter(point2d src, doublepoint2d* center, size_t* totalPoints)
{

	size_t id = (size_t)(src.y) * width + src.x;
	point2d s;
	double centerX = 0;
	double centerY = 0;
	size_t totalP = 0;


	queue<point2d> q;

	//cluster[id] = color;
	// Mark the source cell as visited

	clusterVisited[id] = true;

	q.push(src); // Enqueue source cell

	// Do a BFS starting from source cell
	while (!q.empty())
	{

		point2d pt = q.front();
		totalP++;

		centerX = centerX + pt.x;
		centerY = centerY + pt.y;

		q.pop();

		for (int i = 0; i < 4; i++)
		{
			int x = pt.x + xNum[i];
			int y = pt.y + yNum[i];

			id = (size_t)(y) * width + x;

			if ((isValid(x, y) && ((cell[id] == CellState::HARD) || (cell[id] == PERCOLATE) || (cell[id] == CellState::SOFT)) && (clusterVisited[id] == false)))
			{
				clusterVisited[id] = true;
				q.push({ x,y });
			}

		}
	}

	center->x = centerX / totalP;
	center->y = centerY / totalP;
	*totalPoints = totalP;

	return 0;
}


//
// calculate mean Radius
// of giration of a cluster
//

int Grid::clusterInertia(char* mat, unsigned char* clustersImage, point2d src, doublepoint2d center, size_t totalPoints, double* inertia, double* radiusOfGyration)
{
//	size_t totalP = 0;
	double sum = 0;
//	double sqrtSum = 0;
	int x, y;

	double distance;
	queue<point2d> q;
	char color = rand() % 0xC0 + 0x2F;
	size_t id = (size_t)src.y * width + src.x;

	// Mark the source cell as visited

	clusterVisited[id] = false;
	clustersImage[id] = color;

	//initialize

	//start points is src
	q.push(src); // Enqueue source cell


	// Do a BFS starting from source cell
	while (!q.empty())
	{

		point2d pt = q.front();  //take the front point
		//totalP++;				// increase one total points
		q.pop();				// take of front point

		distance = ((center.x - pt.x) * (center.x - pt.x)) + ((center.y - pt.y) * (center.y - pt.y));

		sum = sum + distance;
		//		sqrtSum += sqrt(distance);

		for (int i = 0; i < 4; i++)
		{
			x = pt.x + xNum[i];
			y = pt.y + yNum[i];

			id = (size_t)(y) * width + x;

			if ((isValid(x, y) && (clusterVisited[id] == true)))
			{

				clustersImage[id] = color;
				clusterVisited[id] = false;

				q.push({ x,y });
			}
		}
	}


	//Mean radius  of cluster
	*inertia = sum;
	*radiusOfGyration = sqrt(sum / totalPoints);

	return 0;
}


Cluster Grid::markClusters()
{
	int i, j;

	//	int sumClusters = 1;
	size_t id;
	doublepoint2d center(0.0, 0.0);
	Cluster currCluster;
	point2d src;

//	double meanDistance = 0;

	queue<Cluster> cClusters;
	double maxRadius = 0;



	for (i = 0; i < width; i++)
	{
		for (j = 0; j < height; j++)
		{
			src.x = i;
			src.y = j;
			id = (size_t) int(j) * width + i;

			if ((cell[id] == CellState::HARD) && (clusterVisited[id] == false))
			{

				clusterCenter(src, &currCluster.center, &currCluster.totalPoints);

				currCluster.start = src;
				cClusters.push(currCluster);


			}

		}

	}

	//

	//#pragma omp parallel for



	while (!cClusters.empty())
	{

		currCluster = cClusters.front();
		cClusters.pop();
		clusterInertia(cell, cluster, currCluster.start, currCluster.center, currCluster.totalPoints, &currCluster.inertia, &currCluster.radius);

		Clusters.push_back({ currCluster.radius,currCluster.inertia,currCluster.totalPoints });



		if (currCluster.radius > maxRadius)
			maxRadius = currCluster.radius;



//		int j = 0;
	}


	cout << "Max Radius = " << maxRadius << "\n";
	cMaxClusterRadius.push_back(maxRadius);
	//cradiousTotals.totals = totalClusters;*/
	return currCluster;

}



double Grid::findCorrelationLength(std::vector<clusterStatistics> iClusters)
{
	double numerator = 0;
	double denominator = 0;
	double weight = 0;

	for (size_t i = 0; i < iClusters.size(); i++)
		//if (iClusters.size()>10)
	{
		weight = double(iClusters[i].totalPoints) * double(iClusters[i].totalPoints);
		numerator = numerator + ((iClusters[i].radius * iClusters[i].radius) * weight);
		denominator = denominator + weight;
	}

	//if (denominator != 0)
	return (double)(numerator / denominator);
	// else
	//	return 0.0;
}


int  Grid::percolate(void)
{
	int iter;
//	int perc = false;
	int step = width / 8;
	int result[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };

	for (iter = 0; iter < width / 8; iter++)
	{

#pragma omp parallel sections
		{
#pragma omp section
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

		if ((result[0] > -1)
			|| (result[1] > -1)
			|| (result[2] > -1)
			|| (result[3] > -1)
			|| (result[4] > -1)
			|| (result[5] > -1)
			|| (result[6] > -1)
			|| (result[7] > -1)) break;
	}


	if (iter < (width / 8) - 1)
		memset(end, PERCOLATE, width);


	return  iter < (width / 8);



}


int Grid::percolate(double* proccessTime)
{
	int result;
	clock_t istart = clock();
	result = percolate();
	clock_t iend = clock();
	*proccessTime = ((double)(iend - istart)) / CLOCKS_PER_SEC;
	return result;
}


int  Grid::percolateWithRealPathLength(double* totalPaths, double* meanLength, double* meanCalculatedLength, double* ielectricConductivities, double* ithermalConductivities, double* iYoungModulus, double* iPoissonRatio, double* meanRVEResistance, double* meanRVEThermalResistance, double* meanRVEYoungModulus, double* MeanRVEPoissonRatio)
{
	int i;

//	double sumCalculatedLength = 0;

	point2dShort startPoint;
	point2d endPoint = { 0,0 };

	//Dimension 4 for parallel proccessing}

	double pelectricResistance[4] = { 0,0,0,0 };
	double pthermalResistance[4] = { 0,0,0,0 };
	double pYoungModulus[4] = { 0,0,0,0 };
	double pPoissonRatio[4] = { 0,0,0,0 };
	int realLength[4] = { 0,0,0,0 };
	int pathsLength[4] = { 0,0,0,0 };


	double sumResistance = 0;
	double sumThermalResistance = 0;
	//double sumYoungModulus = 0;
	//double sumPoissonRatio = 0;
	double sumRealLength = 0;
	

	*totalPaths = 0;
	*meanLength = 0;
	*meanCalculatedLength = 0;

	bool isPercolate = false;


	//#pragma omp parallel for
	for (i = 0; i < width; i++)
	{
		//Warning Parallel version caused problems
		//#pragma omp parallel shared(totalPaths,sumCalculatedLength,sumResistance,sumThermalResistance)
		//#pragma omp sections

			//#pragma omp section

				//pathLength = 0;
		isPercolate = false;
		startPoint = { (short)i,(short)0 };
		if (cell[i] == CellState::HARD || cell[i] == CellState::SOFT)
			isPercolate = DFS(cell, visited, startPoint, &endPoint, &pathsLength[0]);


		if (isPercolate)
		{

			//debug: cout <<"start=" <<i<< "," << "0" << "end=" << endPoint.x << "," << endPoint.y << "\n";
			BFS(cell, visited, { i,0 }, &realLength[0], ielectricConductivities, &pelectricResistance[0], ithermalConductivities, &pthermalResistance[0], iYoungModulus, &pYoungModulus[0], iPoissonRatio, &pPoissonRatio[0], &endPoint);
			*totalPaths = *totalPaths + 1;
			sumRealLength = sumRealLength + realLength[0];


			//debug:	cout << "Paths:" << *totalPaths << " Total function hoops:" << pathsLength[0] << " Real Path Length:" << realLength[0] << " Path resistance:" << pelectricResistance[0] << " Thermal resistance:" << pthermalResistance[0] << "\n";
		}




		/*
		//#pragma omp section
						{
							//pathLength = 0;
							//startPoint = { i,0,k };
							//startPoint = { i,0,k };
							if (cell[(k + (depth / 4))* width + i] == CellState::HARD)
								if (DFS(cell, visited, { (short)i,(short)0,(short)(k + (depth / 4)) }, &endPoint, &pathsLength[1]))
								{
									cout << " From " << i << " " << 0 << " " << k << " To " << endPoint.x << " " << endPoint.y << " " << endPoint.z << "\n";
									//std::fill_n(visited, total, false);
									BFS(cell, visited, { i,0,k + depth / 4 }, &realLength[1], ielectricConductivities, &resistance[1], ithermalConductivities, &thermalResistance[1], &endPoint);
									*totalPaths = *totalPaths + 1;
									*meanLength = *meanLength + pathLength;

									sumCalculatedLength += realLength[1];
									sumResistance += resistance[1];
									sumThermalResistance += thermalResistance[1];
									// Debug cout << "Path:" << *totalPaths << " Total function hoops:" << pathsLength[1] << " Real Path Length:" << realLength[1];
									// Debug cout<< " Path resistance:" << resistance[1] << " Thermal resistance:" << thermalResistance[1] << "\n";
								}

						}
		//#pragma omp section
						{
							//pathLength = 0;
							//startPoint = { i,0,k };
							if (cell[(k + (2 * depth / 4))* width + i] == CellState::HARD)
								if (DFS(cell, visited, { (short)i,(short)0,(short)(k + (2 * depth / 4)) }, &endPoint, &pathsLength[2]))
								{
									cout << " From " << i << " " << 0 << " " << k << " To " << endPoint.x << " " << endPoint.y << " " << endPoint.z << "\n";
									//std::fill_n(visited, total, false);
									BFS(cell, visited, { i,0,k + (2 * depth / 4) }, &realLength[2], ielectricConductivities, &resistance[2], ithermalConductivities, &thermalResistance[2] ,&endPoint);
									*totalPaths = *totalPaths + 1;
									*meanLength = *meanLength + pathLength;

									sumCalculatedLength += realLength[2];
									sumResistance += resistance[2];
									sumThermalResistance += thermalResistance[2];
									// Debug cout << "Path:" << *totalPaths << " Total function hoops:" << pathsLength[2] << " Real Path Length:" << realLength[2] << " Path resistance:" << resistance[2] << " Thermal resistance:" << thermalResistance[2] << "\n";
								}

						}
		//#pragma omp section
						{
							//pathLength = 0;
							//startPoint = { i,0,k };

							if (cell[(k + (3 * depth / 4))* width + i] == CellState::HARD)
								if (DFS(cell, visited, { (short)i,(short)0,(short)(k + (3 * depth / 4)) }, &endPoint, &pathsLength[3]))
								{
									cout << " From " << i << " " << 0 << " " << k << " To " << endPoint.x << " " << endPoint.y << " " << endPoint.z << "\n";
									//std::fill_n(visited, total, false);
									BFS(cell, visited, { i,0,k + (3 * depth / 4) }, &realLength[3], ielectricConductivities, &resistance[3], ithermalConductivities, &thermalResistance[3],&endPoint);
									*totalPaths = *totalPaths + 1;
									*meanLength = *meanLength + pathLength;

									sumCalculatedLength += realLength[3];
									sumResistance += resistance[3];
									sumThermalResistance += thermalResistance[3];
									// Debug cout << "Path:" << *totalPaths << " Total function hoops:" << pathsLength[3] << " Real Path Length:" << realLength[3] << " Path resistance:" << resistance[3] << " Thermal resistance:" << thermalResistance[3] << "\n";
								}

						}*/


	}

	sumResistance = 0;
	std::list<std::list<smallQueueNode>>::iterator it;
	for (it = pathsList.begin(); it != pathsList.end(); ++it)
	{
		CalcPropetriesAtPath(cell, visited, *it, pelectricResistance, ielectricConductivities, pthermalResistance, ithermalConductivities,
			pYoungModulus, iYoungModulus, pPoissonRatio, iPoissonRatio);


		sumResistance += 1 / pelectricResistance[0];
		sumThermalResistance += 1 / pthermalResistance[0];
		//sumYoungModulus += pYoungModulus[0];
		//sumPoissonRatio += pPoissonRatio[0];
		//debug: cout << "sumResistance=" << sumResistance << "\n";

	}



	if ((*totalPaths) > 0)
	{
		memset(end, PERCOLATE, width);

		(*meanLength) = (sumRealLength) / (*totalPaths);
		(*meanCalculatedLength) = (sumRealLength) / (*totalPaths);

		(*meanRVEResistance) = 1 / sumResistance;
		(*meanRVEThermalResistance) = 1 / sumThermalResistance;
		double meanRVEPathWidth = (sumResistance / ielectricConductivities[1]) * (*meanLength / (*totalPaths));
		(*meanRVEYoungModulus) = pow((iYoungModulus[1] * (*totalPaths) * meanRVEPathWidth), (width / (*meanCalculatedLength))) + (iYoungModulus[0] * (width - ((*totalPaths) * meanRVEPathWidth)) / width);
		(*MeanRVEPoissonRatio) = (iPoissonRatio[1] * (*totalPaths) * meanRVEPathWidth / width) + (iPoissonRatio[0] * (width - ((*totalPaths) * meanRVEPathWidth)) / width);


		//(*MeanRVEPoissonRatio) = sumPoissonRatio;

		//(*meanRVEYoungModulus) = sumYoungModulus;//(sumYoungModulus * (*totalPaths) + (width - (*totalPaths)) * iYoungModulus[0]) / width;       //sumYoungModulus; // / (*totalPaths);

	}
	else {
		(*meanLength) = 0;
		*meanCalculatedLength = 0;
		*meanRVEResistance = 0;
		*meanRVEThermalResistance = 0;
		*meanRVEYoungModulus = iYoungModulus[0];
		*MeanRVEPoissonRatio = iPoissonRatio[0];
	}



	pathsList.clear();
	return (*totalPaths > 0);
}

int  Grid::percolateWithRealPathLength(double* totalpaths, double* meanlength, double* meanRealLength, double* proccessTime, double* ielectricConductivities, double* omeanRVEResistance, double* ithermalConductivities, double* omeanRVEThermalResistance, double* iYoungModulus, double* omeanRVEYoungModulus, double* iPoissonRatio, double* omeanRVEPoissonRatio)
{
	int result;
	clock_t istart = clock();
	//int paths;
	result = percolateWithRealPathLength(totalpaths, meanlength, meanRealLength, ielectricConductivities, ithermalConductivities, iYoungModulus, iPoissonRatio, omeanRVEResistance, omeanRVEThermalResistance, omeanRVEYoungModulus, omeanRVEPoissonRatio);

	clock_t iend = clock();
	*proccessTime = ((double)(iend - istart)) / CLOCKS_PER_SEC;
	cout << "mean Paths per RVE=" << *totalpaths << "\n";
	cout << "mean RVE electric resistance=" << *omeanRVEResistance << "\n";
	cout << "mean RVE Thermal resistance=" << *omeanRVEThermalResistance << "\n";
	cout << "mean RVE Young Modulus=" << *omeanRVEYoungModulus << "\n";
	cout << "mean RVE Poissons Ratio=" << *omeanRVEPoissonRatio << "\n";

	return result;
}

//show on text mode only for debug proposes
void  Grid::show() const
{
	cout << "\n";

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			std::cout << cell[i * width + j];
		}
		cout << "\n";
	}

	cout << "\n";

}

void  Grid::saveToDisk(char* imageFileName, bool saveAsBMP)
{

	if (saveAsBMP)
	{
		generateBitmapImage(cell, height, width, imageFileName);
	}
	else
	{
		generatePGMImage(cell, height, width, imageFileName);
	}
}



void  Grid::saveToDisk(char* path, char* imageFileName, bool saveAsBMP)
{
	char* filename{ new char[strlen(path) + strlen(imageFileName) + 1] };

	strcpy(filename, path);
	strcat(filename,  imageFileName);
	saveToDisk(filename, saveAsBMP);
}




Grid::~Grid() {
	delete[] start;
	delete[] ingadients;
	delete[] visited;
	delete[] clusterVisited;
	delete[] cluster;

	Clusters.~vector();
	cMaxClusterRadius.~vector();


	start = 0;
	end = 0;
}
