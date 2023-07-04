/*This file is part of Electric Percolation (eperc2d) program.

Created from Lefteris Lamprou lef27064@otenet.gr during PhD thesis

eperc2d is free SOFTware : you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free SOFTware Foundation, either version 3 of the License, or
(at your option) any later version.

eperc2d is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Foobar.If not, see < https://www.gnu.org/licenses/>.
*/

#pragma once

#include <stdio.h> 
#include <chrono>
#include <iostream>     
#include <fstream>   
#include <string>
#include <queue>

using namespace std;
//To store matrix cell cordinates
struct point2d
{
	int x;
	int y;
	point2d(int a, int b);
	point2d(void);
	
	
};

//To store matrix cell cordinates
struct doublepoint2d
{
	double x;
	double y;
	doublepoint2d(double a, double b);
	doublepoint2d(void);
	
};

struct point2dShort
{
	short int x;
	short int y;
	short int z;

};

//To store matrix cell cordinates
struct Cluster
{
	double radius;
	doublepoint2d center;
	point2d start;
	long long int totalPoints;
	double inertia;

	//Cluster(double a);
	//Cluster(double a, double b, double c);
	Cluster(void);

};


struct clusterStatistics
{
	double radius;
	double inertia;
	long long int totalPoints;
};

/*
class Cluster {

public:

	int id;
	int totalPoints;
	doublepoint2d center;
	point2d start;
	double meanRadius = 0;
	double sumRi_mi = 0;

	
	double getMeanRadius(void) 	{return (meanRadius * totalPoints);};
	
	

	Cluster(int startX, int startY);
	

	//Cluster();

	Cluster(int startX, int startY, double centerX, double centerY);

	Cluster(point2d istart, doublepoint2d icenter);
	

	~Cluster();
};


class MeanCluster
{
public:
	double getMeanWeightedRadius(void);
	double getMeanRadius(void);
	double getMeanCluster(void);
	double getMaxRadius(void);
	int total;
	double meanClusters;
	queue<radiusTotals> SampleMeanRadius;
	MeanCluster();
	~MeanCluster();
	double meanRadius;
	double maxRadius;
	double meanWeightedRadius;
	void addMeanRadius(radiusTotals cRadiusTotals);
	

private:
*/

