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
GNU General Public License for more more details.

You should have received a copy of the GNU General Public License
along with Foobar.If not, see < https://www.gnu.org/licenses/>.

Theory of this is published in two papers:
1. E. Lambrou and L. N. Gergidis, “A computational method for calculating the electrical and thermal properties of random composite” ,
Physica A: Statistical Mechanics and its Applications, Volume 642, 2024, 129760, ISSN 0378-4371,
https://doi.org/10.1016/j.physa.2024.129760
2. E. Lambrou and L. N. Gergidis, “A particle digitization-based computational method for continuum percolation,” Physica A: Statistical Mechanics
and its Applications, vol. 590, p. 126738, 2022

if you use this programm and write a paper or report please cite above papers

*/

// Include the header file for the BatchMonteCarlo class
#include "BatchMonteCarlo.h"

/// <summary>
/// main routine with default input path
/// </summary>
/// <param name=""></param>
// Default constructor for BatchMonteCarlo
BatchMonteCarlo::BatchMonteCarlo(void)
{
	// Set the default input directory name
	directory = "inputs";

	// Get the current executable path
	exePath = std::filesystem::current_path();
	//cout << "\nEXE PATH " << exePath; // Debugging line (commented out)
	// Read settings from the "settings.txt" file
	settings.readFromFile("settings.txt");
	//cout << "\nReaded Setings "; // Debugging line (commented out)
}

/// <summary>
/// main routine with input path
/// </summary>
/// <param name="idirectory">input path</param>
// Constructor for BatchMonteCarlo that takes an input directory
BatchMonteCarlo::BatchMonteCarlo(string idirectory)
{
	// Set the input directory to the provided value
	directory = idirectory;
	// Get the current executable path
	exePath = std::filesystem::current_path();
	// Read settings from the "settings.txt" file
	settings.readFromFile("settings.txt");
}

/// <summary>
/// single Run of the algorithm (One Problem)
/// </summary>
/// <param name="fileName"></param>
/// <param name="shapes"></param>
// Performs a single Monte Carlo simulation run for a given input file and shape generator
void BatchMonteCarlo::singleRun(string fileName, ShapeGenerator* shapes)
{
	// Record the starting time of the simulation
	clock_t start = clock();

	// Convert string file name to char*
	char* cInputFile = &fileName[0u];
	cout << "\n";
	//cout << "--------------------------------------------------------------------------------------------------------------\n"; // Commented out formatting
	cout << "Input File: " << fileName << "\n"; // Display the current input file
	cout << "\n";
	cout << "--------------------------------------------------------------------------------------------------------------\n";
	// Read shape data from the input file
	shapes->readFromFile(cInputFile);

	// Create output directories for this project
	shapes->initDirs();

	// Solve for the area of all components based on their percentage weights
	shapes->areaSolve();

	// Display the grid dimensions
	cout << "Running for " << shapes->width << "x" << shapes->height << " Grid \n";

	// Initialize random number generator (if not already done)

	// Initialize the grid with specified width and height
	Grid grid(shapes->width, shapes->height);

	// Point the shapes' grid pointer to the newly created grid
	shapes->grid = &grid;
	// Point the shapes' settings pointer to the global settings
	shapes->settings = &settings;

	// Reset the start time before the Monte Carlo simulation begins
	start = clock();

	// Start the Monte Carlo simulation
	shapes->monteCarlo();

	double sumMaxClusterRadius = 0;
	double avgMaxClusterRadius = 0;

	// Calculate the sum of maximum cluster radii
	for (size_t i = 0; i < grid.cMaxClusterRadius.size(); i++)
		sumMaxClusterRadius = sumMaxClusterRadius + grid.cMaxClusterRadius[i];

	// Calculate the average maximum cluster radius
	avgMaxClusterRadius = sumMaxClusterRadius / grid.cMaxClusterRadius.size();

	// Store the average maximum cluster radius for this case
	casesMeanMaxClusterRadius.push_back(avgMaxClusterRadius);
	// Debug cout << "avgMaxClusterRadius=" << avgMaxClusterRadius <<"\n"; // Debugging line (commented out)

	//grid.cMaxClusterRadius.clear(); // Clear cluster radius data (commented out)
	// Display simulation progress
	cout << endl << "Running " << shapes->width << "x" << shapes->height << " grid " << shapes->iterations << " times " << endl;

	// Record the ending time of the simulation
	clock_t end = clock();
	// Calculate the CPU time used
	double cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;

	// Save simulation results
	shapes->Report();             // Save general report
	shapes->ReportWithSemicolon(); // Save report with semicolon delimiter
	shapes->ReportStatistics();    // Save statistical report

	// Display total time taken
	cout << "Total time =" << cpu_time_used << "sec or " << cpu_time_used / 60 << "min or " << cpu_time_used / 3600 << "h \n";
	// Display finish time
	cout << "Finish at " << NowToString();
}

// Gets all input files from the specified directory
int BatchMonteCarlo::getInputFiles(void)
{
	//C++ 17*
	total = 0; // Initialize total file count to 0

	// Iterate through all files in the directory
	for (const auto& file : std::filesystem::directory_iterator(directory))
	{
		// Store the generic string path of the file
		inputfiles[total] = file.path().generic_string();
		//cout << "\n Debug_2:" << inputfiles[count] << "\n"; // Debugging line (commented out)
		//cout << "\n Debug:"<<file.path().generic_string() << "\n"; // Debugging line (commented out)
		total++; // Increment the total file count
	}

	// Return 0 if files were found, otherwise return failure
	if (total > 0)
		return 0;
	else
		return EXIT_FAILURE;

}

// Displays the list of input files
void BatchMonteCarlo::show(void)
{
	cout << "\n";
	cout << "--------------------------------------------------------------------------------------------------------------\n";
	cout << "Files in working directory " << directory << "\n"; // Display the working directory
	// Iterate and display each input file
	for (int i = 0; i < total; i++)
		cout << inputfiles[i] << "\n";
	cout << "\n";
	cout << "--------------------------------------------------------------------------------------------------------------\n";
	return;
}

// Runs all Monte Carlo simulations in the batch
void BatchMonteCarlo::Run()
{
	show(); // Display the list of input files

	char fileNameComma[80];      // Buffer for comma-separated report file name
	char fileNameSemicolon[80]; // Buffer for semicolon-separated report file name
	bool headerSaved = false;     // Flag to ensure header is saved only once


	for (int i = 0; i < total; i++) // Loop through each input file
	{
		// Construct the full path to the input file
		string inputfile = exePath.generic_string() + "/" + inputfiles[i];
		// Check if the file is not a hidden file (doesn't start with '.')
		if (inputfiles[i][0] != '.')
		{
			// Run a single simulation for the current input file
			singleRun(inputfile, &iShapes[i]);

			// Save report headers only once
			if (!headerSaved)
			{
				headerSaved = true;
				singlRunsaveResultstoReportHeader(ReportType::COMMA, &fileNameComma[0]);       // Header for comma-separated
				singlRunsaveResultstoReportHeader(ReportType::SEMICOLON, &fileNameSemicolon[0]); // Header for semicolon-separated
			}

			// Save results for the current run to both report types
			singlRunsaveResultstoReport(ReportType::COMMA, &fileNameComma[0], i);       // Save to comma-separated
			singlRunsaveResultstoReport(ReportType::SEMICOLON, &fileNameSemicolon[0], i); // Save to semicolon-separated
		}
	}

	//saveResults();           // (Commented out)
	//saveResultsWithSemicolon(); // (Commented out)
}

// Saves the header of the report for a single run
void BatchMonteCarlo::singlRunsaveResultstoReportHeader(ReportType ireportType, char* fileName)
{
	string seperator; // Separator string (comma or semicolon)
	time_t t = time(0); // Get current time
	struct tm* now = localtime(&t); // Convert to local time

	//char fileName[80]; // (Commented out as fileName is passed as parameter)
	std::ofstream componentFile; // Output file stream

	if (ireportType == SEMICOLON) // If report type is semicolon-separated
	{
		seperator = ";"; // Set separator to semicolon
		// Format file name with date and time for semicolon report
		strftime(fileName, 80, "Report %Y-%m-%d %H %M %S-Semicolon.csv", now);
		std::locale cpploc{ "" }; // Set locale
		componentFile.imbue(cpploc); // Imbue locale for proper numeric formatting
		componentFile.open(fileName); // Open the file
	}
	else // If report type is comma-separated (default)
	{
		seperator = ","; // Set separator to comma
		// Format file name with date and time for comma report
		strftime(fileName, 80, "Report %Y-%m-%d %H %M %S-Comma.csv", now);
		componentFile.open(fileName); // Open the file
	}

	// Write program information and header
	componentFile << "-------------------------------------------------------------------------------------------------------------------------\n";
	componentFile << info.program;
	componentFile << info.version;
	componentFile << info.date;
	componentFile << info.author;
	componentFile << info.licence;
	componentFile << "-------------------------------------------------------------------------------------------------------------------------\n";
	componentFile << "Start at " << NowToString() << "\n";

	// Write column headers
	componentFile << "Case" << seperator << "Mean Percolation Probability" << seperator;
	if (this->iShapes[0].calcStatistcs) // If statistics calculation is enabled
		componentFile << "Max Cluster Radius" << seperator << "Correlation" << seperator << "Length" << seperator;

	if (this->iShapes[0].calcElectricConductivity) // If electric conductivity calculation is enabled
		componentFile << "Electric Conductivity" << seperator << "Thermal Conductivity" << seperator << "Young Modulus"
		<< seperator << "Poisson Ratio" << seperator << "Total Conductive Paths" << seperator << "Mean Conductive Length" << seperator
		<< "Log Electric Conductivity" << seperator << "Log Thermal Conductivity" << seperator << "Log Young Modulus"
		<< seperator << "Log Poisson Ratio" << seperator << "Log Total Conductive Paths" << seperator << "Log Mean Conductive Length" << seperator;

	if (this->iShapes[0].calcElectricConductivityWithFDM) // If FDM electric conductivity is enabled
		componentFile << "FDM Ix" << seperator << "FDM Iy" << seperator << "FDM ro" << seperator;

	componentFile << "Process Time" << seperator << "Preperation Time" << seperator << "Grid(X)" << seperator << "Grid(Y)" << seperator << "ppms" << seperator;

	// Write headers for component-specific properties
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

	componentFile << "\n"; // New line after headers
	componentFile.close(); // Close the file
	return;

}

// Saves results for a single run to the report file
void BatchMonteCarlo::singlRunsaveResultstoReport(ReportType ireportType, char* fileName, int i)
{
	string seperator;          // Separator string (comma or semicolon)
	std::ofstream componentFile; // Output file stream

	if (ireportType == SEMICOLON) // If report type is semicolon-separated
	{
		seperator = ";"; // Set separator to semicolon
		std::locale cpploc{ "" }; // Set locale
		componentFile.imbue(cpploc); // Imbue locale for proper numeric formatting
	}
	else
		seperator = ","; // Set separator to comma

	// Open the file in append mode
	componentFile.open(fileName, std::ios_base::app);
	// Calculate mean paths and mean path length
	double meanPaths = average_element(iShapes[i].paths, 0, iShapes[i].iterations);
	double meanPathsLength = average_element(iShapes[i].meanPathLength, 0, iShapes[i].iterations);
	// Write case name and mean percolation probability
	componentFile << iShapes[i].projectName << seperator << iShapes[i].meanPercolation << seperator;
	if (this->iShapes[0].calcStatistcs) // If statistics calculation is enabled
		componentFile << casesMeanMaxClusterRadius[i] << seperator << iShapes[i].correleationLength << seperator;
	if (this->iShapes[0].calcElectricConductivity) // If electric conductivity calculation is enabled
		componentFile << iShapes[i].meanElectricConductivity << seperator << iShapes[i].meanThermalConductivity << seperator
		<< iShapes[i].meanYoungModulus << seperator << iShapes[i].meanPoissonRatio << seperator << meanPaths << seperator << meanPathsLength << seperator
		<< log(iShapes[i].meanElectricConductivity) << seperator << log(iShapes[i].meanThermalConductivity) << seperator
		<< log(iShapes[i].meanYoungModulus) << seperator << log(iShapes[i].meanPoissonRatio) << seperator << log(meanPaths) << seperator << log(meanPathsLength) << seperator;
	if (this->iShapes[0].calcElectricConductivityWithFDM) // If FDM electric conductivity is enabled
		componentFile << setw(13) << iShapes[i].FDResults[2 * i] << seperator << setw(13) << iShapes[i].FDResults[2 * i + 1] << seperator << setw(13) << (1 / iShapes[i].FDResults[2 * i]) << seperator;

	// Write process time, preparation time, grid dimensions, and pixels per minimum size
	componentFile << iShapes[i].meanSetUpTime << seperator << iShapes[i].meanTime << seperator << iShapes[i].width << seperator << iShapes[i].height << seperator
		<< iShapes[i].pixelsPerMinimumSize;

	// Write component-specific area percentages
	for (int j = 0; j < iShapes[i].totalComponents; j++)
		componentFile << seperator << iShapes[i].componentsArea[j];
	// Write real component area percentages
	for (int j = 0; j < iShapes[i].totalComponents; j++)
		componentFile << seperator << iShapes[i].realComponentAreas[j];
	// Write component-specific X dimensions
	for (int j = 0; j < iShapes[i].totalComponents; j++)
		componentFile << seperator << iShapes[i].dimensionX[j];

	// Write component-specific Y dimensions
	for (int j = 0; j < iShapes[i].totalComponents; j++)
		componentFile << seperator << iShapes[i].dimensionY[j];

	// Write component-specific hoop sizes
	for (int j = 0; j < iShapes[i].totalComponents; j++)
		componentFile << seperator << iShapes[i].hoops[j];

	componentFile << "\n"; // New line for the next entry

	componentFile.close(); // Close the file
}

// Saves all results to a report file based on the specified report type
void BatchMonteCarlo::saveResultstoReport(ReportType ireportType)
{
	string seperator;          // Separator string (comma or semicolon)
	time_t t = time(0);         // Get current time
	struct tm* now = localtime(&t); // Convert to local time

	char fileName[80];          // Buffer for report file name
	std::ofstream componentFile; // Output file stream

	if (ireportType == SEMICOLON) // If report type is semicolon-separated
	{
		seperator = ";"; // Set separator to semicolon
		// Format file name with date and time for semicolon report
		strftime(fileName, 80, "Report %Y-%m-%d %H %M %S-Semicolon.csv", now);
		std::locale cpploc{ "" }; // Set locale
		componentFile.imbue(cpploc); // Imbue locale for proper numeric formatting
		componentFile.open(fileName); // Open the file
	}
	else // If report type is comma-separated (default)
	{
		seperator = ","; // Set separator to comma
		// Format file name with date and time for comma report
		strftime(fileName, 80, "Report %Y-%m-%d %H %M %S-Comma.csv", now);
		componentFile.open(fileName); // Open the file
	}

	// Write program information and header
	componentFile << "-------------------------------------------------------------------------------------------------------------------------\n";
	componentFile << info.program;
	componentFile << info.version;
	componentFile << info.date;
	componentFile << info.author;
	componentFile << info.licence;
	componentFile << "-------------------------------------------------------------------------------------------------------------------------\n";
	componentFile << "Start at " << NowToString() << "\n";

	// Write column headers
	componentFile << "Case" << seperator << "Mean Percolation Probability" << seperator;
	if (this->iShapes[0].calcStatistcs) // If statistics calculation is enabled
		componentFile << "Max Cluster Radius" << seperator << "Correlation" << seperator << "Length" << seperator;

	if (this->iShapes[0].calcElectricConductivity) // If electric conductivity calculation is enabled
		componentFile << "Electric Conductivity" << seperator << "Thermal Conductivity" << seperator << "Young Modulus"
		<< seperator << "Poisson Ratio" << seperator << "Total Conductive Paths" << seperator << "Mean Conductive Length" << seperator
		<< "Log Electric Conductivity" << seperator << "Log Thermal Conductivity" << seperator << "Log Young Modulus"
		<< seperator << "Log Poisson Ratio" << seperator << "Log Total Conductive Paths" << seperator << "Log Mean Conductive Length" << seperator;

	if (this->iShapes[0].calcElectricConductivityWithFDM) // If FDM electric conductivity is enabled
		componentFile << "FDM Ix" << seperator << "FDM Iy" << seperator << "FDM ro" << seperator;

	componentFile << "Process Time" << seperator << "Preperation Time" << seperator << "Grid(X)" << seperator << "Grid(Y)" << seperator << "ppms" << seperator;

	// Write headers for component-specific properties
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

	componentFile << "\n"; // New line after headers


	for (int i = 0; i < total; i++) // Loop through each simulation case
	{
		// Calculate mean paths and mean path length
		double meanPaths = average_element(iShapes[i].paths, 0, iShapes[i].iterations);
		double meanPathsLength = average_element(iShapes[i].meanPathLength, 0, iShapes[i].iterations);
		// Write case name and mean percolation probability
		componentFile << iShapes[i].projectName << seperator << iShapes[i].meanPercolation << seperator;
		if (this->iShapes[0].calcStatistcs) // If statistics calculation is enabled
			componentFile << casesMeanMaxClusterRadius[i] << seperator << iShapes[i].correleationLength << seperator;
		if (this->iShapes[0].calcElectricConductivity) // If electric conductivity calculation is enabled
			componentFile << iShapes[i].meanElectricConductivity << seperator << iShapes[i].meanThermalConductivity << seperator
			<< iShapes[i].meanYoungModulus << seperator << iShapes[i].meanPoissonRatio << seperator << meanPaths << seperator << meanPathsLength << seperator
			<< log(iShapes[i].meanElectricConductivity) << seperator << log(iShapes[i].meanThermalConductivity) << seperator
			<< log(iShapes[i].meanYoungModulus) << seperator << log(iShapes[i].meanPoissonRatio) << seperator << log(meanPaths) << seperator << log(meanPathsLength) << seperator;
		if (this->iShapes[0].calcElectricConductivityWithFDM) // If FDM electric conductivity is enabled
			componentFile << setw(13) << iShapes[i].FDResults[2 * i] << seperator << setw(13) << iShapes[i].FDResults[2 * i + 1] << seperator << setw(13) << (1 / iShapes[i].FDResults[2 * i]) << seperator;

		// Write process time, preparation time, grid dimensions, and pixels per minimum size
		componentFile << iShapes[i].meanSetUpTime << seperator << iShapes[i].meanTime << seperator << iShapes[i].width << seperator << iShapes[i].height << seperator
			<< iShapes[i].pixelsPerMinimumSize;

		// Write component-specific area percentages
		for (int j = 0; j < iShapes[i].totalComponents; j++)
			componentFile << seperator << iShapes[i].componentsArea[j];
		// Write real component area percentages
		for (int j = 0; j < iShapes[i].totalComponents; j++)
			componentFile << seperator << iShapes[i].realComponentAreas[j];
		// Write component-specific X dimensions
		for (int j = 0; j < iShapes[i].totalComponents; j++)
			componentFile << seperator << iShapes[i].dimensionX[j];

		// Write component-specific Y dimensions
		for (int j = 0; j < iShapes[i].totalComponents; j++)
			componentFile << seperator << iShapes[i].dimensionY[j];

		// Write component-specific hoop sizes
		for (int j = 0; j < iShapes[i].totalComponents; j++)
			componentFile << seperator << iShapes[i].hoops[j];
		componentFile << "\n"; // New line for the next entry

	}

	componentFile.close(); // Close the file

}
// Saves results to a report file with semicolon as the delimiter
void BatchMonteCarlo::saveResultsWithSemicolon(void)
{
	saveResultstoReport(SEMICOLON); // Call the general save function with SEMICOLON type
}

// Saves results to a report file with comma as the delimiter
void BatchMonteCarlo::saveResults(void)
{
	saveResultstoReport(COMMA); // Call the general save function with COMMA type
}