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
*/

#pragma once

#include <stdio.h> 
#include <chrono>
#include <iostream>     
#include <fstream>   
#include <string>
#include "general.h"
#include <cstring>
using namespace std;
class Settings {
	
public:
	
	bool saveImageFile;
	bool RandomSaveImageFile;
	int totalImagesToSave;
	bool saveShapes;
	bool saveAsBmpImage;
	bool isLattice=true;
	Settings(void) : saveImageFile(true), RandomSaveImageFile(true), totalImagesToSave(1), saveShapes(true), saveAsBmpImage(true) {};
	Settings(bool isaveImageFile, bool iRandomSaveImageFile, int itotalImagesToSave, bool isaveShapes) : saveImageFile(isaveImageFile), RandomSaveImageFile(iRandomSaveImageFile), totalImagesToSave(itotalImagesToSave), saveShapes(isaveShapes),saveAsBmpImage(true) {};

	void readFromFile(char* fileName);
};