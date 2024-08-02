/*
	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma comment(linker, "/STACK:600000000")
#pragma comment(linker,  "/HEAP:4200000000")

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <time.h>
#include <chrono>
#include "Shapes.h"
#include "Grid.h"
#include "ShapeGenerator.h"
#include "general.h"
#include "BatchMonteCarlo.h"
#include "Settings.h"


using namespace std;


int main() {
	//setlocale(LC_NUMERIC,".OCP"); // ".OCP" if you want to use system settings "French_Canada.1252"

	cout << "------------------------------------------------------------------------------------------------------------------------------------\n";
	cout << info.program;
	cout << info.version;
	cout << info.date;
	cout << info.author;
	cout << info.licence;
	cout << "------------------------------------------------------------------------------------------------------------------------------------\n";
	cout << "Start at " << NowToString() << "\n";



	cout << "Batch mode\n";

	BatchMonteCarlo* batchMonteCarlo = new BatchMonteCarlo();


	if (batchMonteCarlo->getInputFiles() != EXIT_FAILURE)
	{
		batchMonteCarlo->Run();
		batchMonteCarlo->saveResults();
	}
	else
		return EXIT_FAILURE;


	cout << "Press any key to exit...";
	//delay(5);
	//Read key for exit
	char resp;
	cin.get(resp);

	return EXIT_SUCCESS;
}