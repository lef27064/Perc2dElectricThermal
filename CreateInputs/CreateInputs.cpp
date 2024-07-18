// CreateInputs.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <fstream>
#include <iostream>
#include <string>
#include <iomanip>
#include <map>
#include <random>
#include <cmath>

using namespace std;
int main()
{

	cout << "-------------------------------------------------------------------------------------------------------------------------\n";
	cout << "Percolation Setup Input Program \n";
	cout << "version 1.0\n";
	cout << "Jan 2024\n";
	cout << "Lefteris Lamprou\n";
	cout << "This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License\n";
	cout << "-------------------------------------------------------------------------------------------------------------------------\n";

	string inputFile = "input.txt";
	string directory = "..\\percolation\\inputs";

	double ar[] = { 1 };
	int totalfj = 20;
	int totalfi = 20;
	double fistart = 0.10;
	double fi = 0;

	double fiend = 0.99;
	double fjstart = 0;
	double fjend = 1;
	double fj = 0;
	

	int Widths[] = { 2000 };
	int Ppms[] = { 201 };
	int Iters[] = { 300 };

		
	
	for (int i = 1; i <=25; i=i+1)
	//for (int j = 0; j <= 0; j=j+1)
		//	for (int j = 0; j <14; j++)
	{
		ofstream inputFile;
		int iters = Iters[0];
		int ppminCircle = 101;
		//= 2 * sqrt(l);
		int width = 500; //Widths[j];
		int height = width;
		int depth = width;
		//double w = fi + i * deltafi;
		fi = 0.59+(float)i /100.0;
		//fj = j / 100.0;
		double w = fi;
		string FileName = directory + "\\in_" + to_string(i) + "_" + to_string(ppminCircle) + "_" + to_string(width) + "_" + to_string(w) + ".txt";
		// Debug cout << FileName << "\n";
		inputFile.open(FileName);
		inputFile << "#name\n";
		inputFile << "ellipse_" << i << "iter" << iters << "ppmC_" << ppminCircle << "width_" << width << "_Area" << w << "\n";
		inputFile << "#total component\n";
		inputFile << "2\n";
		inputFile << "#components % weights\n";
		inputFile << 1.0 - fi  << " " << fi <<"\n";
		inputFile << "#Components type Geometry : RECTANGLE = 0, SLOPEDRECTANGLE = 1, ELLIPSE = 3, моме = 4 (disolver)\n";
		inputFile << "4 2\n";
		inputFile << "#SizeType CONSTANT = 0 (Constant Size), VARIABLE = 1, (Variable Size), NONE = 3 None(disolver)\n";
		inputFile << "3 0\n";
		inputFile << "#Special Weights\n";
		inputFile << "1.0 1.0\n";
		inputFile << "#Dimension X\n";
		inputFile << "10.0e-09 10.0e-09\n";
		inputFile << "#Dimension Y\n";
		inputFile << "10.0e-09 10.0e-09\n";
		//	inputFile << "#Dimension Z\n";
		//	inputFile << "10.0e-09 10.0e-09\n";
		inputFile << "#hoops\n";
		inputFile << "0.0 0.0\n";
		inputFile << "#Electric Conductivities\n";
		inputFile << "1.e-16 1.0\n";
		inputFile << "#Thermal Conductivities\n";
		inputFile << "1.e-16 1.0\n";
		inputFile << "#Young Modulus\n";
		inputFile << "1.e+5 1.e+10\n";
		inputFile << "#Poisson Ratio\n";
		inputFile << "0.3 0.32\n";
		inputFile << "#width - height - depth\n";
		inputFile << width << " " << height << "\n"; // << depth << "\n";
		inputFile << "#iterations\n";
		inputFile << iters << "\n";
		inputFile << "#pixels per min Sphere\n";
		inputFile << ppminCircle << "\n";
		inputFile << "#Swiss chesse case TRUE or FALSE\n";
		inputFile << "FALSE\n";
		inputFile << "# CALC ELECTRIC CONDUCTIVITY\n";
		inputFile << "TRUE\n";
		inputFile << "# CALC ELECTRIC CONDUCTIVITY WITH FINITE DIFFERENCES\n";
		inputFile << "false\n";
		inputFile << "# CALCULATE STATISTICS\n";
		inputFile << "FALSE\n";
		//	inputFile << "#Calc electric conductivity\n";
		//	inputFile << "FALSE\n";
		inputFile.close();

	}
};



// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
