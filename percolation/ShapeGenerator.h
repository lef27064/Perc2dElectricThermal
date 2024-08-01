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
//#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <cmath>
#include <fstream>
#include <string>
#include <random>
#include <limits>
#include <algorithm>
#include <time.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <omp.h>
#include <filesystem>

#include "Shapes.h"
#include "Grid.h"
#include "Settings.h"
#include "FD2DEL.h"



using namespace std;


#define maxComponents   20
#define maxCases		5000


class ShapeGenerator {
private:
	std::mt19937 eng; // object which produces random bits
	std::random_device r;
	std::seed_seq seed;
	/*std::uniform_int_distribution<int> distx;
	std::uniform_int_distribution<int> disty;
	std::uniform_int_distribution<int> distangle;
	std::uniform_int_distribution<int> distsize;*/
public:
	string projectName;
	string projectDir;
	bool swissCheese=true;
	int iterations=0;
	bool calcElectricConductivity = false;
	bool calcElectricConductivityWithFDM = false;
	bool calcStatistcs = false;

	int width=0;
	int height=0;
	int totalComponents=0;
	double realComponentAreas[maxComponents*maxCases] = {};

	double components[maxComponents] = {};			// % weights
	double specialWeights[maxComponents] = {};		// % special Weights
	double dimensionX[maxComponents] = {};			// Dimension x of Shape
	double dimensionY[maxComponents] = {};			// Simension y of Shape
	ShapeType componentsType[maxComponents] = {};    // type of components
	SizeType  componentsSizeType[maxComponents] = {};
	double componentsArea[maxComponents] = {};       // area of components
	double hoops[maxComponents] = {};				// Hoops distance;
	double materialsElectricConductivity[maxComponents] = {}; // Electric Conductivities
	double materialsThermalConductivity[maxComponents] = {};
	double materialsYoungModulus[maxComponents] = {};
	double materialsPoissonRatio[maxComponents] = {};

	Grid* grid;
	int Results[maxCases] = {};
	float FDResults[maxCases * 2] = {};
	double Times[maxCases] = {};
	double setUpTimes[maxCases] = {};
	double calcComponents[maxComponents] = {};
	double paths[maxCases] = {};
	double meanPathLength[maxCases] = {};
	double meanRealPathLength[maxCases] = {};

	double electricConductivity[maxCases] = {};
	double thermalConductivities[maxCases] = {};
	double YoungModulus[maxCases] = {};
	double PoissonRatio[maxCases] = {};

	double meanRVEResistances[maxCases] = {};
	double meanRVEThermalResistance[maxCases] = {};
	double meanRVEYoungModulus[maxCases] = {};
	double meanRVEPoissoonRatio[maxCases] = {};

	double meanElectricConductivity=0;
	double meanThermalConductivity=0;
	double meanYoungModulus=0;
	double meanPoissonRatio=0;
	//double meanYoungModulusInLayers=0;

	int pixelsPerMinimumCircle;
	double min=0;
	double max=0;
	double factor=0;

	double meanPercolation=0;
	double meanTime=0;
	double meanSetUpTime=0;
	Settings* settings;
	vector<double> correleationLengths;
	double correleationLength=0;

	double thresh=0;
public:
	void initDirs(void);
	//initialize
	ShapeGenerator(int tcomp, double comp[], ShapeType compType[], SizeType compSizeType[], double sWeights[], double dimX[], double dimY[], double ihoops[], Grid* iGrid, Settings* settings);
	ShapeGenerator();

	void areaSolve(void);

	bool isInsideGrid(Point cPoint, Grid * iGrid);

	bool isInsideGrid(iPoint cPoint, Grid * iGrid);


	std::vector<std::string> split(std::string strToSplit, char delimeter);

	void scale(Point orig, Point* scaled, double scaleX, double scaleY);
	void setupCaseLattice(int caseNo, double* setUpTime);
	void setupCase(int caseNo, double* setUpTime);

	SlopedRectangle addOneSlopedRectangle(int caseNo, int i, std::normal_distribution<double> size, int * totalRectangles, int * totalSlopedRectangles, int * totalEllipses, int * totalCircles, double * hoop);

	Ellipse addOneEllipse(int caseNo, int ingradient, std::normal_distribution<double> size, int* totalRectangles, int* totalSlopedRectangles, int* totalEllipses, int* totalCircles, double* hoop);

	void SetupSizes(int i, double dimensionY, double dimensionX, double factor, std::normal_distribution<double> size, SizeType componentsSizeType, double * rectWidth, double * rectHeight, ShapeType componentsType, double * maxAngle, double * minAngle, int * totalSlopedRectangles, int * totalRectangles, int * totalCircles, int * totalEllipses);
	void swissCheeseCheckAndDraw(int ingradient, double a, double b, Ellipse* cEllipse, double hoop);

	void swissCheeseCheckAndDraw(int ingradient, double rectWidth, double rectHeight, SlopedRectangle* sRectangle, double hoop);
	void monteCarlo(void);
	void calcMeanCorrellationLength();
	void Report();
	void ReportWithSemicolon();
	void ReportStatistics(void);
	
	SlopedRectangle generateSlopedRectangle(Point downleft, Point upRight, double width, double height, double maxAngle, double minAngle);

	Ellipse generateEllipse(Point downleft, Point upRight, double ia, double ib, double maxAngle, double minAngle);
	void digitizeSlopedRectangle(int ingradient, SlopedRectangle sRect, Grid* iGrid, CellState state);

	void digitizeSlopedRectangle(int ingradient, SlopedRectangle sRect, Grid* iGrid, double* realArea, CellState state);
	void digitizeSlopedRectangleWithBorder(int ingradient, SlopedRectangle sRect, Grid* iGrid, double* realArea, CellState state);

	void digitizeEllipse(int ingradient, Ellipse iEllipse, Grid * iGrid, double * realArea, CellState state);

	void digitizeEllipse(int ingradient, Ellipse iEllipse, Grid* iGrid, CellState state);
	void digitizeEllipseWithBorder(Ellipse iEllipse, Grid * iGrid, double * realArea, CellState state);


	void digitizeEllipseDiv4(int ingradient, Ellipse iEllipse, Grid* iGrid, double* realArea, CellState state, bool x, bool y);

	void readFromFile(char* inputFileName);
	void exportForFDM(char* fileName);


};




