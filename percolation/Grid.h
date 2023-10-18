/*This file is part of Electric Percolation (eperc2d) program.

Created from Lefteris Lamprou lef27064@otenet.gr during PhD thesis

eperc2d is free software : you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free software Foundation, either version 3 of the License, or
(at your option) any later version.

eperc2d is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Foobar.If not, see < https://www.gnu.org/licenses/>.
*/

#pragma once
#include <assert.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h> 
#include <time.h>
#include <chrono>
#include <queue>
#include <stack>
#include <list>
#include "Shapes.h"
#include "image.h"
#include "general.h"
#include "cluster.h"
#include <omp.h>

#define maxDimension 30000

struct nodeWithState
{
	point2d pt;			// The cordinates of a cell
	point2d previous;
	int dist;			// cell's distance  from the source
	float resistance;	// cell's resistance from the source
	float thermalResistance;
	float YoungModulus;
	float PoissonRatio;
	char state;
};


struct smallQueueNode
{
	point2d pt;			// The cordinates of a cell
	point2d previous;
	int dist;			// cell's distance  from the source
};

struct queueNode: smallQueueNode {
	//point2d pt;			// The cordinates of a cell
	//point2d previous;
	//int dist;			    // cell's distance  from the source
	
	float resistance;	// cell's resistance from the source
	float thermalResistance;
	float YoungModulus;
	float PoissonRatio;

};

class Grid {
	bool isValid(int x, int y);
	int floodFill(char* mat, bool* ivisited, point2d src);

	
	int percolate(void);
	long long total;
public:
	list<list<smallQueueNode>> pathsList;
	char* cell, * start, * end;
	unsigned char* cluster;
	int totalClusters = 0;
	int width, height;
	unsigned char* ingadients;
	bool* visited;
	bool* clusterVisited;

	//init
	Grid(int x, int y);

	void clear(void);
	double VerticalYoungModulus[maxDimension];
	vector<clusterStatistics> Clusters;

	vector<double> cMaxClusterRadius;


	//read
	char get(int line, int column);
	void inverse(void);
	int countArea();
	void set(int line, int column, char what);

	void set(int x, int y, int what);


	void set(unsigned char* data, int x, int y, unsigned char what);

	unsigned char get(unsigned char* data, int x, int y);

	//Rectangle cellCoordinates(int x, int y);
	iPoint toLocaldimensions(double x, double y);
	iPoint toLocaldimensionsUp(double x, double y);
	iPoint toLocaldimensionsDown(double x, double y);

	Range fromRectangleToRange(double x1, double y1, double x2, double y2);


	bool isClearSlopedRectangle(int x, int y, int dx, int dy, double slope);
	bool isClearEllipse(int x, int y, int a, int b, double slope);

	
	int DFS(char* mat, bool* ivisited, point2dShort src, point2d* target, int* distance);

	long long int countPathPixels(char* mat);

	int DFSRestore(char* mat, bool* ivisited, point2dShort src);

	
	void CalcPropetriesAtPath(char* mat, bool* ivisited, list<smallQueueNode> path, double* resistance, double* ielectricConductivities, double* thermalResistance, double* ithermalConductivities, double* YoungModulus, double* iYoungModulus, double* PoissonRatio, double* iPoissonRatio);

	void calcPropertiesAtPoint(char* mat, bool* ivisited, queueNode* currentPoint, double* ielectricConductivities, double* ithermalConductivities, double* iYoungModulus, double* iPoissonRatio);

	
	void MarkMinimumPath(char* mat, bool* ivisited, stack<smallQueueNode>* clusterStack, list<smallQueueNode>* listPath);

	
	int drawPath(char* mat, bool* ivisited, list<smallQueueNode> cpath);
	
	int drawPaths(char* mat, bool* ivisited);

	int BFS(char* mat, bool* ivisited, point2d src, int* distance, double* ielectricConductivities, double* resistance, double* ithermalConductivities, double* thermalResistance, double* iYoungModulus, double* YoungModulus, double* iPoissonRatio, double* PoissonRatio, point2d* finalPoint);

	int clusterCenter(point2d src, doublepoint2d* center, long long int* totalPoints);

	int clusterInertia(char* mat, unsigned char* clustersImage, point2d src, doublepoint2d center, long long int totalPoints, double* inertia, double* radiusOfGyration);

	Cluster markClusters();

	double findCorrelationLength(std::vector<clusterStatistics> iClusters);

	int countPixelsInsidelopedRectangeMultiple(int x, int y, int dx, int dy, double slope, CellState state);

	int countPixelsInsideEllipseMultiple(int x, int y, int a, int b, double slope, CellState state);


	int countPixelsInsidelopedRectange(int x, int y, int dx, int dy, double slope, CellState state);
	int countPixelsInsideEllipse(int x, int y, int a, int b, double slope, CellState state);
		
	int percolate(double* proccessTime);
	int percolateWithRealPathLength(double* totalPaths, double* meanLength, double* meanCalculatedLength, double* ielectricConductivities, double* ithermalConductivities, double* iYoungModulus, double* iPoissonRatio, double* meanResistance, double* meanThermalResistance, double* meanYoungModulus, double* MeanPoissonRatio);
	int percolateWithRealPathLength(double* totalpaths, double* meanlength, double* meanRealLength, double* proccessTime, double* ielectricConductivities, double* omeanResistance, double* ithermalConductivities, double* omeanThermalResistance, double* iYoungModulus, double* omeanYoungModulus, double* iPoissonRatio, double* omeanPoissonRatio);
	void show() const;

	void saveToDisk(char* imageFileName, bool saveAsBMP);
	void saveToDisk(char* path, char* imageFileName, bool saveAsBMP);
	~Grid();
};
