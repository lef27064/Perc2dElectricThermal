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

#include <filesystem>

#include "Shapes.h"
#include "Grid.h"
#include "ShapeGenerator.h"

#include "general.h"



#define maxFilesInDir 300

class BatchMonteCarlo {
public:
	int total=0;


	string directory;
	string inputfiles[maxFilesInDir];
	//string exePath;

	std::filesystem::path  exePath;
	Settings settings;
	ShapeGenerator iShapes[maxFilesInDir];

	//std::vector<double> maxClusterRadiusPerCase;
	std::vector<double> casesMeanMaxClusterRadius;

	//initialize
	BatchMonteCarlo(void);
	BatchMonteCarlo(string idirectory);
	//void singleRun(string fileName, ShapeGenerator *shapes);
	void singleRun(string fileName, ShapeGenerator* shapes);
	int getInputFiles(void);
	void show(void);
	void Run();
	void singlRunsaveResultstoReportHeader(ReportType ireportType, char* fileName);

	void singlRunsaveResultstoReport(ReportType ireportType, char* fileName, int i);
	
	void saveResultstoReport(ReportType ireportType);
	void saveResultsWithSemicolon(void);
	void saveResults();
};