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
1. E. Lambrou and L. N. Gergidis, “A computational method for calculating the electrical and thermal properties of random composite” ,
Physica A: Statistical Mechanics and its Applications, Volume 642, 2024, 129760, ISSN 0378-4371,
https://doi.org/10.1016/j.physa.2024.129760
2. E. Lambrou and L. N. Gergidis, “A particle digitization-based computational method for continuum percolation,” Physica A: Statistical Mechanics
and its Applications, vol. 590, p. 126738, 2022

if you use this programm and write a paper or report please cite above papers

*/

#pragma once
#include <fstream>   // Required for file stream operations (e.g., std::ofstream).
#include <stdio.h>   // Standard input/output library, used for C-style file operations (FILE*, fopen, fprintf, fclose).
#include "Grid.h"    // Custom header file likely defining the 'Grid' class or related structures/enums for cell states.
#include <errno.h>   // Provides access to the 'errno' variable and error codes for reporting system errors.
#include <string.h>  // Provides string manipulation functions, potentially used for error messages (e.g., strerror).


// Structure to represent an sRGB color with blue, green, and red components.
// The order (Blue, Green, Red) is typical for BMP file format pixel data.
struct sRGB {
	unsigned char blue;
	unsigned char green;
	unsigned char red;
};

// Global constant: Number of bytes per pixel for RGB images (3 bytes for Red, Green, Blue).
const int bytesPerPixel = 3; /// red, green, blue
// Global constant: Size of the BMP file header in bytes.
const int fileHeaderSize = 14;
// Global constant: Size of the BMP info header in bytes.
const int infoHeaderSize = 40;

// Function declarations for image generation.

// Generates a PGM (Portable Graymap) image from a character array representing pixel data.
// @param image A pointer to the character array containing image pixel data.
// @param height The height of the image in pixels.
// @param width The width of the image in pixels.
// @param imageFileName A C-style string representing the name of the output PGM file.
void  generatePGMImage(char* image, int height, int width, char* imageFileName);

// Generates a BMP (Bitmap) image from a character array representing pixel data.
// This function likely maps internal cell states to specific RGB colors.
// @param image A pointer to the character array containing image pixel data.
// @param height The height of the image in pixels.
// @param width The width of the image in pixels.
// @param imageFileName A C-style string representing the name of the output BMP file.
void generateBitmapImage(char* image, int height, int width, char* imageFileName);

// Generates a BMP (Bitmap) image from an integer array, possibly for "Fortran-style" binary data.
// This might imply a simpler black/white or 2-color mapping based on integer values.
// @param image A pointer to the integer array containing image pixel data.
// @param height The height of the image in pixels.
// @param width The width of the image in pixels.
// @param imageFileName A C-style string representing the name of the output BMP file.
void generateBitmapImageFortranStyle(int* image, int height, int width, char* imageFileName);

// Saves clusters as a grayscale BMP image from an unsigned character array.
// This function likely assigns different gray shades to different cluster IDs.
// @param image A pointer to the unsigned character array containing cluster IDs or grayscale values.
// @param height The height of the image in pixels.
// @param width The width of the image in pixels.
// @param imageFileName A C-style string representing the name of the output BMP file.
void saveClustersAsBitmapImage(unsigned char* image, int height, int width, char* imageFileName);

// Saves clusters as a color BMP image, assigning distinct colors to different cluster IDs.
// It takes `totalColors` to distribute colors across the spectrum for better visual distinction.
// @param image A pointer to the unsigned character array containing cluster IDs.
// @param height The height of the image in pixels.
// @param width The width of the image in pixels.
// @param totalColors The total number of distinct colors (clusters) to represent in the image.
// @param imageFileName A C-style string representing the name of the output BMP file.
void saveClustersAsBitmapImage(unsigned char* image, int height, int width, int totalColors, char* imageFileName);

// Utility functions for creating BMP header components.

// Creates and returns a pointer to a byte array representing the BMP file header.
// This header contains general information about the BMP file, such as file size and pixel data offset.
// @param height The height of the image.
// @param width The width of the image.
// @return A pointer to an unsigned char array containing the BMP file header bytes.
unsigned char* createBitmapFileHeader(int height, int width);

// Creates and returns a pointer to a byte array representing the BMP info header.
// This header contains detailed information about the image, such as dimensions, color depth, and compression.
// @param height The height of the image.
// @param width The width of the image.
// @return A pointer to an unsigned char array containing the BMP info header bytes.
unsigned char* createBitmapInfoHeader(int height, int width);