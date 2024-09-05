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
#include <fstream>
#include <stdio.h>
#include "Grid.h"
#include <errno.h>
#include <string.h>

const int bytesPerPixel = 3; /// red, green, blue
const int fileHeaderSize = 14;
const int infoHeaderSize = 40;

void  generatePGMImage(char *image, int height, int width, char* imageFileName);
void generateBitmapImage(char *image, int height, int width, char* imageFileName);
void generateBitmapImageFortranStyle(int* image, int height, int width, char* imageFileName);
void saveClustersAsBitmapImage(unsigned char *image, int height, int width, char* imageFileName);

void saveClustersAsBitmapImage(unsigned char * image, int height, int width, int totalColors, char * imageFileName);


unsigned char* createBitmapFileHeader(int height, int width);
unsigned char* createBitmapInfoHeader(int height, int width);

