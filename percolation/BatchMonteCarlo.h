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
	void saveResultsWithSemicolon(void);
	void saveResults();
};