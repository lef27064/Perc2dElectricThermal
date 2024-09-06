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

#include "Settings.h"

void Settings::readFromFile(char* fileName)
{
		string line;
		ifstream inputFile(fileName);

		if (inputFile.is_open())
		{
			while (getline(inputFile, line))
			{
				//skip line if starts with #
				while (line[0] == '#')
				{
					getline(inputFile, line);
				}

				//Read save image file
				if ((line =="TRUE") || (line=="true"))
					saveImageFile=true;
				else
					saveImageFile = false;


				getline(inputFile, line);

				//skip line if starts with #
				while (line[0] == '#')
				{
					getline(inputFile, line);
				}
				//read random save image file
				if ((line == "TRUE") || (line == "true"))
					RandomSaveImageFile = true;
				else
					RandomSaveImageFile = false;
				
				//Read total images to save
				getline(inputFile, line);
				//skip line if starts with #
				while (line[0] == '#')
				{
					getline(inputFile, line);
				}


				totalImagesToSave = std::stoi(line);

				//--------------------------------------
				

				getline(inputFile, line);
				//read save shapes
				//skip line if starts with #
				while (line[0] == '#')
				{
					getline(inputFile, line);
				}
			
				if ((line == "TRUE") || (line == "true"))
					saveShapes = true;
				else
					saveShapes = false;

				getline(inputFile, line);
				
				//read if save bmp
				//skip line if starts with #
				while (line[0] == '#')
				{
					getline(inputFile, line);
				}

				if ((line == "BMP") || (line == "bmp"))
					saveAsBmpImage = true;
				else
					saveAsBmpImage = false;

				getline(inputFile, line);
				//read is Lattice shapes
				//skip line if starts with #
				
				while (line[0] == '#')
				{
					getline(inputFile, line);
				}

				if ((line == "TRUE") || (line == "true"))
					isLattice = true;
				else
					isLattice = false;
			}

			inputFile.close();

		

						
		}
		else

		{
			cout << "\n*************************************************************************************\n";
			cout << "Error cannot find ";
			std::cout.write(fileName, strlen(fileName));
			cout << " Program halted..." << '\n';
			cout << "*************************************************************************************\n";
			//Some delay is better than wait for user response
			delay(4);
			
			/*
			char resp;
			cin.get(resp);
			*/
			exit(-1);
		}
	
}
