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
1. E. Lambrou and L. N. Gergidis, “A computational method for calculating the electrical and thermal properties of random composite” ,
Physica A: Statistical Mechanics and its Applications, Volume 642, 2024, 129760, ISSN 0378-4371,
https://doi.org/10.1016/j.physa.2024.129760
2. E. Lambrou and L. N. Gergidis, “A particle digitization-based computational method for continuum percolation,” Physica A: Statistical Mechanics
and its Applications, vol. 590, p. 126738, 2022

if you use this programm and write a paper or report please cite above papers

*/

#include "BatchMonteCarlo.h"

/// <summary>
/// main routine with default input path
/// </summary>
/// <param name=""></param>
BatchMonteCarlo::BatchMonteCarlo(void)
{
	//input directory
	directory = "inputs";

	exePath = std::filesystem::current_path();
	//cout << "\nEXE PATH " << exePath;
	settings.readFromFile("settings.txt");
	//cout << "\nReaded Setings ";
}

/// <summary>
/// main routine with input path
/// </summary>
/// <param name="idirectory">input path</param>
BatchMonteCarlo::BatchMonteCarlo(string idirectory)
{
	directory = idirectory;
	exePath = std::filesystem::current_path(); 
	settings.readFromFile("settings.txt");
}

/// <summary>
/// single Run of the algorithm (One Problem)
/// </summary>
/// <param name="fileName"></param>
/// <param name="shapes"></param>
void BatchMonteCarlo::singleRun(string fileName, ShapeGenerator* shapes)
{
	clock_t start = clock();

	char* cInputFile = &fileName[0u];
	cout << "\n";
	//cout << "--------------------------------------------------------------------------------------------------------------\n";
	cout << "Input File: " << fileName << "\n";
	cout << "\n";
	cout << "--------------------------------------------------------------------------------------------------------------\n";
	shapes->readFromFile(cInputFile);

	//create output dirs for this project
	shapes->initDirs();

	//Solve area of all components from % weights
	shapes->areaSolve();

	cout << "Running for " << shapes->width << "x" << shapes->height << " Grid \n";

	//initialize random


	//initialize grid width x height
	Grid grid(shapes->width, shapes->height);

	//point grid to shapes.grid
	shapes->grid = &grid;
	//point 
	shapes->settings = &settings;

	start = clock();

	//Start Monte Carlo
	shapes->monteCarlo();

	double sumMaxClusterRadius = 0;
	double avgMaxClusterRadius = 0;

	//calc sumMaxClusterRadius
	for (size_t i = 0; i < grid.cMaxClusterRadius.size(); i++)
		sumMaxClusterRadius = sumMaxClusterRadius + grid.cMaxClusterRadius[i];

	avgMaxClusterRadius = sumMaxClusterRadius / grid.cMaxClusterRadius.size();


	casesMeanMaxClusterRadius.push_back(avgMaxClusterRadius);
	// Debug cout << "avgMaxClusterRadius=" << avgMaxClusterRadius <<"\n";

	//grid.cMaxClusterRadius.clear();
	cout << endl << "Running " << shapes->width << "x" << shapes->height << " grid " << shapes->iterations << " times " << endl;

	clock_t end = clock();
	double cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;

	//Save results
	shapes->Report();
	shapes->ReportWithSemicolon();
	shapes->ReportStatistics();

	cout << "Total time =" << cpu_time_used << "sec or " << cpu_time_used / 60 << "min or " << cpu_time_used / 3600 << "h \n";
	cout << "Finish at " << NowToString();
}

int BatchMonteCarlo::getInputFiles(void)
{
	//C++ 17*
	total = 0;
	
	for (const auto& file : std::filesystem::directory_iterator(directory)) 
	{
		inputfiles[total] =  file.path().generic_string();
		//cout << "\n Debug_2:" << inputfiles[count] << "\n";
		//cout << "\n Debug:"<<file.path().generic_string() << "\n";
		total++;
		
	}

	if (total > 0)
		return 0;
	else
		return EXIT_FAILURE;
		
}

void BatchMonteCarlo::show(void)
{
	cout << "\n";
	cout << "--------------------------------------------------------------------------------------------------------------\n";
	cout << "Files in working directory " << directory << "\n";
	for (int i = 0; i < total; i++)
		cout << inputfiles[i] << "\n";
	cout << "\n";
	cout << "--------------------------------------------------------------------------------------------------------------\n";
	return;
}

void BatchMonteCarlo::Run()
{
	show();
	
	char fileNameComma[80];
	char fileNameSemicolon[80];
	bool headerSaved = false;
	
	
	for (int i = 0; i < total; i++)
	{
		//string inputfile = exePath.generic_string() + "/" + directory + "/" + inputfiles[i];
		string inputfile = exePath.generic_string() + "/" + inputfiles[i];
		if (inputfiles[i][0] != '.') 
		{
			singleRun(inputfile, &iShapes[i]);
			
			if (!headerSaved)
			{
				headerSaved = true;
				singlRunsaveResultstoReportHeader(ReportType::COMMA, &fileNameComma[0]);
				singlRunsaveResultstoReportHeader(ReportType::SEMICOLON, &fileNameSemicolon[0]);
			}

			singlRunsaveResultstoReport(ReportType::COMMA, &fileNameComma[0], i);
			singlRunsaveResultstoReport(ReportType::SEMICOLON, &fileNameSemicolon[0], i);
		}
	}

	//saveResults();
	//saveResultsWithSemicolon();
}

void BatchMonteCarlo::singlRunsaveResultstoReportHeader(ReportType ireportType, char* fileName)
{
	string seperator;
	time_t t = time(0);   // get time now
	struct tm* now = localtime(&t);

	//char fileName[80];
	std::ofstream componentFile;

	if (ireportType == SEMICOLON)
	{
		seperator = ";";
		strftime(fileName, 80, "Report %Y-%m-%d %H %M %S-Semicolon.csv", now);
		std::locale cpploc{ "" };
		componentFile.imbue(cpploc);
		componentFile.open(fileName);
	}
	else
	{
		seperator = ",";
		strftime(fileName, 80, "Report %Y-%m-%d %H %M %S-Comma.csv", now);
		componentFile.open(fileName);
	}

	componentFile << "-------------------------------------------------------------------------------------------------------------------------\n";
	componentFile << info.program;
	componentFile << info.version;
	componentFile << info.date;
	componentFile << info.author;
	componentFile << info.licence;
	componentFile << "-------------------------------------------------------------------------------------------------------------------------\n";
	componentFile << "Start at " << NowToString() << "\n";


	componentFile << "Case" << seperator << "Mean Percolation Probability" << seperator;
	if (this->iShapes[0].calcStatistcs)
		componentFile << "Max Cluster Radius" << seperator << "Correlation" << seperator << "Length" << seperator;

	if (this->iShapes[0].calcElectricConductivity)
		componentFile << "Electric Conductivity" << seperator << "Thermal Conductivity" << seperator << "Young Modulus"
		<< seperator << "Poisson Ratio" << seperator << "Total Conductive Paths" << seperator << "Mean Conductive Length" << seperator
		<< "Log Electric Conductivity" << seperator << "Log Thermal Conductivity" << seperator << "Log Young Modulus"
		<< seperator << "Log Poisson Ratio" << seperator << "Log Total Conductive Paths" << seperator << "Log Mean Conductive Length" << seperator;

	if (this->iShapes[0].calcElectricConductivityWithFDM)
		componentFile << "FDM Ix" << seperator << "FDM Iy" << seperator << "FDM ro" << seperator;

	componentFile << "Process Time" << seperator << "Preperation Time" << seperator << "Grid(X)" << seperator << "Grid(Y)" << seperator << "ppms" << seperator;

	for (int j = 0; j < iShapes[0].totalComponents; j++)
		componentFile << "% Area - " << j << seperator;
	for (int j = 0; j < iShapes[0].totalComponents; j++)
		componentFile << "% Real Area - " << j << seperator;
	for (int j = 0; j < iShapes[0].totalComponents; j++)
		componentFile << "Size X -" << j << seperator;

	for (int j = 0; j < iShapes[0].totalComponents; j++)
		componentFile << "Size Y -" << j << seperator;
	for (int j = 0; j < iShapes[0].totalComponents; j++)
		componentFile << "Hoop Size - " << j << seperator;

	componentFile << "\n";
	componentFile.close();
	return;

}

void  BatchMonteCarlo::singlRunsaveResultstoReport(ReportType ireportType, char* fileName,int i)
{
	string seperator;
	std::ofstream componentFile;
	if (ireportType == SEMICOLON)
	{
		seperator = ";";
		std::locale cpploc{ "" };
		componentFile.imbue(cpploc);
	}
	
	else
		seperator = ",";

	componentFile.open(fileName, std::ios_base::app);
	double meanPaths = average_element(iShapes[i].paths, 0, iShapes[i].iterations);
	double meanPathsLength = average_element(iShapes[i].meanPathLength, 0, iShapes[i].iterations);
	componentFile << iShapes[i].projectName << seperator << iShapes[i].meanPercolation << seperator;
	if (this->iShapes[0].calcStatistcs)
		componentFile << casesMeanMaxClusterRadius[i] << seperator << iShapes[i].correleationLength << seperator;
	if (this->iShapes[0].calcElectricConductivity)
		componentFile << iShapes[i].meanElectricConductivity << seperator << iShapes[i].meanThermalConductivity << seperator
		<< iShapes[i].meanYoungModulus << seperator << iShapes[i].meanPoissonRatio << seperator << meanPaths << seperator << meanPathsLength << seperator
		<< log(iShapes[i].meanElectricConductivity) << seperator << log(iShapes[i].meanThermalConductivity) << seperator
		<< log(iShapes[i].meanYoungModulus) << seperator << log(iShapes[i].meanPoissonRatio) << seperator << log(meanPaths) << seperator << log(meanPathsLength) << seperator;
	if (this->iShapes[0].calcElectricConductivityWithFDM)
		componentFile << setw(13) << iShapes[i].FDResults[2 * i] << seperator << setw(13) << iShapes[i].FDResults[2 * i + 1] << seperator << setw(13) << (1 / iShapes[i].FDResults[2 * i]) << seperator;

	componentFile << iShapes[i].meanSetUpTime << seperator << iShapes[i].meanTime << seperator << iShapes[i].width << seperator << iShapes[i].height << seperator
		<< iShapes[i].pixelsPerMinimumSize;

	for (int j = 0; j < iShapes[i].totalComponents; j++)
		componentFile << seperator << iShapes[i].componentsArea[j];
	for (int j = 0; j < iShapes[i].totalComponents; j++)
		componentFile << seperator << iShapes[i].realComponentAreas[j];
	for (int j = 0; j < iShapes[i].totalComponents; j++)
		componentFile << seperator << iShapes[i].dimensionX[j];

	for (int j = 0; j < iShapes[i].totalComponents; j++)
		componentFile << seperator << iShapes[i].dimensionY[j];

	for (int j = 0; j < iShapes[i].totalComponents; j++)
		componentFile << seperator << iShapes[i].hoops[j];
	
	componentFile << "\n";

	componentFile.close();
}

 

void BatchMonteCarlo::saveResultstoReport(ReportType ireportType)
{	string seperator;
	time_t t = time(0);   // get time now
	struct tm* now = localtime(&t);

	char fileName[80];
	std::ofstream componentFile;

	if (ireportType == SEMICOLON)
	{
		seperator = ";";
		strftime(fileName, 80, "Report %Y-%m-%d %H %M %S-Semicolon.csv", now);
		std::locale cpploc{ "" };
		componentFile.imbue(cpploc);
		componentFile.open(fileName);
	}
	else
	{
		seperator = ",";
		strftime(fileName, 80, "Report %Y-%m-%d %H %M %S-Comma.csv", now);
		componentFile.open(fileName);
	}
	
	componentFile << "-------------------------------------------------------------------------------------------------------------------------\n";
	componentFile << info.program;
	componentFile << info.version;
	componentFile << info.date;
	componentFile << info.author;
	componentFile << info.licence;
	componentFile << "-------------------------------------------------------------------------------------------------------------------------\n";
	componentFile << "Start at " << NowToString() << "\n";


	componentFile << "Case" << seperator << "Mean Percolation Probability" << seperator;
	if (this->iShapes[0].calcStatistcs)
		componentFile << "Max Cluster Radius" << seperator << "Correlation" << seperator << "Length" << seperator;

	if (this->iShapes[0].calcElectricConductivity)
		componentFile << "Electric Conductivity" << seperator << "Thermal Conductivity" << seperator << "Young Modulus"
		<< seperator<< "Poisson Ratio" << seperator << "Total Conductive Paths" << seperator << "Mean Conductive Length"<< seperator
		<< "Log Electric Conductivity" << seperator << "Log Thermal Conductivity" << seperator << "Log Young Modulus"
		<< seperator << "Log Poisson Ratio" << seperator << "Log Total Conductive Paths" << seperator << "Log Mean Conductive Length" << seperator;

	if (this->iShapes[0].calcElectricConductivityWithFDM)
		componentFile << "FDM Ix" << seperator<< "FDM Iy" << seperator << "FDM ro" << seperator;

	componentFile << "Process Time" << seperator <<  "Preperation Time" << seperator << "Grid(X)" << seperator <<  "Grid(Y)" << seperator << "ppms" << seperator;

	for (int j = 0; j < iShapes[0].totalComponents; j++)
		componentFile << "% Area - " << j << seperator;
	for (int j = 0; j < iShapes[0].totalComponents; j++)
		componentFile << "% Real Area - " << j << seperator;
	for (int j = 0; j < iShapes[0].totalComponents; j++)
		componentFile << "Size X -" << j << seperator;

	for (int j = 0; j < iShapes[0].totalComponents; j++)
		componentFile << "Size Y -" << j << seperator;
	for (int j = 0; j < iShapes[0].totalComponents; j++)
		componentFile << "Hoop Size - " << j << seperator;

	componentFile << "\n";


	for (int i = 0; i < total; i++)
	{
		double meanPaths = average_element(iShapes[i].paths, 0, iShapes[i].iterations);
		double meanPathsLength = average_element(iShapes[i].meanPathLength, 0, iShapes[i].iterations);
		componentFile << iShapes[i].projectName << seperator << iShapes[i].meanPercolation << seperator;
		if (this->iShapes[0].calcStatistcs)
			componentFile << casesMeanMaxClusterRadius[i] << seperator << iShapes[i].correleationLength << seperator;
		if (this->iShapes[0].calcElectricConductivity)
			componentFile << iShapes[i].meanElectricConductivity << seperator << iShapes[i].meanThermalConductivity << seperator
			<< iShapes[i].meanYoungModulus << seperator << iShapes[i].meanPoissonRatio << seperator << meanPaths << seperator << meanPathsLength << seperator
			<< log(iShapes[i].meanElectricConductivity) << seperator << log(iShapes[i].meanThermalConductivity) << seperator
			<< log(iShapes[i].meanYoungModulus) << seperator << log(iShapes[i].meanPoissonRatio) << seperator << log(meanPaths) << seperator << log(meanPathsLength) << seperator;
		if (this->iShapes[0].calcElectricConductivityWithFDM)
			componentFile << setw(13) << iShapes[i].FDResults[2 * i] << seperator << setw(13) << iShapes[i].FDResults[2 * i + 1] << seperator << setw(13) << (1 / iShapes[i].FDResults[2 * i])<< seperator;

		componentFile << iShapes[i].meanSetUpTime << seperator << iShapes[i].meanTime << seperator << iShapes[i].width << seperator << iShapes[i].height << seperator
			<< iShapes[i].pixelsPerMinimumSize;

		for (int j = 0; j < iShapes[i].totalComponents; j++)
			componentFile << seperator << iShapes[i].componentsArea[j];
		for (int j = 0; j < iShapes[i].totalComponents; j++)
			componentFile << seperator << iShapes[i].realComponentAreas[j];
		for (int j = 0; j < iShapes[i].totalComponents; j++)
			componentFile << seperator << iShapes[i].dimensionX[j];

		for (int j = 0; j < iShapes[i].totalComponents; j++)
			componentFile << seperator << iShapes[i].dimensionY[j];

		for (int j = 0; j < iShapes[i].totalComponents; j++)
			componentFile << seperator << iShapes[i].hoops[j];
		componentFile << "\n";

	}

	componentFile.close();

}
void BatchMonteCarlo::saveResultsWithSemicolon(void)
{
	saveResultstoReport(SEMICOLON);
}

void BatchMonteCarlo::saveResults(void)
{
	saveResultstoReport(COMMA);
}
