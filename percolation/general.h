/*
This file is part of Electric,Thermal, Mechanical Properties
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
1. E. Lambrou and L. N. Gergidis, “A computational method for calculating the electrical and thermal properties of random composite” ,
Physica A: Statistical Mechanics and its Applications, Volume 642, 2024, 129760, ISSN 0378-4371,
https://doi.org/10.1016/j.physa.2024.129760
2. E. Lambrou and L. N. Gergidis, “A particle digitization-based computational method for continuum percolation,” Physica A: Statistical Mechanics
and its Applications, vol. 590, p. 126738, 2022

if you use this programm and write a paper or report please cite above papers

*/

#pragma once // Ensures this header file is included only once in a compilation unit

#include <fstream>      // For file stream operations (e.g., reading from files)
#include <iostream>     // For input/output operations (e.g., console printing)
#include <string>       // For string manipulation
#include <chrono>       // For time-related functionalities (e.g., measuring execution time)
#include <time.h>       // For C-style time functions
#include <filesystem>   // For filesystem operations (e.g., getting current directory)
#include <vector>       // Added for std::vector if not implicitly included elsewhere.

using namespace std; // Using the standard namespace to avoid prefixing std::

// Macro to get the current working directory using C++17 filesystem library
#define GetCurrentDir std::filesystem::current_path();

// Enumeration defining possible states for a cell in the grid
enum CellState {
	PERCOLATE = '1', // Represents a cell that is part of a percolating cluster (conductor)
	HARD = '3',      // Represents a 'hard core' cell (e.g., impenetrable particle center)
	EMPTY = '0',     // Represents an empty cell (no conductor)
	SOFT = '2',      // Represents a 'soft shell' or 'hoop' cell (e.g., part of a particle's outer layer)
	BORDER = '4',    // Represents a cell on the boundary of the simulation domain
	PATH = '5',      // Represents a cell identified as part of a connected path
	SIDEPATH = '6'   // Represents a cell identified as part of a side path (e.g., for specific analysis)
};

// Enumeration defining types of particle sizes or distribution
enum SizeType {
	CONSTANT = 0,    // Particles have a constant size
	VARIABLE = 1,    // Particles have variable sizes
	NONE = 3         // No specific particle size consideration (e.g., for matrix-only scenarios)
};

// Enumeration defining directions, likely for applied fields or material orientation
enum Direction
{
	HORIZONTAL = 0, // Horizontal direction
	VERTICAL = 1    // Vertical direction
};

// Enumeration defining report output formatting types
enum ReportType
{
	COMMA = 0,     // Values in reports are separated by commas
	SEMICOLON = 1  // Values in reports are separated by semicolons
};

// Enumeration defining types of shapes for particles or inclusions
enum ShapeType
{
	RECTANGLE = 0,         // Normal rectangular shape
	SLOPEDRECTANGLE = 1,   // Sloped rectangular shape
	CIRCLE = 2,            // Circular shape
	ELLIPSE = 3,           // Elliptical shape
	//	SLOPED = 5,              // To be implemented: generally sloped shape
	//	CONSTANTSLOPEDRECTANGLE = 6, // To be implemented: constant sloped rectangle
	//	CONSTANTSLOPEDELLIPSE = 7,   // To be implemented: constant sloped ellipse
	NOTHING = 4            // No specific shape (e.g., for matrix or generic elements)
};

// Structure to hold program information
struct pinfo {
	string program;      // Name of the program
	string version;      // Full version string (e.g., "Version 2.0 ...")
	string date;         // Last update date
	string author;       // Author's name
	string licence;      // Licensing information
	int mayorVersion;    // Major version number
	int minorVersion;    // Minor version number
	int dimensions;      // Number of dimensions the program operates in (e.g., 2 for 2D)
};

// Constant instance of pinfo structure, containing details about the program
pinfo const info{
		"Electric,Thermal, Mechanical Properties Estimation With Percolation Theory (ETMPEWPT) (2D version) \n", // Program name
		"Version 2.0 Percolation, Cluster Analysis, Electric - Thermal - Mechanical properties\n", // Version details
		"Last update: 1/7/2024\n", // Last update date
		"Author:Eleftherios Lamprou\n", // Author
		"Licence:This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License.\nDeveloped during PhD Thesis (2017-2024)\n", // License information
		2, // Major version number
		0, // Minor version number
		2  // Number of dimensions
};

// Function prototypes:

// Converts the current time to a string format.
string NowToString(void);

// Finds the minimum element in a given array within a specified range.
double minElement(double values[], int first, int last);

// Finds the maximum element in a given array within a specified range.
double maxElement(double values[], int first, int last);

// Calculates the average of elements in a given array within a specified range.
double average_element(double values[], int first, int last);

// Pauses the execution for a specified number of seconds.
void delay(int number_of_seconds);