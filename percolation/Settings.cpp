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
			cout << "Error cannot find " << *fileName << "Program halted..." << '\n';
			char resp;
			cin.get(resp);
			exit(-1);
		}
	
}
