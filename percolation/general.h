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

#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <chrono>
#include <time.h>
#include <filesystem>

//#define GetCurrentDir std::filesystem::current_path();


using namespace std;

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
	NONE = 3		  //None (disolver)
};

enum  Direction
{
	HORIZONTAL = 0,
	VERTICAL=1
};

enum ShapeType
{
	RECTANGLE = 0,			  // normal rectangle	
	SLOPEDRECTANGLE = 1,      // sloped rectangle
	CIRCLE = 2,				  // circle
	ELLIPSE = 3,			  // Ellipse		
	SLOPED = 5,
	CONSTANTSLOPEDRECTANGLE = 6,// Const Sloped Rectangle
	CONSTANTSLOPEDELLIPSE = 7,  // Const Sloped Ellipse
	NOTHING = 4				  // none (disolver)	
};

struct pinfo {
	string program;
	string version;
	string date;
	string author;
	string licence;
	int mayorVersion;
	int minorVersion;
};

pinfo const info{ 
		"Rerclolation 2d\n", 
		"Version 2.0 Percolation, Cluster Analysis,  Electric - Thermal - Mechanical properties estimation \n",
		"Last update: 1/7/2024\n",
		"Author:Eleftherios Lamprou\n", 
		"Licence:This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License.\n Developed during my PhD Thesis (2017-2024)\n",
		2,
		0
};

string NowToString(void);  

double  min_element(double values[], int first, int last);
double  max_element(double values[], int first, int last);
double  average_element(double values[], int first, int last);
