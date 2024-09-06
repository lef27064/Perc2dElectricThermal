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

#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <chrono>
#include <time.h>
#include <filesystem>

using namespace std;

#define GetCurrentDir std::filesystem::current_path();

enum CellState {
	PERCOLATE = '1',  //Percolate symbol
	HARD = '3',       //CellState::HARD core
	EMPTY = '0',      //no conductor
	SOFT = '2',       //CellState::SOFT circle (hoop)
	BORDER = '4',
	PATH = '5',
	SIDEPATH='6'
};

enum SizeType {
	CONSTANT = 0,	  //Constant Size 	
	VARIABLE = 1,	  //Variable Size 	
	NONE = 3		  //None (matrix)
};

enum  Direction
{
	HORIZONTAL = 0,
	VERTICAL=1
};

enum  ReportType
{
	COMMA     = 0,
	SEMICOLON = 1
};

enum ShapeType
{
	RECTANGLE = 0,			  // normal rectangle	
	SLOPEDRECTANGLE = 1,      // sloped rectangle
	CIRCLE = 2,				  // circle
	ELLIPSE = 3,			  // Ellipse		
//	SLOPED = 5,               // to be implemented
//	CONSTANTSLOPEDRECTANGLE = 6,// Const Sloped Rectangle
//	CONSTANTSLOPEDELLIPSE = 7,  // Const Sloped Ellipse
	NOTHING = 4				  // none (matrix)	
};

struct pinfo {
	string program;
	string version;
	string date;
	string author;
	string licence;
	int mayorVersion;
	int minorVersion;
	int dimensions;
};

pinfo const info{ 
		"Electric,Thermal, Mechanical Properties Estimation With Percolation Theory (ETMPEWPT) (2D version) \n", 
		"Version 2.0 Percolation, Cluster Analysis,  Electric - Thermal - Mechanical properties\n",
		"Last update: 1/7/2024\n",
		"Author:Eleftherios Lamprou\n", 
		"Licence:This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License.\nDeveloped during PhD Thesis (2017-2024)\n",
		2,
		0,
		2
};

string NowToString(void);  

double  minElement(double values[], int first, int last);
double  maxElement(double values[], int first, int last);
double  average_element(double values[], int first, int last);
void    delay(int number_of_seconds);
