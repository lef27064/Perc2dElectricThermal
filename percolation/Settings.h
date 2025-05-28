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

#pragma once // Ensures that this header file is included only once during compilation

#include <stdio.h>     // Standard input/output library (e.g., for file operations)
#include <chrono>      // For time-related functionalities
#include <iostream>    // For standard input/output streams (e.g., `cout`, `cin`)
#include <fstream>     // For file stream operations (e.g., reading/writing files)
#include <string>      // For using the `std::string` class
#include "general.h"   // Includes a custom header file likely containing common definitions and utility functions
#include <cstring>     // For C-style string manipulation functions (e.g., `memset`)

using namespace std; // Brings all names from the `std` namespace into the current scope

// Definition of the Settings class
class Settings {

public:
    // Member variables to control various simulation settings
    bool saveImageFile;       // Flag to determine if image files should be saved
    bool RandomSaveImageFile; // Flag to determine if image saving should be random (e.g., save only some images)
    int totalImagesToSave;    // Total number of images to save if `RandomSaveImageFile` is true
    bool saveShapes;          // Flag to determine if generated shapes should be saved
    bool saveAsBmpImage;      // Flag to determine if images should be saved in BMP format
    bool isLattice;           // Flag to determine if the simulation uses a lattice structure (default to true)

    // Default constructor: Initializes all settings with default values
    Settings(void) : saveImageFile(true), RandomSaveImageFile(true), totalImagesToSave(1), saveShapes(true), saveAsBmpImage(true), isLattice(true) {};

    // Parameterized constructor: Allows initializing settings with custom values
    // `isaveImageFile`: initial value for `saveImageFile`
    // `iRandomSaveImageFile`: initial value for `RandomSaveImageFile`
    // `itotalImagesToSave`: initial value for `totalImagesToSave`
    // `isaveShapes`: initial value for `saveShapes`
    Settings(bool isaveImageFile, bool iRandomSaveImageFile, int itotalImagesToSave, bool isaveShapes)
        : saveImageFile(isaveImageFile), RandomSaveImageFile(iRandomSaveImageFile),
        totalImagesToSave(itotalImagesToSave), saveShapes(isaveShapes), saveAsBmpImage(true), isLattice(true) {
    };

    // Member function to read settings from a specified file
    // `fileName`: C-style string representing the path to the settings file
    void readFromFile(char* fileName);
};