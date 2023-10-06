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
#include <dirent.h>
//#define WINDOWS   
#ifdef WINDOWS
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif


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
};

pinfo const info{ 
		"Rerclolation 2d\n", 
		"Version 1.5 (Cluster Analysis - Electric, Thermal, Mechanical properties calculation (in progress...)",
		"4/7/2023  \n",
		"Lefteris Lamprou\n", 
		"This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License\n" 
};

//Time now to String
string NowToString(void);  
//check if a directory exists
bool dirExists(char* pathname);
//find minimum of array 
double  min_element(double values[], int first, int last);
double  max_element(double values[], int first, int last);
double  average_element(double values[], int first, int last);

string GetCurrentWorkingDir(void); 
//void CreateInputFiles(void);

//string ExePath(void);