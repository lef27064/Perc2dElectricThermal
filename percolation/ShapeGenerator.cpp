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


#include "ShapeGenerator.h"

ShapeGenerator::ShapeGenerator()
{
	totalComponents = 0;
	iterations = 400;
	std::fill_n(PoissonRatio, maxCases, 0);
	std::fill_n(Results, maxCases, 0);
	std::fill_n(Times, maxCases, 0.0);
	std::fill_n(YoungModulus, maxCases, 0.0);
	std::fill_n(componentsArea, maxCases, 0.0);
	
}
/*
ShapeGenerator::~ShapeGenerator()
{
}*/

void ShapeGenerator::initDirs(void)
{
	//c++17
	string iprojectDir = "./" + projectName;
	string ishapesDir = iprojectDir + "/shapes/";
	string iimagesDir = iprojectDir + "/images/";
	


	if (!std::filesystem::exists(iprojectDir))
			std::filesystem::create_directory(iprojectDir);
	
	if (!std::filesystem::exists(ishapesDir))
		std::filesystem::create_directory(ishapesDir);

	if (!std::filesystem::exists(iimagesDir))
		std::filesystem::create_directory(iimagesDir);
	

	/*
	* before c++17 compiler
	string iprojectDir = "./" + projectName;
	string ishapesDir = iprojectDir + "/shapes/";
	string iimagesDir = iprojectDir + "/images/";

	char* cprojectDir = &iprojectDir[0u];
	char* cshapesDir = &ishapesDir[0u];
	char* cimagesDir = &iimagesDir[0u];

	int result = 0 ;
	


	if (!dirExists(cprojectDir))
	{
		
		result = mkdir(cprojectDir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		if (result != 0)
		{
			cout << "Error create directory " << projectDir;
			exit(-1);
		}
		
	}

	if (!dirExists(cshapesDir))
	{
		result = mkdir(cshapesDir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		if (result != 0)
		{
			cout << "Error create directory " << ishapesDir;
			exit(-1);
		}

	}

	if (!dirExists(cimagesDir))
	{
		result = mkdir(cimagesDir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		if (result != 0)
		{
			cout << "Error create directory " << iimagesDir;
			exit(-1);
		}
	}
	
	return;
	*/
}

ShapeGenerator::ShapeGenerator(int tcomp, double comp[], ShapeType compType[], SizeType compSizeType[], double sWeights[], double dimX[], double dimY[], double ihoops[], Grid* iGrid, Settings* isettings) : totalComponents(tcomp), grid(iGrid), settings(isettings)
{
	for (int i = 0; i < totalComponents; i++)
	{
		components[i] = comp[i];
		componentsType[i] = compType[i];
		specialWeights[i] = sWeights[i];
		dimensionX[i] = dimX[i];
		dimensionY[i] = dimY[i];
		componentsArea[i] = comp[i];
		hoops[i] = ihoops[i];
		componentsSizeType[i] = compSizeType[i];

	}



}

void ShapeGenerator::areaSolve(void)
{

	double sum;
	double sumError;
	double epsilon = 1.e-10;

//	double maxError = 0;
//	double totalWeight = 0;
	double sumAreas = 0;
//	int whereMaxError = 0;
	int iter = 0;

	//initialize
	for (int i = 0; i < totalComponents; i++)
		componentsArea[i] = components[i];

	do
	{
		iter++;
		sum = 0;
//		maxError = 0;

		//compute ÓAi*ñi 
		for (int i = 0; i < totalComponents; i++)
		{
			sum = sum + components[i] * specialWeights[i];

		}

//		maxError = 0;

		//calculate  current Wi = Ai*ñi/ ÓAj*ñj and compute errors
		//
		sumError = 0;
		for (int i = 0; i < totalComponents; i++)
		{
			calcComponents[i] = (componentsArea[i] * specialWeights[i]) / sum;
			sumError = sumError + abs(calcComponents[i] - components[i]);
			// debug cout <<"COMPONENT["<<i<<"] Area="<< componentsArea[i] <<" Error="<< calcComponents[i] - components[i] <<"\n";

		}
		sumAreas = 0;
		//find 	 Ai = W * ÓAj*ñj		
		for (int i = 0; i < totalComponents; i++)
		{
			componentsArea[i] = components[i] * sum / specialWeights[i];
			sumAreas += componentsArea[i];
		}


	} while (sumError > epsilon);
	cout << "Solve from % Weights to % Area\n";
	//print some information

	for (int i = 0; i < totalComponents; i++)
	{
		componentsArea[i] = componentsArea[i] / sumAreas;
		cout << "COMPONENT[" << i << "] % Area=" << componentsArea[i] << " Error=" << calcComponents[i] - components[i] << " Weights%=" << components[i] << "\n";
	}
	cout << "-----------------------------------------------------------------------------------------\n";
}



//Is point inside Grid
bool ShapeGenerator::isInsideGrid(Point cPoint, Grid* iGrid)
{
	if ((cPoint.x >= 0 && cPoint.x < iGrid->width) &&
		(cPoint.y >= 0 && cPoint.y < iGrid->height))
		return true;
	else
		return false;

}

//Is point inside Grid iPoint
//to do:mayby needs template
bool ShapeGenerator::isInsideGrid(iPoint cPoint, Grid* iGrid)
{
	if ((cPoint.x >= 0 && cPoint.x < iGrid->width) &&
		(cPoint.y >= 0 && cPoint.y < iGrid->height))

		return true;
	else
		return false;

}

//Scale point x,y to x' , y'
void ShapeGenerator::scale(Point orig, Point* scaled, double scaleX, double scaleY)
{
	scaled->x = orig.x * scaleX;
	scaled->y = orig.y * scaleY;
}

void ShapeGenerator::setupCaseLattice(int caseNo, double* setUpTime)
{

	{


		clock_t start = clock();
		int i, j;
		double what;


		std::random_device rd;  //Will be used to obtain a seed for the random number engine
		std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
		std::uniform_real_distribution<> dis(0.0, 1.0);


		thresh = componentsArea[1];
		for (i = 0; i < width; i++) {
#pragma omp parallel for
			for (j = 0; j < height; j++)
			{
				what = dis(gen);
				if (what <= thresh)
				{
					grid->set(i, j, CellState::HARD);
					grid->set(grid->ingadients, i, j, 1);
				}
				else
				{
					grid->set(i, j, CellState::EMPTY);
					grid->set(grid->ingadients, i, j, 0);
				}

			}
		}




		clock_t end = clock();
		*setUpTime = ((double)(end - start)) / CLOCKS_PER_SEC;

	}

}


void ShapeGenerator::setupCase(int caseNo, double* setUpTime)
{
	//int pixelsPerMinimumCircle = 3;
	int totalEllipses;
	int totalCircles;
	int totalRectangles;
	int totalSlopedRectangles;

	double area = 0;


	Ellipse cEllipse(Point(0, 0), 0, 0, 0);
	iPoint iCircle(0, 0);
	Rectangle cRectangle(Point(0, 0), Point(0, 0));
	SlopedRectangle sRectangle(Point(0, 0), 0, 0, 0);


	clock_t start = clock();



	for (int i = 0; i < totalComponents; i++)
	{


		ofstream componentFile;
		string Base = "_shapes.txt";
		string FileName = projectName + "/shapes/case_" + to_string(caseNo) + "_component_" + to_string(i) + Base;

		//open report file
		if ((settings->saveShapes) && (componentsType[i] != ShapeType::NOTHING))
			componentFile.open(FileName);



		//initialize
		totalEllipses = 0;
		totalCircles = 0;
		totalRectangles = 0;
		totalSlopedRectangles = 0;

		//initialize random engine
		std::normal_distribution<double> size(dimensionX[i] * factor, 0.25 * dimensionX[i] * factor); //mean followed by stdiv


		//if it is particle
		if (componentsType[i] != ShapeType::NOTHING)
		{

			//calculate maximum area for current compount
			double maxComp = componentsArea[i] * grid->width * grid->height;

			//print some headers in file
			if (settings->saveShapes)
			{
				componentFile << "Component " << i << " maximum area=" << maxComp << " [on " << grid->width << "x" << grid->height << "]\n";
				if ((componentsType[i] == ShapeType::SLOPEDRECTANGLE) || (componentsType[i] == ShapeType::RECTANGLE))
					componentFile << "Component type: Sloped Rectangle - Rectangle\n";
				else
					componentFile << "Component type: Ellipse - Circle\n";

				componentFile << "------------------------------------------------------------------------------\n";
				componentFile << "------------------- Center------------- dimensions----------------------------\n";
				componentFile << "component| id|    X    |     Y   |  Width/a | height/b |   hoop   |   Slope   \n";
				componentFile << "------------------------------------------------------------------------------\n";
			}

			//initialize total real area of component
			realComponentAreas[caseNo * totalComponents + i] = 0;

			do //do while area < max area for current compound
			{
				//init area
				area = 0;

				//is it rect or rect with slope ?
				if ((componentsType[i] == ShapeType::SLOPEDRECTANGLE) || (componentsType[i] == ShapeType::RECTANGLE))
				{

					double hoop;
					addOneSlopedRectangle(caseNo, i, size, &totalRectangles, &totalSlopedRectangles, &totalEllipses, &totalCircles, &hoop);

					//save some information if it must
					if (settings->saveShapes)
						componentFile << setw(11) << i << setw(1) << "," << setw(5) << totalSlopedRectangles << setw(1) << "," << setw(11) << sRectangle.center.x << setw(1) << "," << setw(11) << sRectangle.center.y << setw(1) << "," << setw(11) << sRectangle.width << setw(1) << "," << setw(11) << sRectangle.height << setw(1) << "," << setw(11) << hoop << setw(1) << "," << setw(11) << sRectangle.slope << "\n";

				}

				//is it circle or ellipse ?
				if ((componentsType[i] == ShapeType::CIRCLE) || (componentsType[i] == ShapeType::ELLIPSE))
				{
					double hoop;

					//add one ellipse
					cEllipse = addOneEllipse(caseNo, i, size, &totalRectangles, &totalSlopedRectangles, &totalEllipses, &totalCircles, &hoop);

					//save some information if it must
					if (settings->saveShapes)
						componentFile << setw(11) << i << setw(1) << "," << setw(5) << totalEllipses << setw(1) << "," << setw(11) << cEllipse.center.x << setw(1) << "," << setw(11) << cEllipse.center.y << setw(1) << "," << setw(11) << cEllipse.a << setw(1) << "," << setw(11) << cEllipse.b << setw(1) << "," << setw(11) << hoop << setw(1) << "," << setw(11) << cEllipse.slope << "\n";


				}
			}// while (abs(realComponentAreas[caseNo*totalComponents + i] - (maxComp)<maxComp/1000));
			while (realComponentAreas[caseNo * totalComponents + i] < maxComp);

			if (swissCheese)
				grid->inverse();

			cout << "Component[" << i << "] Total Circles=" << totalCircles << " Total Ellipses=" << totalEllipses << " Total Rectangles=" << totalRectangles << " Total Sloped Rectangles=" << totalSlopedRectangles << "\n";
			cout << "Component[" << i << "] Real area=" << realComponentAreas[caseNo * totalComponents + i] << " [on " << grid->width << "x" << grid->height << "]" << "%=" << realComponentAreas[caseNo * totalComponents + i] / ((grid->width) * grid->height) << "\n";
			// Debug cout << "total CellState::HARD pixels for all components " << 1.0*grid->countArea() / (grid->width*grid->height) << " %\n";

			if (settings->saveShapes)
			{
				componentFile << "------------------------------------------------------------------------------\n";
				componentFile << "Component[" << i << "] Total Circles=" << totalCircles << " Total Ellipses=" << totalEllipses << " Total Rectangles=" << totalRectangles << " Total Sloped Rectangles=" << totalSlopedRectangles << "\n";
				componentFile << "Component[" << i << "] Real area=" << realComponentAreas[caseNo * totalComponents + i] << " [on " << grid->width << "x" << grid->height << "]=" << "%=" << realComponentAreas[caseNo * totalComponents + i] / (grid->width * grid->height) << "\n";
				componentFile << "------------------------------------------------------------------------------\n";
			}
		}
		componentFile.close();
	}
	clock_t end = clock();
	*setUpTime = ((double)(end - start)) / CLOCKS_PER_SEC;


}

SlopedRectangle ShapeGenerator::addOneSlopedRectangle(int caseNo, int ingradient, std::normal_distribution<double> size, int* totalRectangles, int* totalSlopedRectangles, int* totalEllipses, int* totalCircles, double* hoop)
{
	iPoint Center(0, 0);
	iPoint dims(0, 0);
	Point dCenter(0, 0);
	double maxAngle = 0;
	double minAngle = 0;
//	double sharedArea = 0;
	double rectWidth;
	double rectHeight;


	SlopedRectangle sRectangle(Point(0, 0), 0, 0, 0);

	SetupSizes(ingradient, dimensionY[ingradient], dimensionX[ingradient], factor, size, componentsSizeType[ingradient], &rectWidth, &rectHeight, componentsType[ingradient],
		&maxAngle, &minAngle, totalSlopedRectangles, totalSlopedRectangles, totalCircles, totalEllipses);


	*hoop = hoops[ingradient] * factor;


	sRectangle = generateSlopedRectangle(Point(0.0, 0.0), Point(grid->width - 1, grid->height - 1), rectWidth, rectHeight, maxAngle, minAngle);
	Center = grid->toLocaldimensions(sRectangle.center.x, sRectangle.center.y);

	dims = grid->toLocaldimensionsUp(sRectangle.width, sRectangle.height);
	double pSlope = sRectangle.slope;

	sRectangle = SlopedRectangle(Point(Center.x, Center.y), double(dims.x), double(dims.y), pSlope);
	//sharedArea = grid->countPixelsInsidelopedRectange(Center.x, Center.y, dims.x, dims.y, sRectangle.slope, CellState::HARD);

	swissCheeseCheckAndDraw(ingradient, rectWidth, rectHeight, &sRectangle, *hoop);

	//debug only cout << "Area=" << realComponentAreas[caseNo * totalComponents + ingradient] << " "<< sRectangle.realArea <<"\n";
	realComponentAreas[caseNo * totalComponents + ingradient] = realComponentAreas[caseNo * totalComponents + ingradient] + sRectangle.realArea;// -sharedArea;

	return sRectangle;

}



//only for simplify setup case
Ellipse ShapeGenerator::addOneEllipse(int caseNo, int ingradient, std::normal_distribution<double> size, int* totalRectangles, int* totalSlopedRectangles, int* totalEllipses, int* totalCircles, double* hoop)
{
	double a;
	double b;

	double maxAngle = 0;
	double minAngle = 0;
//	double sharedArea = 0;

	Ellipse cEllipse(Point(0, 0), 0, 0, 0);

	SetupSizes(ingradient, dimensionY[ingradient], dimensionX[ingradient], factor, size, componentsSizeType[ingradient], &a, &b, componentsType[ingradient], &maxAngle, &minAngle, totalRectangles, totalSlopedRectangles, totalCircles, totalEllipses);


	// calculate hoop distance (half) 
	*hoop = 0.5 * hoops[ingradient] * factor;


	//randomize maxTries
	//maxTries = 3;

	iPoint Center(0, 0);
	iPoint axes(0, 0);
	Point dCenter(0, 0);

	//generate random Ellipse
	cEllipse = generateEllipse(Point(0, 0), Point(grid->width - 1, grid->height - 1), a, b, maxAngle, minAngle);
	Center = grid->toLocaldimensions(cEllipse.center.x, cEllipse.center.y);
	axes = grid->toLocaldimensionsUp(cEllipse.a, cEllipse.b);
	double pSlope = cEllipse.slope;
	cEllipse = Ellipse(Point(Center.x, Center.y), double(axes.x), double(axes.y), pSlope);

	//sharedArea = grid->countPixelsInsideEllipse(Center.x, Center.y, axes.x, axes.y, cEllipse.slope, CellState::HARD);

	swissCheeseCheckAndDraw(ingradient, a, b, &cEllipse, *hoop);

	realComponentAreas[caseNo * totalComponents + ingradient] = realComponentAreas[caseNo * totalComponents + ingradient] + cEllipse.realArea;// -sharedArea;
	//devug only cout << "Area=" << realComponentAreas[caseNo * totalComponents + ingradient] << " " << cEllipse.realArea << "\n";

	return cEllipse;
}



void ShapeGenerator::SetupSizes(int ingradient, double dimensionY, double dimensionX, double factor, std::normal_distribution<double> size,
	SizeType componentsSizeType, double* rectWidth, double* rectHeight, ShapeType componentsType,
	double* maxAngle, double* minAngle, int* totalSlopedRectangles, int* totalRectangles, int* totalCircles, int* totalEllipses)
{
	bool haveSlope = false;
	//is it variable sized?
	double shapeFactor = 1.0;
	switch (componentsType)
	{
	case RECTANGLE:
		*totalRectangles = *totalRectangles + 1; shapeFactor = 1;  break;
	case SLOPEDRECTANGLE:
		*totalSlopedRectangles = *totalSlopedRectangles + 1; haveSlope = true; shapeFactor = 1;
		break;
	case CIRCLE:
		*totalCircles = *totalCircles + 1; shapeFactor = 0.5;
		break;
	case ELLIPSE:
		*totalEllipses = *totalEllipses + 1; shapeFactor = 0.5; haveSlope = true;
		break;
	case NOTHING:
		break;
	default:
		break;
	} componentsType;



	if (componentsSizeType == VARIABLE)
	{
		//width = random 
		*rectWidth = size(eng) * shapeFactor;

		//height = width * aspect ratio
		*rectHeight = *rectWidth * dimensionY / dimensionX;
	}
	else
	{
		*rectWidth = dimensionX * factor * shapeFactor;

		*rectHeight = dimensionY * factor * shapeFactor;
	}

	//have slope ?
	if (haveSlope)
	{
		*maxAngle = 90;
		*minAngle = -90;
	}
	else
	{
		*maxAngle = 0;
		*minAngle = 0;
	}

}


void ShapeGenerator::swissCheeseCheckAndDraw(int ingradient, double a, double b, Ellipse* cEllipse, double hoop)
{
	if (!swissCheese)
	{
		//add core plus hoop Area
		if (hoop > 0)
		{
			cEllipse->a = a + hoop;
			cEllipse->b = b + hoop;
			digitizeEllipse(ingradient, *cEllipse, grid, CellState::SOFT);
		}
		//add only core Area
		cEllipse->a = a;
		cEllipse->b = b;
		digitizeEllipse(ingradient, *cEllipse, grid, &cEllipse->realArea, CellState::HARD);
	}
	else
	{
		//add core plus hoop Area
		if (hoop > 0)
		{
			cEllipse->a = a;
			cEllipse->b = b;
			digitizeEllipse(ingradient, *cEllipse, grid, CellState::SOFT);
		}
		//add only core Area
		cEllipse->a = a - hoop;
		cEllipse->b = b - hoop;
		digitizeEllipse(ingradient, *cEllipse, grid, &cEllipse->realArea, CellState::HARD);
	}
}


void ShapeGenerator::swissCheeseCheckAndDraw(int ingradient, double rectWidth, double rectHeight, SlopedRectangle* sRectangle, double hoop)
{

	if (!swissCheese)
	{
		//add current only core Area
		if (hoop > 0)
		{
			//sRectangle->width = rectWidth + hoop;
			//sRectangle->height = rectHeight + hoop;
			SlopedRectangle sRectWithHoop(Point(sRectangle->center.x, sRectangle->center.y), rectWidth + hoop, rectHeight + hoop, sRectangle->slope);
			digitizeSlopedRectangle(ingradient, sRectWithHoop, grid, CellState::SOFT);
		}

		sRectangle->width = rectWidth;
		sRectangle->height = rectHeight;
		digitizeSlopedRectangle(ingradient, *sRectangle, grid, &sRectangle->realArea, CellState::HARD);
	}
	else
	{
		if (hoop > 0)
		{
			sRectangle->width = rectWidth;
			sRectangle->height = rectHeight;
			digitizeSlopedRectangle(ingradient, *sRectangle, grid, CellState::SOFT);
		}
		SlopedRectangle sRectWithHoop(Point(sRectangle->center.x, sRectangle->center.y), rectWidth - hoop, rectHeight - hoop, sRectangle->slope);
		//sRectangle->width = rectWidth - hoop;
		//sRectangle->height = rectHeight - hoop;
		digitizeSlopedRectangle(ingradient, sRectWithHoop, grid, &sRectangle->realArea, CellState::HARD);
	}

}

void ShapeGenerator::monteCarlo(void)
{
	string sPercolate[2] = { "Is NOT percolate ", "IS percolate" };
	double sumPercolation = 0;
	double sumTime = 0;
	double sumSetupTime = 0;
	//double totalPaths = 0;
	double sumElectricConductivity = 0;
	double sumThemalConductivity = 0;
	double sumYoungModulus = 0;
	double sumPOissonRatio = 0;
	double sumPaths = 0;
	double meanRVEPaths = 0;
	double sumPathLength = 0;
	double meanRVEPathLength = 0;
	double meanRVEPathWidth = 0;


	//double sumMeanPathLength = 0;
	cout << "-----------------------------------------------------------------------------------------\n";
	cout << "Pixels Per minimum Circle=" << pixelsPerMinimumCircle << "\n";
	cout << "Minimum Size " << min << "\n";
	cout << "Factor [pixelsPerMinimumCircle / minimumSize]" << factor << "\n";
	cout << "-----------------------------------------------------------------------------------------\n";


	std::seed_seq 	seed({ r(), r(), r(), r(),r(), r(), r(), r(), r() });
	eng.seed(seed);

	for (int i = 0; i < iterations; i++)
	{
		cout << "-----------------------------------------------------------------------------------------\n";
		cout << "Case " << i + 1 << " of " << iterations << "\n";
		string  Base;
		std::seed_seq seed{ r(), r(), r(), r(),r(), r(), r(), r(), r() };
		eng.seed(seed);
		if (!settings->saveAsBmpImage)
			Base = "_Result.pgm";
		else
			Base = "_Result.bmp";
		Results[i] = 0;
		grid->clear();

		if (settings->isLattice)
			setupCaseLattice(i, &setUpTimes[i]);
		else
			setupCase(i, &setUpTimes[i]);


		paths[i] = 0;
		meanPathLength[i] = 0;

		//Calculate electric conductivity
		if (calcElectricConductivity)
		{
			//Results[i] = grid->percolateWithRealPathLength(&paths[i], &meanPathLength[i], &meanRealPathLength[i], &Times[i], materialsElectricConductivity, materialsThermalConductivity, materialsYoungModulus, materialsPoissonRatio, &this->meanPathResistances[i], &this->meanPathThermalResistance[i], &this->meanPathYoungModulus[i], &this->meanPathPoissoonRatio[i]);
			Results[i] = grid->percolateWithRealPathLength(&paths[i], &meanPathLength[i],
				&meanRealPathLength[i], &Times[i], materialsElectricConductivity, &this->meanRVEResistances[i],
				materialsThermalConductivity, &this->meanRVEThermalResistance[i], materialsYoungModulus,
				&this->meanRVEYoungModulus[i], materialsPoissonRatio, &this->meanRVEPoissoonRatio[i]);


			// debug only cout << "__________________" << this->meanPathResistances[i] << "---"<< meanPathLength[i]  	<< meanRealPathLength[i] << " " << meanPathResistances[i] << this->meanPathThermalResistance[i]<< this->meanPathYoungModulus[i]<< this->meanPathPoissoonRatio[i]<<"\n";
		}
		else
		{  // if not calculate
			Results[i] = grid->percolate(&Times[i]);

			// if percolates and not calculate electric conductivity then minimum one path for calculations
			if (Results[i])
				paths[i] = 1;

		}


		if (!Results[i])
			paths[i] = 0;
		else
			sumPathLength += (meanRealPathLength[i] * paths[i]);

		sumPaths += paths[i];
		sumPercolation += Results[i];
		sumTime += Times[i];
		sumSetupTime += setUpTimes[i];



		if (sumPaths > 0)
			meanRVEPathLength = sumPathLength / sumPaths;
		else
			meanRVEPathLength = 0;

		int totalConductivesRVE = 0;
		if (paths[i] > 0)
		{

			electricConductivity[i] = 1 / meanRVEResistances[i];
			thermalConductivities[i] = 1 / meanRVEThermalResistance[i];
			YoungModulus[i] = meanRVEYoungModulus[i];
			PoissonRatio[i] = meanRVEPoissoonRatio[i];
			totalConductivesRVE++;

		}

		sumElectricConductivity += electricConductivity[i];
		sumThemalConductivity += thermalConductivities[i];
		sumYoungModulus += YoungModulus[i];
		sumPOissonRatio += PoissonRatio[i];

		if (calcElectricConductivityWithFDM)
		{
			cout << "-----------------------------------------------------------------------------------------\n";
			cout << "Calculate Electric conductivity with Finite Differences Method..\n,";
			int x = this->grid->width;
			int y = x;
			FD2DEL fd2Del(totalComponents, maxComponents);
			fd2Del.initValues(x, y);
			fd2Del.intitArrays();
			fd2Del.readFromArray(this->grid->ingadients, materialsElectricConductivity);
			char m[] = "m1.dat";
			char out[] = "out.bmp";

			fd2Del.run(m, out, totalComponents);


			generateBitmapImageFortranStyle(fd2Del.pix, height + 2, width + 2, out);


			/*
			*/
			FDResults[i * 2] = fd2Del.currx;
			FDResults[i * 2 + 1] = fd2Del.curry;
		}

		if (this->calcStatistcs)
		{
			cout << "-----------------------------------------------------------------------------------------\n";
			cout << "Calculate Statistics.. Mark Clusters,";

//			Cluster cCluster = grid->markClusters();
			double corrLength = grid->findCorrelationLength(grid->Clusters);
			cout << "Correlation length=" << corrLength << "\n";

			correleationLengths.push_back(corrLength);

		}

		char* fileName = "cluster.bmp";

		if (settings->saveImageFile)
		{
			string FileName = to_string(i) + Base;
			FileName = projectName + "/images/" + FileName;
			char* cstr = &FileName[0u];


			if (!settings->RandomSaveImageFile)
			{
				if (i % (iterations / settings->totalImagesToSave) == 0)
				{



					cout << "Save image file .. " << FileName << "\n";
					grid->saveToDisk(cstr, settings->saveAsBmpImage);
					if (calcStatistcs)
					{
						saveClustersAsBitmapImage(grid->cluster, grid->height, grid->width, 0xAA, fileName);
						cout << "\nSave Clusters as image\n";
					}
				}
			}
			else
			{
				if ((rand() % iterations) < settings->totalImagesToSave)
				{
					cout << "Save images files .. " << FileName << "\n";
					grid->saveToDisk(cstr, settings->saveAsBmpImage);

				}
			}
		}


	}

	meanRVEPaths = sumPaths / iterations;
	meanPercolation = sumPercolation / iterations;
	meanTime = sumTime / iterations;
	meanSetUpTime = sumSetupTime / iterations;
	meanElectricConductivity = (sumElectricConductivity / iterations) + ((width - meanRVEPaths) * materialsElectricConductivity[0] / width);
	meanThermalConductivity = (sumThemalConductivity / iterations) + ((width - meanRVEPaths) * materialsThermalConductivity[0] / width);
	meanRVEPathWidth = meanElectricConductivity / (meanRVEPaths * meanRVEPathLength* materialsElectricConductivity[1]);
	/*
	//traversal direction
	//base properties
	//double stiffnessOfFiber = (sumPOissonRatio / iterations) / meanRVEPaths;

	//double baseMeanPoissonRatio = ((meanRVEPaths * sumPOissonRatio / iterations) + (width - meanRVEPaths) * materialsPoissonRatio[0]) / width;
	//double baseMeanYoungModulus = 1/(1/(sumYoungModulus / iterations) + 1/(((width - meanRVEPaths) * materialsYoungModulus[0]) / width));

	//double baseMeanPoissonRatio = ((meanRVEPaths * stiffnessOfFiber) + (width - meanRVEPaths) * materialsPoissonRatio[0]) / width;
	//double baseMeanYoungModulus = 1 / ((meanRVEPaths / materialsYoungModulus[1]) + ((width - meanRVEPaths) / materialsYoungModulus[0]));


	//meanPoissonRatio = ((meanRVEPaths* stiffnessOfFiber)+((width - meanRVEPaths) * baseMeanPoissonRatio)) / width;
	//meanYoungModulus = ((meanRVEPaths * materialsYoungModulus[1]) + (width - meanRVEPaths) * baseMeanYoungModulus) / width;

	//Last Good estimation
	//meanPoissonRatio = ((meanRVEPaths * sumPOissonRatio / iterations) + ((width - meanRVEPaths) * baseMeanPoissonRatio)) / width;
	//meanYoungModulus = (sumYoungModulus / (iterations) + ((width - meanRVEPaths) * baseMeanYoungModulus) / width);
	*/

	/* This estimation is based on assumption that k = (nw)^(-S/L)*/
	meanYoungModulus = materialsYoungModulus[1] * pow((meanRVEPaths * meanRVEPathWidth), (-width / meanRVEPathLength))/(width);


	cout << "-----------------------------------Results------------------------------------------------\n";
	cout << "Mean Percolation  =" << meanPercolation << "\n";
	cout << "Mean Proccess Time  =" << meanTime << "\n";
	cout << "Mean SetUp Time  =" << meanSetUpTime << "\n";
	cout << "Mean Paths per RVE=" << meanRVEPaths << "\n";
	cout << "Mean Paths Length per RVE=" << meanRVEPathLength << "\n";
	cout << "Mean RVE path Width=" << meanRVEPathWidth/width << " m  or " << meanRVEPathWidth*factor/width<<	" in pixels\n";
	cout << "Mean Special Electric Counductivity =" << meanElectricConductivity << "\n";
	cout << "Mean Special Thermal Counductivity =" << meanThermalConductivity << "\n";
	cout << "Mean Young Modulus =" << meanYoungModulus << "\n";
	cout << "Mean Poisson Ratio " << meanPoissonRatio << "\n";
	cout << "-----------------------------------------------------------------------------------------\n";

	grid->clear();

}

void ShapeGenerator::calcMeanCorrellationLength()
{
	double sum = 0;

	for (size_t i = 0; i < correleationLengths.size(); i++)
		sum = sum + correleationLengths[i];
	correleationLength = sum / correleationLengths.size();
}

void ShapeGenerator::Report()
{
	ofstream File;

	string FileName = projectName + "/report.txt";

	File.open(FileName);

	File << info.program;
	File << info.version;

	File << "Report starting: " << NowToString() << "\n";

	File << "---------------------------------------------------------------------------------------------------\n";
	File << "Inputs\n";
	File << "|#  ,%Weights,%Spec. Weights,X Dimension,Y Dimension,Type,Size type,      Area,hoop Dist\n";
	for (int i = 0; i < totalComponents; i++)
		File << setw(4) << i << "," <<
		setw(9) << components[i] << "," <<
		setw(15) << specialWeights[i] << "," <<
		setw(12) << dimensionX[i] << "," <<
		setw(12) << dimensionY[i] << "," <<
		setw(5) << componentsType[i] << "," <<
		setw(10) << componentsSizeType[i] << "," <<
		setw(11) << componentsArea[i] << "," <<
		setw(9) << hoops[i] << "\n";


	File << "Pixels Per minimum Size:" << pixelsPerMinimumCircle << "\n";

	File << "Factor [pixelsPerMinimumCircle / minimumSize]:" << factor << "\n";
	File << "Maximum Dimension:" << max << "\n";
	File << "Minimum Dimension:" << min << "\n";
	File << "Grid size =[" << grid->width << "x" << grid->height << "]" << "\n";
	File << "Total iterations: " << iterations << "\n";

	File << "---------------------------------------------------------------------------------------------------\n";

	File << "Realiazation No,Percolate Y[1]/N[0], Process Time, Setup Time ";
	for (int i = 0; i < totalComponents; i++)
		File << setw(15) << ",% Area Comp[" << i << "]";

	if (calcStatistcs)
		File << ",Max Cluster Radius ,Correlation Length";

	if (calcElectricConductivity)
		File << ",Electric Conductivity, Thermal Conductivity, Young Modulus, Poisson Ratio ,Total Conductive Paths,Mean Real Path Length,Current Electric Conductivity";

	if (calcElectricConductivityWithFDM)
		File << ",FDM Ix       ,FDM Iy,       FDM  ro";

	File << "\n";


	for (int i = 0; i < iterations; i++)
	{
		File << setw(7) << i + 1 << setw(1) << "," << setw(19) << Results[i] << setw(1) << "," << setw(10) << Times[i] << "," << setw(15) << setUpTimes[i];
		for (int j = 0; j < totalComponents; j++)
		{
			double SumRealComponentsArea = 0;
			for (int k = (i * totalComponents); k < (i + 1) * totalComponents; k++)
				SumRealComponentsArea = SumRealComponentsArea + realComponentAreas[k];

			realComponentAreas[i * totalComponents] = (long(grid->width) * grid->height) - SumRealComponentsArea;

			File << setw(13) << "," << realComponentAreas[i * totalComponents + j] / (long(grid->width) * grid->height);
		}
		if (calcStatistcs)
			File << "," << setw(13) << this->grid->cMaxClusterRadius[i]
			<< "," << setw(13) << correleationLengths[i];
		double eleCondu;
		if (paths[i] > 0)
			eleCondu = (1 / this->meanRVEResistances[i]) * this->paths[i] / width;
		else
			eleCondu = 0;
		if (calcElectricConductivity)
			File << "," << setw(13) << this->meanRVEResistances[i]
			<< "," << setw(13) << this->meanRVEThermalResistance[i]
			<< "," << setw(13) << this->meanRVEYoungModulus[i]
			<< "," << setw(13) << this->meanRVEPoissoonRatio[i]
			<< "," << setw(13) << this->paths[i]
			<< "," << setw(13) << this->meanRealPathLength[i]
			<< "," << setw(13) << eleCondu;



		if (calcElectricConductivityWithFDM)
			File << "," << setw(13) << this->FDResults[2 * i] << "," << setw(13) << this->FDResults[2 * i + 1] << "," << setw(13) << (1 / this->FDResults[2 * i]);

		File << "\n";
	}
	File << "---------------------------------------------------------------------------------------------------\n";

	File << "Mean Percolation=" << meanPercolation << "\n";
	File << "Mean Processing Time=" << meanTime << "\n";
	File << "Mean set up time=" << meanSetUpTime << "\n";
	File << "Mean correlation Length=" << correleationLength << "\n";

	for (int j = 0; j < totalComponents; j++)
	{
		double mean = 0;
		for (int i = 0; i < iterations; i++)
			mean = mean + realComponentAreas[i * totalComponents + j] / (grid->width * grid->height);

		File << "Mean Area of component[" << j << "]=" << mean / iterations << "\n";
	}
	File.close();


}


void ShapeGenerator::ReportWithSemicolon()
{
	ofstream File;

	string FileName = projectName + "/ReportWithSemicolon.txt";

	File.open(FileName);

	File << info.program;
	File << info.version;

	File << "Report starting: " << NowToString() << "\n";

	File << "---------------------------------------------------------------------------------------------------\n";
	File << "Inputs\n";
	File << "|#  ;%Weights;%Spec. Weights;X Dimension;Y Dimension;Type;Size type;      Area;hoop Dist\n";
	for (int i = 0; i < totalComponents; i++)
		File << setw(4) << i << ";" <<
		setw(9) << components[i] << ";" <<
		setw(15) << specialWeights[i] << ";" <<
		setw(12) << dimensionX[i] << ";" <<
		setw(12) << dimensionY[i] << ";" <<
		setw(5) << componentsType[i] << ";" <<
		setw(10) << componentsSizeType[i] << ";" <<
		setw(11) << componentsArea[i] << ";" <<
		setw(9) << hoops[i] << "\n";


	File << "Pixels Per minimum Size:" << pixelsPerMinimumCircle << "\n";

	File << "Factor [pixelsPerMinimumCircle / minimumSize]:" << factor << "\n";
	File << "Maximum Dimension:" << max << "\n";
	File << "Minimum Dimension:" << min << "\n";
	File << "Grid size =[" << grid->width << "x" << grid->height << "]" << "\n";
	File << "Total iterations: " << iterations << "\n";

	File << "---------------------------------------------------------------------------------------------------\n";

	File << "Realiazation No;Percolate Y[1]/N[0]; Process Time; Setup Time ";
	for (int i = 0; i < totalComponents; i++)
		File << setw(15) << ";% Area Comp[" << i << "]";

	if (calcStatistcs)
		File << ";Max Cluster Radius ;Correlation Length";

	if (calcElectricConductivity)
		File << ";Electric Conductivity; Thermal Conductivity; Young Modulus; Poisson Ratio ;Total Conductive Paths;Mean Real Path Length;Current Electric Conductivity";

	if (calcElectricConductivityWithFDM)
		File << ";FDM Ix       ;FDM Iy;       FDM ñ";

	File << "\n";


	for (int i = 0; i < iterations; i++)
	{
		File << setw(7) << i + 1 << setw(1) << ";" << setw(19) << Results[i] << setw(1) << ";" << setw(10) << Times[i] << ";" << setw(15) << setUpTimes[i];
		for (int j = 0; j < totalComponents; j++)
		{
			double SumRealComponentsArea = 0;
			for (int k = (i * totalComponents); k < (i + 1) * totalComponents; k++)
				SumRealComponentsArea = SumRealComponentsArea + realComponentAreas[k];

			realComponentAreas[i * totalComponents] = (long(grid->width) * grid->height) - SumRealComponentsArea;

			File << setw(13) << ";" << realComponentAreas[i * totalComponents + j] / (long(grid->width) * grid->height);
		}
		if (calcStatistcs)
			File << ";" << setw(13) << this->grid->cMaxClusterRadius[i]
			<< ";" << setw(13) << correleationLengths[i];
		double eleCondu;
		if (paths[i] > 0)
			eleCondu = (1 / this->meanRVEResistances[i]) * this->paths[i] / width;
		else
			eleCondu = 0;
		if (calcElectricConductivity)
			File << ";" << setw(13) << this->meanRVEResistances[i]
			<< ";" << setw(13) << this->meanRVEThermalResistance[i]
			<< ";" << setw(13) << this->meanRVEYoungModulus[i]
			<< ";" << setw(13) << this->meanRVEPoissoonRatio[i]
			<< ";" << setw(13) << this->paths[i]
			<< ";" << setw(13) << this->meanRealPathLength[i]
			<< ";" << setw(13) << eleCondu;



		if (calcElectricConductivityWithFDM)
			File << ";" << setw(13) << this->FDResults[2 * i] << ";" << setw(13) << this->FDResults[2 * i + 1] << ";" << setw(13) << (1 / this->FDResults[2 * i]);

		File << "\n";
	}
	File << "---------------------------------------------------------------------------------------------------\n";

	File << "Mean Percolation=" << meanPercolation << "\n";
	File << "Mean Processing Time=" << meanTime << "\n";
	File << "Mean set up time=" << meanSetUpTime << "\n";
	File << "Mean correlation Length=" << correleationLength << "\n";

	for (int j = 0; j < totalComponents; j++)
	{
		double mean = 0;
		for (int i = 0; i < iterations; i++)
			mean = mean + realComponentAreas[i * totalComponents + j] / (grid->width * grid->height);

		File << "Mean Area of component[" << j << "]=" << mean / iterations << "\n";
	}
	File.close();


}


void ShapeGenerator::ReportStatistics(void)
{
	ofstream File;

	string FileName = projectName + "/Statistics.csv";

	File.open(FileName);

	File << info.program;
	File << info.version;

	File << "Report starting: " << NowToString() << "\n";

	File << "---------------------------------------------------------------------------------------------------\n";
	File << "Inputs\n";
	File << "|#  |%Weights|%Spec. Weights|X Dimension|Y Dimension|Type|Size type|      Area|hoop Dist\n";
	for (int i = 0; i < totalComponents; i++)
		File << setw(4) << i << setw(9) << components[i] << setw(15) << specialWeights[i] << setw(12) << dimensionX[i] << setw(12) << dimensionY[i] << setw(5) << componentsType[i] << setw(10) << componentsSizeType[i] << setw(11) << componentsArea[i] << setw(9) << hoops[i] << "\n";


	File << "Pixels Per minimum Size:" << pixelsPerMinimumCircle << "\n";

	File << "Factor [pixelsPerMinimumCircle / minimumSize]:" << factor << "\n";
	File << "Maximum Dimension:" << "," << max << "\n";
	File << "Minimum Dimension:" << "," << min << "\n";
	File << "Grid size =[" << "," << grid->width << "x" << "," << grid->height << "]" << "\n";
	File << "Total iterations: " << "," << iterations << "\n";

	File << "---------------------------------------------------------------------------------------------------\n";
	File << "Total  Points  |Mean Radius | Inertia \n";

	//clustersSize = int(0.5 * sqrt(grid->height * grid->height + grid->width* grid->width));
	for (size_t i = 0; i < grid->Clusters.size(); i++)
	{
		if (grid->Clusters[i].totalPoints > 0)
		{

			File << grid->Clusters[i].totalPoints;
			File << "," << setw(10) << grid->Clusters[i].radius;
			File << "," << setw(10) << grid->Clusters[i].inertia;
		}

		File << "\n";
	}
	File << "---------------------------------------------------------------------------------------------------\n";
	File.close();




}



SlopedRectangle ShapeGenerator::generateSlopedRectangle(Point downleft, Point upRight, double width, double height, double maxAngle, double minAngle)
{
	double angle;
	Point center(std::uniform_real_distribution<>{downleft.x, upRight.x }(eng), std::uniform_real_distribution<> {downleft.y, upRight.y }(eng));

	if (maxAngle != minAngle)
		angle = std::uniform_real_distribution<>{ minAngle, maxAngle }(eng);

	else
		angle = maxAngle;


	angle = angle * M_PI / (180);

	SlopedRectangle Result(center, width, height, angle);


	return Result;
}



Ellipse ShapeGenerator::generateEllipse(Point downleft, Point upRight, double ia, double ib, double maxAngle, double minAngle)
{

	Point center(std::uniform_real_distribution<>{downleft.x, upRight.x }(eng), std::uniform_real_distribution<> {downleft.y, upRight.y }(eng));


	double angle;
	if (maxAngle != minAngle)
		angle = std::uniform_real_distribution<>{ minAngle, maxAngle }(eng);

	else
		angle = maxAngle;

	angle = angle * M_PI / (180);
	Ellipse Result(center, ia, ib, angle);

	return Result;

}


void ShapeGenerator::digitizeSlopedRectangle(int ingradient, SlopedRectangle sRect, Grid* iGrid, CellState state)
{
	int i, j;
	Range region(iGrid->toLocaldimensionsDown(sRect.minX() - 1, sRect.minY() - 1), iGrid->toLocaldimensionsUp(sRect.maxX() + 1, sRect.maxY() + 1));

	//check bounds
	if (region.from.x < 0)
		region.from.x = 0;
	if (region.from.y < 0)
		region.from.y = 0;
	if (region.to.x > iGrid->width)
		region.to.x = iGrid->width;
	if (region.to.y > iGrid->height)
		region.to.y = iGrid->height;

	for (i = region.from.x; i < region.to.x; i++)
		for (j = region.from.y; j < region.to.y; j++)
		{
			Point rotated = Point::transform(i, j, sRect.center.x, sRect.center.y, -sRect.slope);
			if ((1.0 * rotated.x <= (1.0 * sRect.center.x + (sRect.width / 2.0)))
				&& (1.0 * rotated.x >= (1.0 * sRect.center.x - (sRect.width / 2.0)))
				&& (1.0 * rotated.y <= (1.0 * sRect.center.y + (sRect.height / 2.0)))
				&& (1.0 * rotated.y >= (1.0 * sRect.center.y - (sRect.height / 2.0))))
			{

				if (state == CellState::HARD)
				{
					iGrid->set(i, j, state);
					iGrid->ingadients[j * width + i] = (unsigned char)(ingradient);
				}
				else
					if (iGrid->get(i, j) != CellState::HARD)
					{
						iGrid->ingadients[j * width + i] = (unsigned char)(ingradient);
						iGrid->set(i, j, state);
					}
			}
		}


}



void ShapeGenerator::digitizeSlopedRectangle(int ingradient, SlopedRectangle sRect, Grid* iGrid, double* realArea, CellState state)
{
	/*int i, j;

	Range region(iGrid->toLocaldimensionsDown(0, sRect.minY() - 2), iGrid->toLocaldimensionsUp(0, sRect.maxY() + 2));

	*realArea = 0;

	//check bounds
	if ((abs(sRect.slope) > M_PI * 45 / 180))
	{
		if (region.from.y < 0)
			region.from.y = 0;

		if (region.to.y > iGrid->height)
			region.to.y = iGrid->height;

		double lamda = tan(-sRect.slope);
		double d = 0.5*sRect.height *(2 - abs(sin(sRect.slope)));

#pragma omp parallel for

		for (j = region.from.y; j < region.to.y; j++)
		{

			region.from.x = int(((j - sRect.center.y) / lamda) + sRect.center.x - d-2);
			region.to.x = int(((j - sRect.center.y) / lamda) + sRect.center.x + d+2);

			if (region.from.x > region.to.x)
			{
				int temp = region.from.x;
				region.from.x = region.to.x;
				region.to.x = temp;

			}


			if (region.from.x < 0)
				region.from.x = 0;

			if (region.to.x > iGrid->width)
				region.to.x = iGrid->width;

			for (i = region.from.x; i < region.to.x; i++)
			{

				Point rotated = Point::transform(i + 0.5, j + 0.5, sRect.center.x, sRect.center.y, -sRect.slope);
				//if rotated inside circle


				// Debug draw scan region:
				// iGrid->set(i, j, CellState::SOFT);
				if ((rotated.x <= (sRect.center.x + (sRect.width / 2))) && (rotated.x >= (sRect.center.x - (sRect.width / 2))) && ((rotated.y <= (sRect.center.y + (sRect.height / 2))) && rotated.y >= (sRect.center.y - (sRect.height / 2))))
				{
					char cPixel = iGrid->get(i, j);
					if (state == CellState::HARD)
					{

						if ((cPixel != 0) && (cPixel != CellState::HARD) && (cPixel != BORDER)) // in one step
							*realArea += 1.0;

						iGrid->set(i, j, state);

					}
					else
						if (cPixel != CellState::HARD)
							iGrid->set(i, j, state);
				}
			}
		}
	}
	else
	{
		region.from.x = int(sRect.minX()) - 2;
		region.to.x = int(sRect.maxX()) + 2;

		if (region.from.x < 0)
			region.from.x = 0;

		if (region.to.x > iGrid->width)
			region.to.x = iGrid->width;

		double lamda = -tan(sRect.slope);
		double d = 0.5*sRect.height / cos(sRect.slope);

#pragma omp parallel for

		for (i = region.from.x; i < region.to.x; i++)
		{

			region.from.y = int(((i - sRect.center.x) * lamda) + sRect.center.y - d - 2);
			region.to.y = int(((i - sRect.center.x) * lamda) + sRect.center.y + d + 2);

			if (region.from.y > region.to.y)
			{
				int temp = region.from.y;
				region.from.y = region.to.y;
				region.to.y = temp;

			}

			if (region.from.y < 0)
				region.from.y = 0;

			if (region.to.y > iGrid->height)
				region.to.y = iGrid->height;

			for (j = region.from.y; j < region.to.y; j++)
			{

				Point rotated = Point::transform(i + 0.5, j + 0.5, sRect.center.x, sRect.center.y, -sRect.slope);
				//if rotated inside circle
				//distance = ((rotated.x - sRect.center.x)*(rotated.x - iEllipse.center.x) / (iEllipse.a*iEllipse.a)) + ((rotated.y - iEllipse.center.y)*(rotated.y - iEllipse.center.y) / (iEllipse.b* iEllipse.b));

				//iGrid->set(i, j, CellState::SOFT);
				if((rotated.x <= (sRect.center.x + (sRect.width / 2))) && (rotated.x >= (sRect.center.x - (sRect.width / 2))) && ((rotated.y <= (sRect.center.y + (sRect.height / 2))) && rotated.y >= (sRect.center.y - (sRect.height / 2))))
				{
					char cPixel = iGrid->get(i, j);
					if (state == CellState::HARD)
					{

						if ((cPixel != 0) && (cPixel != CellState::HARD) && (cPixel != BORDER)) // in one step
							*realArea += 1.0;

						iGrid->set(i, j, state);

					}
					else
						if (cPixel != CellState::HARD)
							iGrid->set(i, j, state);
				}
			}
		}
	}
	*/


	int i, j;
	Range region(iGrid->toLocaldimensionsDown(sRect.minX() - 1, sRect.minY() - 1), iGrid->toLocaldimensionsUp(sRect.maxX() + 1, sRect.maxY() + 1));
	*realArea = 0;

	//check bounds
	if (region.from.x < 0)
		region.from.x = 0;
	if (region.from.y < 0)
		region.from.y = 0;
	if (region.to.x > iGrid->width)
		region.to.x = iGrid->width;
	if (region.to.y > iGrid->height)
		region.to.y = iGrid->height;

	for (i = region.from.x; i < region.to.x; i++)
		for (j = region.from.y; j < region.to.y; j++)
		{

			Point rotated = Point::transform(i + 0.5, j + 0.5, sRect.center.x, sRect.center.y, -sRect.slope);
			if ((rotated.x <= (sRect.center.x + (sRect.width / 2))) && (rotated.x >= (sRect.center.x - (sRect.width / 2))) && ((rotated.y <= (sRect.center.y + (sRect.height / 2))) && rotated.y >= (sRect.center.y - (sRect.height / 2))))
			{


				char cPixel = iGrid->get(i, j);
				if (state == CellState::HARD)
				{

					if ((cPixel != 0) && (cPixel != CellState::HARD) && (cPixel != BORDER)) // in one step
					{
						iGrid->ingadients[j * width + i] = (unsigned char)(ingradient);
						*realArea += 1.0;
					}
					iGrid->set(i, j, state);

				}
				else
					if (cPixel != CellState::HARD)
						iGrid->set(i, j, state);
			}
		}


}


void ShapeGenerator::digitizeSlopedRectangleWithBorder(int ingradient, SlopedRectangle sRect, Grid* iGrid, double* realArea, CellState state)
//	void ShapeGenerator::digitizeSlopedRectangle(SlopedRectangle sRect, Grid* iGrid, double *realArea, CellState state)
{
	int i, j;
	double zone = 0.90;
	Range region(iGrid->toLocaldimensionsDown(sRect.minX() - 1, sRect.minY() - 1), iGrid->toLocaldimensionsUp(sRect.maxX() + 1, sRect.maxY() + 1));
	*realArea = 0;

	//check bounds
	if (region.from.x < 0)
		region.from.x = 0;
	if (region.from.y < 0)
		region.from.y = 0;
	if (region.to.x > iGrid->width)
		region.to.x = iGrid->width;
	if (region.to.y > iGrid->height)
		region.to.y = iGrid->height;

	for (i = region.from.x; i < region.to.x; i++)
		for (j = region.from.y; j < region.to.y; j++)
		{

			Point rotated = Point::transform(i + 0.5, j + 0.5, sRect.center.x, sRect.center.y, -sRect.slope);

			if ((rotated.x <= (sRect.center.x + (sRect.width / 2))) && (rotated.x >= (sRect.center.x - (sRect.width / 2))) && ((rotated.y <= (sRect.center.y + (sRect.height / 2))) && rotated.y >= (sRect.center.y - (sRect.height / 2))))
			{
				if ((rotated.x <= sRect.center.x + (sRect.width / 2)) && (rotated.x >= sRect.center.x + (zone * sRect.width / 2)) ||
					(rotated.x >= sRect.center.x - (sRect.width / 2)) && (rotated.x <= sRect.center.x - (zone * sRect.width / 2)) ||
					(rotated.y <= sRect.center.y + (sRect.height / 2)) && (rotated.y >= sRect.center.y + (zone * sRect.height / 2)) ||
					(rotated.y >= sRect.center.y - (sRect.height / 2)) && (rotated.y <= sRect.center.y - (zone * sRect.height / 2)))
				{
					if (state == CellState::HARD)
					{
						if (iGrid->get(i, j) != 0)
						{
							*realArea += 1.0;
							iGrid->ingadients[j * width + i] = (unsigned char)(ingradient);
						}
						iGrid->set(i, j, BORDER);

					}
					else
						if (iGrid->get(i, j) != CellState::HARD)
							iGrid->set(i, j, BORDER);
				}
				else
					if (state == CellState::HARD)
					{
						if (iGrid->get(i, j) != 0)
						{
							*realArea += 1.0;
							iGrid->ingadients[j * width + i] = (unsigned char)(ingradient);
						}

						iGrid->set(i, j, state);

					}
					else
						if (iGrid->get(i, j) != CellState::HARD)
							iGrid->set(i, j, state);

			}


		}
}




void ShapeGenerator::digitizeEllipse(int ingradient, Ellipse iEllipse, Grid* iGrid, double* realArea, CellState state)
{
	//		omp_set_num_threads(4);
	*realArea = 0;
	double subsum[4];

#pragma omp parallel   //, iEllipsoid, iGrid, realArea, state,isShpere, invDensity, distance, i, j, k)

#pragma omp sections nowait
	{
#pragma omp section
		digitizeEllipseDiv4(ingradient, iEllipse, iGrid, &subsum[0], state, true, true);

#pragma omp section
		digitizeEllipseDiv4(ingradient, iEllipse, iGrid, &subsum[1], state, false, true);

#pragma omp section
		digitizeEllipseDiv4(ingradient, iEllipse, iGrid, &subsum[2], state, true, false);


#pragma omp section
		digitizeEllipseDiv4(ingradient, iEllipse, iGrid, &subsum[3], state, false, false);

	}

	for (int i = 0; i < 4; i++)
	{
		*realArea = subsum[i] + *realArea;
	}

}


void ShapeGenerator::digitizeEllipseDiv4(int ingradient, Ellipse iEllipse, Grid* iGrid, double* realArea, CellState state, bool x, bool y)
{
	int i, j;

	double distance;
	Point rotated(0, 0);
	iPoint iRotated(0, 0);

	*realArea = 0;
	double density = 1.8;

	bool isCircle = (iEllipse.a == iEllipse.b);
	if (isCircle)
		density = 1.0;

	double invDensity = 1 / density;
	double sqInvDensity = invDensity * invDensity;

	double costhetaX = cos(iEllipse.slope);
	double sinthetaX = sin(iEllipse.slope);



	double sqEllipsoidA = (iEllipse.a * iEllipse.a);
	double sqEllipsoidB = (iEllipse.b * iEllipse.b);


	int maxi, maxj;

	maxi = int(density * (iEllipse.a + 2));

	maxj = int(density * (iEllipse.b + 2));


	int muli = 1;
	int mulj = 1;

	if (!x)
		muli = -1;
	if (!y)
		mulj = -1;


	int ii, jj;

	{

		for (i = 0; i < maxi; i++)
			for (j = 0; j < maxj; j++)
			{
				ii = i * muli;
				jj = j * mulj;


				distance =
					((sqInvDensity * i * i) / sqEllipsoidA) +
					((sqInvDensity * j * j) / sqEllipsoidB);

				if (distance <= 1.0)
				{
					if (!isCircle)
						rotated = Point::transform(invDensity * ii, invDensity * jj, 0, 0,
							iEllipse.slope, costhetaX, sinthetaX);
					else
					{
						rotated = { (double)ii,(double)jj };

					}

					rotated = { rotated.x + iEllipse.center.x,
						rotated.y + iEllipse.center.y };



					iRotated = rotated.round();


					if (isInsideGrid(iRotated, iGrid))
					{

						size_t where = (size_t)iRotated.y * width + iRotated.x;
						char cPixel = iGrid->cell[where];
						{
							if (state == CellState::HARD)
							{
								if (cPixel != CellState::HARD)
								{
									iGrid->ingadients[where] = (unsigned char)(ingradient);
									*realArea = 1.0 + *realArea;

								}
								iGrid->cell[where] = CellState::HARD;


							}
							else
								if (cPixel != CellState::HARD)
								{
									iGrid->cell[where] = state;

									iGrid->ingadients[where] = (unsigned char)(ingradient);
								}
						}



					}
				}
			}

	}
}


/*
void ShapeGenerator::digitizeEllipseDiv4(char ingradient, Ellipse iEllipse, Grid* iGrid, double* realArea, CellState state, bool x, bool y)
{
	int i, j;

	double distance;
	Point rotated(0, 0);
	iPoint iRotated(0, 0);

	*realArea = 0;
	double density = 1.8;

	bool isCircle = (iEllipse.a == iEllipse.b);
	if (isCircle)
		density = 1.0;

	double invDensity = 1 / density;
	double sqInvDensity = invDensity * invDensity;

	double costhetaX = cos(iEllipse.slope);
	double sinthetaX = sin(iEllipse.slope);



	double sqEllipsoidA = (iEllipse.a * iEllipse.a);
	double sqEllipsoidB = (iEllipse.b * iEllipse.b);


	int maxi, maxj;

	maxi = int(density * (iEllipse.a + 2));

	maxj = int(density * (iEllipse.b + 2));


	int muli = 1;
	int mulj = 1;

	if (!x)
		muli = -1;
	if (!y)
		mulj = -1;


	int ii, jj;

	{

		for (i = 0; i < maxi; i++)
			for (j = 0; j < maxj; j++)
			{
				ii = i * muli;
				jj = j * mulj;


				distance =
					((sqInvDensity * i * i) / sqEllipsoidA) +
					((sqInvDensity * j * j) / sqEllipsoidB);



				if (distance <= 1.0)
				{
					if (!isCircle)
						rotated = Point::transform(invDensity * ii, invDensity * jj, 0, 0,
							iEllipse.slope, costhetaX, sinthetaX);
					else
					{
						rotated = { (double)ii,(double)jj };

					}

					rotated = { rotated.x + iEllipse.center.x,
						rotated.y + iEllipse.center.y };



					iRotated = rotated.round();


					if (isInsideGrid(iRotated, iGrid))
					{

						size_t where = (size_t)iRotated.y * width + iRotated.x;
						char cPixel = iGrid->cell[where];
						{
							if (state == CellState::HARD)
							{
								if (cPixel != CellState::HARD)
								{
									iGrid->ingadients[where] = ingradient;
									*realArea = 1.0 + *realArea;

								}
								iGrid->cell[where] = CellState::HARD;
							}
							else
								if (cPixel != CellState::HARD)
								{
									iGrid->cell[where] = state;
									iGrid->ingadients[where] = ingradient;
								}
						}



					}
				}
			}

	}
}

*/


void ShapeGenerator::digitizeEllipse(int ingradient, Ellipse iEllipse, Grid* iGrid, CellState state)
{

	int i, j;

	Range region(iGrid->toLocaldimensionsDown(iEllipse.minX() - 1, iEllipse.minY() - 1), iGrid->toLocaldimensionsUp(iEllipse.maxX() + 1, iEllipse.maxY() + 1));



	//check bounds
	if (region.from.x < 0)
		region.from.x = 0;
	if (region.from.y < 0)
		region.from.y = 0;
	if (region.to.x > iGrid->width)
		region.to.x = iGrid->width;
	if (region.to.y > iGrid->height)
		region.to.y = iGrid->height;

	for (i = region.from.x; i < region.to.x; i++)
		for (j = region.from.y; j < region.to.y; j++)
		{

			Point rotated = Point::transform(i + 0.5, j + 0.5, iEllipse.center.x, iEllipse.center.y, -iEllipse.slope);

			if (pow(1.0 * (rotated.x - iEllipse.center.x) / (1.0 * iEllipse.a), 2.0) + pow(1.0 * (rotated.y - iEllipse.center.y) / (1.0 * iEllipse.b), 2.0) <= 1.0)
			{
				if (iGrid->get(i, j) != CellState::HARD)
				{
					if (state == CellState::HARD)
					{
						iGrid->set(i, j, state);
						iGrid->set(iGrid->ingadients, i, j, ingradient);
					}
					else
						if (iGrid->get(i, j) != CellState::HARD)
						{
							iGrid->set(i, j, state);
							iGrid->set(iGrid->ingadients, i, j, ingradient);
						}
				}
			}
		}

}



void ShapeGenerator::digitizeEllipseWithBorder(Ellipse iEllipse, Grid* iGrid, double* realArea, CellState state)
//void ShapeGenerator::digitizeEllipse(Ellipse iEllipse, Grid* iGrid, double *realArea, CellState state)
{
	int i, j;
	double distance;
	Range region(iGrid->toLocaldimensionsDown(iEllipse.minX() - 1, iEllipse.minY() - 1), iGrid->toLocaldimensionsUp(iEllipse.maxX() + 1, iEllipse.maxY() + 1));

	*realArea = 0;

	//check bounds
	if (region.from.x < 0)
		region.from.x = 0;
	if (region.from.y < 0)
		region.from.y = 0;
	if (region.to.x > iGrid->width)
		region.to.x = iGrid->width;
	if (region.to.y > iGrid->height)
		region.to.y = iGrid->height;

	for (i = region.from.x; i < region.to.x; i++)
		for (j = region.from.y; j < region.to.y; j++)
		{

			Point rotated = Point::transform(i + 0.5, j + 0.5, iEllipse.center.x, iEllipse.center.y, -iEllipse.slope);
			//if rotated inside circle
			distance = ((rotated.x - iEllipse.center.x) * (rotated.x - iEllipse.center.x) / (iEllipse.a * iEllipse.a)) + ((rotated.y - iEllipse.center.y) * (rotated.y - iEllipse.center.y) / (iEllipse.b * iEllipse.b));
			if ((distance <= 1.0) && (distance >= 0.8))
			{
				// is material ?
				if (state == CellState::HARD)
				{
					//is inside grid ?
					if (iGrid->get(i, j) != 0)
						*realArea += 1.0;

					iGrid->set(i, j, BORDER);

				}
				else
					if (iGrid->get(i, j) != CellState::HARD)
						iGrid->set(i, j, state);

			}
			else
				if ((distance < 0.8))
				{
					if (state == CellState::HARD)
					{
						//is inside grid ?
						if (iGrid->get(i, j) != 0)
							*realArea += 1.0;

						iGrid->set(i, j, CellState::HARD);

					}
					else
						if (iGrid->get(i, j) != CellState::HARD)
							iGrid->set(i, j, state);

				}

		}

}

/*

void ShapeGenerator::digitizeEllipse(Ellipse iEllipse, Grid* iGrid, double *realArea, CellState state)
{
	int i, j;

	double distance;
	Point rotated(0,0);
	iPoint iRotated(0, 0);

	*realArea = 0;
	double density = 2.0;

	bool isCircle = (iEllipse.a == iEllipse.b);
	if (isCircle)
		density = 1.0;

	double invDensity = 1 / density;
	double sqInvDensity = invDensity * invDensity;
	double sqrEllipseA = (iEllipse.a*iEllipse.a);
	double sqrEllipseB = (iEllipse.b*iEllipse.b);
#pragma omp  for
	for (i = int(-density * (iEllipse.a + 2)); i < int(density*(iEllipse.a + 2)); i++)
	{
		for (j = int(-density * (iEllipse.b + 2)); j < int(density*(iEllipse.b + 2)); j++)
		{

			distance =
				((sqInvDensity*i*i) / sqrEllipseA) +
				((sqInvDensity*j*j) / sqrEllipseB);


				if (distance <= 1.0)
				{
					if (!isCircle)
						rotated = Point::transform(invDensity*i, invDensity*j, 0, 0,iEllipse.slope);
					else
					{
						rotated.x = i;
						rotated.y = j;

					}

					rotated.x = rotated.x + iEllipse.center.x;
					rotated.y = rotated.y + iEllipse.center.y;


					iRotated.x = round(rotated.x);
					iRotated.y = round(rotated.y);



					if ((iRotated.x >= 0) && (iRotated.y  >= 0) && (iRotated.x < width) && (iRotated.y < height))
					{
						char cPixel = iGrid->get(iRotated.x, iRotated.y);
						{

							if (state == CellState::HARD)
							{
								if (cPixel != CellState::HARD)
								{
									//iGrid->set(iGrid->ingadients, iRotated.x, iRotated.y, ingradient);
									*realArea += 1.0;
								}

								iGrid->set(iRotated.x, iRotated.y,  CellState::HARD);
							}
							else
								if (cPixel != CellState::HARD)
								{
									iGrid->set(iRotated.x, iRotated.y, state);
									//iGrid->set(iGrid->ingadients, iRotated.x, iRotated.y, iRotated.z, ingradient);
								}
						}

					}
				}

		}
	}
}
*/

/*

void ShapeGenerator::digitizeEllipse(Ellipse iEllipse, Grid* iGrid, double *realArea, CellState state)
{
	int i, j;
	double distance;
	Range region(iGrid->toLocaldimensionsDown(0, iEllipse.minY() - 2), iGrid->toLocaldimensionsUp(0, iEllipse.maxY() + 2));

	*realArea = 0;

	//check bounds
	if ((abs(iEllipse.slope) > M_PI*45/180))
	{
		if (region.from.y < 0)
			region.from.y = 0;

		if (region.to.y > iGrid->height)
			region.to.y = iGrid->height;

		double lamda = tan(-iEllipse.slope);
		double d = iEllipse.b *(2-abs(sin(iEllipse.slope)));

#pragma omp for

		for (j = region.from.y; j < region.to.y; j++)
		{

			region.from.x = int(((j - iEllipse.center.y) / lamda) + iEllipse.center.x - d);
			region.to.x = int(((j - iEllipse.center.y) / lamda) + iEllipse.center.x + d);

			if (region.from.x > region.to.x)
			{
				int temp = region.from.x;
				region.from.x = region.to.x;
				region.to.x = temp;

			}
			region.from.x--;
			region.to.x++;
			region.to.x++;

			if (region.from.x < 0)
				region.from.x = 0;

			if (region.to.x > iGrid->width)
				region.to.x = iGrid->width;

			for (i = region.from.x; i < region.to.x; i++)
			{

				Point rotated = Point::transform(i + 0.5, j + 0.5, iEllipse.center.x, iEllipse.center.y, -iEllipse.slope);
				//if rotated inside circle
				distance = ((rotated.x - iEllipse.center.x)*(rotated.x - iEllipse.center.x) / (iEllipse.a*iEllipse.a)) + ((rotated.y - iEllipse.center.y)*(rotated.y - iEllipse.center.y) / (iEllipse.b* iEllipse.b));

				//iGrid->set(i, j, CellState::SOFT);
				if (distance <= 1.0)
				{
					char cPixel = iGrid->get(i, j);
					if (state == CellState::HARD)
					{

						if ((cPixel != 0) && (cPixel != CellState::HARD) && (cPixel != BORDER)) // in one step
							*realArea += 1.0;

						iGrid->set(i, j, state);

					}
					else
						if (cPixel != CellState::HARD)
							iGrid->set(i, j, state);
				}
			}
		}
	}
	else
	{
		region.from.x = int(iEllipse.minX()) - 2;
		region.to.x = int(iEllipse.maxX()) + 2;

		if (region.from.x < 0)
			region.from.x = 0;

		if (region.to.x > iGrid->width)
			region.to.x = iGrid->width;

		double lamda = -tan(iEllipse.slope);
		double d = iEllipse.b / cos(iEllipse.slope);

#pragma omp for

		for (i = region.from.x; i < region.to.x; i++)
		{

			region.from.y = int(((i - iEllipse.center.x) * lamda) + iEllipse.center.y - d-1);
			region.to.y = int(((i - iEllipse.center.x) * lamda) + iEllipse.center.y + d+1);

			if (region.from.y > region.to.y)
			{
				int temp = region.from.y;
				region.from.y = region.to.y;
				region.to.y = temp;

			}

			if (region.from.y < 0)
				region.from.y = 0;

			if (region.to.y > iGrid->height)
				region.to.y = iGrid->height;

			for (j = region.from.y ; j < region.to.y ; j++)
			{

				Point rotated = Point::transform(i + 0.5, j + 0.5, iEllipse.center.x, iEllipse.center.y, -iEllipse.slope);
				//if rotated inside circle
				distance = ((rotated.x - iEllipse.center.x)*(rotated.x - iEllipse.center.x) / (iEllipse.a*iEllipse.a)) + ((rotated.y - iEllipse.center.y)*(rotated.y - iEllipse.center.y) / (iEllipse.b* iEllipse.b));

				//iGrid->set(i, j, CellState::SOFT);
				if (distance <= 1.0)
				{
					char cPixel = iGrid->get(i, j);
					if (state == CellState::HARD)
					{

						if ((cPixel != 0) && (cPixel != CellState::HARD) && (cPixel != BORDER)) // in one step
							*realArea += 1.0;

						iGrid->set(i, j, state);

					}
					else
						if (cPixel != CellState::HARD)
							iGrid->set(i, j, state);
				}
			}
		}
	}


}*/


std::vector<std::string> ShapeGenerator::split(std::string strToSplit, char delimeter)
{
	std::stringstream ss(strToSplit);
	std::string item;
	std::vector<std::string> splittedStrings;
	while (std::getline(ss, item, delimeter))
	{
		splittedStrings.push_back(item);
	}
	return splittedStrings;
}


//Read input file
void ShapeGenerator::readFromFile(char* inputFileName)
{
	string line;
	ifstream inputFile(inputFileName);

	if (inputFile.is_open())
	{
		while (getline(inputFile, line))
		{
			//skip line if starts with #
			while (line[0] == '#')
			{
				getline(inputFile, line);
			}

			//Read project Name
			projectName = line;


			getline(inputFile, line);

			//skip line if starts with #
			while (line[0] == '#')
			{
				getline(inputFile, line);
			}
			totalComponents = std::stoi(line);

			//--------------------------------------
			getline(inputFile, line);
			while (line[0] == '#')
			{
				getline(inputFile, line);
			}

			//read % weight of components
			std::vector<std::string> strComp = split(line, ' ');
			for (int i = 0; i < totalComponents; i++)
			{
				components[i] = std::stod(strComp[i]);
			}

			getline(inputFile, line);
			while (line[0] == '#')
			{
				getline(inputFile, line);
			}

			//read for components the type (Circle, Rectangle , ellipse ..)
			std::vector<std::string> strCompType = split(line, ' ');
			for (int i = 0; i < totalComponents; i++)
			{
				componentsType[i] = ShapeType(std::stoi(strCompType[i]));
			}


			getline(inputFile, line);
			while (line[0] == '#')
			{
				getline(inputFile, line);
			}

			//read for components the size type (Constant , Variable)
			std::vector<std::string> strCompSizeType = split(line, ' ');
			for (int i = 0; i < totalComponents; i++)
			{
				componentsSizeType[i] = SizeType(std::stoi(strCompSizeType[i]));
			}

			//

			getline(inputFile, line);
			while (line[0] == '#')
			{
				getline(inputFile, line);
			}

			//read for each component special weight 
			std::vector<std::string> strWeights = split(line, ' ');
			for (int i = 0; i < totalComponents; i++)
			{
				specialWeights[i] = std::stod(strWeights[i]);
			}


			getline(inputFile, line);
			while (line[0] == '#')
			{
				getline(inputFile, line);
			}

			//read for each component first dimension 
			std::vector<std::string> strdimX = split(line, ' ');
			for (int i = 0; i < totalComponents; i++)
			{
				dimensionX[i] = std::stod(strdimX[i]);
			}


			getline(inputFile, line);
			while (line[0] == '#')
			{
				getline(inputFile, line);
			}

			//read for each component second dimension 
			std::vector<std::string> strdimY = split(line, ' ');
			for (int i = 0; i < totalComponents; i++)
			{
				dimensionY[i] = std::stod(strdimY[i]);
			}

			//read for each component hoops distances
			getline(inputFile, line);
			//
			while (line[0] == '#')
			{
				getline(inputFile, line);
			}

			std::vector<std::string> strHoops = split(line, ' ');
			for (int i = 0; i < totalComponents; i++)
			{
				hoops[i] = std::stod(strHoops[i]);
			}


			////read for each component electric conductivity 
			getline(inputFile, line);
			//
			while (line[0] == '#')
			{
				getline(inputFile, line);
			}

			std::vector<std::string> streConductivity = split(line, ' ');
			for (int i = 0; i < totalComponents; i++)
			{
				materialsElectricConductivity[i] = std::stod(streConductivity[i]);
			}

			////read for each component thermal conductivity 
			getline(inputFile, line);
			//
			while (line[0] == '#')
			{
				getline(inputFile, line);
			}

			std::vector<std::string> streThermalConductivity = split(line, ' ');
			for (int i = 0; i < totalComponents; i++)
			{
				materialsThermalConductivity[i] = std::stod(streThermalConductivity[i]);
			}

			////read for each component Young modulus 
			getline(inputFile, line);
			//
			while (line[0] == '#')
			{
				getline(inputFile, line);
			}

			std::vector<std::string> streYoungModulus = split(line, ' ');
			for (int i = 0; i < totalComponents; i++)
			{
				materialsYoungModulus[i] = std::stod(streYoungModulus[i]);
			}


			////read for each component Poisson Ratio 

			getline(inputFile, line);
			//
			while (line[0] == '#')
			{
				getline(inputFile, line);
			}

			std::vector<std::string> strPoissonRatio = split(line, ' ');
			for (int i = 0; i < totalComponents; i++)
			{
				materialsPoissonRatio[i] = std::stod(strPoissonRatio[i]);
			}

			getline(inputFile, line);
			//
			while (line[0] == '#')
			{
				getline(inputFile, line);
			}

			std::vector<std::string> strSizes = split(line, ' ');
			width = std::stoi(strSizes[0]);
			height = std::stoi(strSizes[1]);

			getline(inputFile, line);
			while (line[0] == '#')
			{
				getline(inputFile, line);
			}


			iterations = std::stoi(line);

			getline(inputFile, line);
			while (line[0] == '#')
			{
				getline(inputFile, line);
			}


			pixelsPerMinimumCircle = std::stoi(line);
			getline(inputFile, line);

			while (line[0] == '#')
			{
				getline(inputFile, line);
			}


			if ((line == "TRUE") || (line == "true"))
				swissCheese = true;
			else
				swissCheese = false;


			//Calc Electric conductivity
			getline(inputFile, line);
			while (line[0] == '#')
			{
				getline(inputFile, line);
			}


			if ((line == "TRUE") || (line == "true"))
				calcElectricConductivity = true;
			else
				calcElectricConductivity = false;

			//Calc Electric conductivity with finite differences
			getline(inputFile, line);
			while (line[0] == '#')
			{
				getline(inputFile, line);
			}


			if ((line == "TRUE") || (line == "true"))
				calcElectricConductivityWithFDM = true;
			else
				calcElectricConductivityWithFDM = false;



			//Calc Statistics
			getline(inputFile, line);
			while (line[0] == '#')
			{
				getline(inputFile, line);
			}


			if ((line == "TRUE") || (line == "true"))
				calcStatistcs = true;
			else
				calcStatistcs = false;


		}

		inputFile.close();

		//initialize


		min = min_element(dimensionY, 0, totalComponents); 
		//minimum element
		max = max_element(dimensionX, 0, totalComponents);
		
		//pixelsPerMinimumCircle = sqrt(width*min / max);
		//pixelsPerMinimumCircle =38;

		factor = pixelsPerMinimumCircle / min;
	}
	else

	{
		cout << "Error cannot find " << *inputFileName << "Program halted..." << '\n';
		char resp;
		cin.get(resp);
		exit(-1);
	}
}

void ShapeGenerator::exportForFDM(char* fileName)
{
	ofstream File;
	//string FileName = "microstructure.txt";
	File.open(fileName);

	for (int j = 2; j <= this->grid->width + 1; j++)
		for (int i = 2; i <= this->grid->height + 1; i++)
		{
			int m = (j - 2) * this->grid->width + (i - 2);
			File << int(this->grid->ingadients[m]) + 1 << "\n";

		}

	File.close();
}
