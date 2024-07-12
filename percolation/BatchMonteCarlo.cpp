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
	cout << "\nEXE PATH " << exePath;
	settings.readFromFile("settings.txt");
	cout << "\nReaded Setings ";
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
	cout << "-----------------------------------------------------------------------------------------\n";
	cout << "Input File: " << fileName << "\n";
	cout << "\n";
	cout << "-----------------------------------------------------------------------------------------\n";
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
	cout << endl << "Running " << shapes->width << "x" << shapes->height << " grids " << shapes->iterations << " times " << endl;

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
	//C++ 17
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

	/* <C++ 17
	DIR* dir;
	int count = 0;
	struct dirent* ent;

	char* cDirectory = &directory[0u];
	if ((dir = opendir(cDirectory)) != NULL) {
		//print all the files and directories within directory 
		while ((ent = readdir(dir)) != NULL) {
			inputfiles[count] = ent->d_name;
			count++;
		}
		closedir(dir);
		total = count;
	}
	else {
		// could not open directory 
		perror("");
		return EXIT_FAILURE;
	}
	return 0;
	*/
}

void BatchMonteCarlo::show(void)
{
	cout << "\n";
	cout << "-----------------------------------------------------------------------------------------\n";
	cout << "Files in working directory " << directory << "\n";
	for (int i = 0; i < total; i++)
		cout << inputfiles[i] << "\n";
	cout << "\n";
	cout << "-----------------------------------------------------------------------------------------\n";
	return;
}

void BatchMonteCarlo::Run()
{
	show();
	for (int i = 0; i < total; i++)
	{
		//string inputfile = exePath.generic_string() + "/" + directory + "/" + inputfiles[i];
		string inputfile = exePath.generic_string() + "/" + inputfiles[i];
		if (inputfiles[i][0]!='.')
			singleRun(inputfile, &iShapes[i]);
	}

	saveResults();
	saveResultsWithSemicolon();
}

void BatchMonteCarlo::saveResultsWithSemicolon(void)
{


	time_t t = time(0);   // get time now
	struct tm* now = localtime(&t);

	char fileName[80];
	strftime(fileName, 80, "%Y-%m-%d %H %M %S-Semicolon Seperated.csv", now);

	std::ofstream componentFile;


	componentFile.open(fileName);


	componentFile << "-------------------------------------------------------------------------------------------------------------------------\n";
	componentFile << info.program;
	componentFile << info.version;
	componentFile << info.date;
	componentFile << info.author;
	componentFile << info.licence;
	componentFile << "-------------------------------------------------------------------------------------------------------------------------\n";
	componentFile << "Start at " << NowToString() << "\n";


	componentFile << "Case;Mean Percolation;";
	if (this->iShapes[2].calcStatistcs)
		componentFile << "Max Cluster Radius ;Correlation Length;";

	if (this->iShapes[2].calcElectricConductivity)
		componentFile << "Electric Conductivity; Thermal Conductivity; Young Modulus; Poisson Ratio ;Total Conductive Paths;Mean Conductive Length;";

	if (this->iShapes[2].calcElectricConductivityWithFDM)
		componentFile << "FDM Ix;FDM Iy;FDM ro;";

	componentFile << "Process Time; Preperation Time; Grid(X); Grid(Y);  ppms;";

	for (int j = 0; j < iShapes[2].totalComponents; j++)
		componentFile << "% Area_" << j << ";";
	for (int j = 0; j < iShapes[2].totalComponents; j++)
		componentFile << "Size X_" << j << ";";

	for (int j = 0; j < iShapes[2].totalComponents; j++)
		componentFile << "Size Y_" << j << ";";
	for (int j = 0; j < iShapes[2].totalComponents; j++)
		componentFile << "Hoop" << j << ";";

	componentFile << "\n";


	for (int i = 2; i < total; i++)
	{
		double meanPaths = average_element(iShapes[i].paths, 0, iShapes[i].iterations);
		double meanPathsLength = average_element(iShapes[i].meanPathLength, 0, iShapes[i].iterations);
		componentFile << iShapes[i].projectName << ";" << iShapes[i].meanPercolation << ";";
		if (this->iShapes[2].calcStatistcs)
			componentFile << casesMeanMaxClusterRadius[i - 2] << ";" << iShapes[i].correleationLength << ";";
		if (this->iShapes[2].calcElectricConductivity)
			componentFile << iShapes[i].meanElectricConductivity << ";" << iShapes[i].meanThermalConductivity << ";"
			<< iShapes[i].meanYoungModulus << ";" << iShapes[i].meanPoissonRatio << ";" << meanPaths << ";" << meanPathsLength << ";";

		componentFile << iShapes[i].meanSetUpTime << ";" << iShapes[i].meanTime << ";" << iShapes[i].width << ";" << iShapes[i].height << ";"
			<< iShapes[i].pixelsPerMinimumCircle;

		for (int j = 0; j < iShapes[i].totalComponents; j++)
			componentFile << ";" << iShapes[i].componentsArea[j];
		for (int j = 0; j < iShapes[i].totalComponents; j++)
			componentFile << ";" << iShapes[i].dimensionX[j];

		for (int j = 0; j < iShapes[i].totalComponents; j++)
			componentFile << ";" << iShapes[i].dimensionY[j];

		for (int j = 0; j < iShapes[2].totalComponents; j++)
			componentFile << ";" << iShapes[i].hoops[j];
		componentFile << "\n";

	}

	componentFile.close();
}


void BatchMonteCarlo::saveResults(void)
{


	time_t t = time(0);   // get time now
	struct tm* now = localtime(&t);

	char fileName[80];
	strftime(fileName, 80, "%Y-%m-%d %H %M %S-Comma Seperated.csv", now);

	std::ofstream componentFile;


	componentFile.open(fileName);


	componentFile << "-------------------------------------------------------------------------------------------------------------------------\n";
	componentFile << info.program;
	componentFile << info.version;
	componentFile << info.date;
	componentFile << info.author;
	componentFile << info.licence;
	componentFile << "-------------------------------------------------------------------------------------------------------------------------\n";
	componentFile << "Start at " << NowToString() << "\n";


	componentFile << "Case,Mean Percolation,";
	if (this->iShapes[2].calcStatistcs)
		componentFile << "Max Cluster Radius ,Correlation Length,";

	if (this->iShapes[2].calcElectricConductivity)
		componentFile << "Electric Conductivity, Thermal Conductivity, Young Modulus, Poisson Ratio ,Total Conductive Paths,Mean Conductive Length,";

	if (this->iShapes[2].calcElectricConductivityWithFDM)
		componentFile << "FDM Ix,FDM Iy,FDM ro,";

	componentFile << "Process Time, Preperation Time, Grid(X), Grid(Y),  ppms,";

	for (int j = 0; j < iShapes[2].totalComponents; j++)
		componentFile << "% Area_" << j << ",";
	for (int j = 0; j < iShapes[2].totalComponents; j++)
		componentFile << "Size X_" << j << ",";

	for (int j = 0; j < iShapes[2].totalComponents; j++)
		componentFile << "Size Y_" << j << ",";
	for (int j = 0; j < iShapes[2].totalComponents; j++)
		componentFile << "Hoop" << j << ",";

	componentFile << "\n";


	for (int i = 2; i < total; i++)
	{
		double meanPaths = average_element(iShapes[i].paths, 0, iShapes[i].iterations);
		double meanPathsLength = average_element(iShapes[i].meanPathLength, 0, iShapes[i].iterations);
		componentFile << iShapes[i].projectName << "," << iShapes[i].meanPercolation << ",";
		if (this->iShapes[2].calcStatistcs)
			componentFile << casesMeanMaxClusterRadius[i - 2] << "," << iShapes[i].correleationLength << ",";
		if (this->iShapes[2].calcElectricConductivity)
			componentFile << iShapes[i].meanElectricConductivity << "," << iShapes[i].meanThermalConductivity << ","
			<< iShapes[i].meanYoungModulus << "," << iShapes[i].meanPoissonRatio << "," << meanPaths << "," << meanPathsLength << ",";

		componentFile << iShapes[i].meanSetUpTime << "," << iShapes[i].meanTime << "," << iShapes[i].width << "," << iShapes[i].height << ","
			<< iShapes[i].pixelsPerMinimumCircle;

		for (int j = 0; j < iShapes[i].totalComponents; j++)
			componentFile << "," << iShapes[i].componentsArea[j];
		for (int j = 0; j < iShapes[i].totalComponents; j++)
			componentFile << "," << iShapes[i].dimensionX[j];

		for (int j = 0; j < iShapes[i].totalComponents; j++)
			componentFile << "," << iShapes[i].dimensionY[j];

		for (int j = 0; j < iShapes[2].totalComponents; j++)
			componentFile << "," << iShapes[i].hoops[j];
		componentFile << "\n";

	}

	componentFile.close();
}