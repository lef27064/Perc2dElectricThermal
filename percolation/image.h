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

