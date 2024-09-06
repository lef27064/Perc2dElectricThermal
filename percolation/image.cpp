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

#include "image.h"
		


sRGB RGB_Black = { 0x0,0x0,0x0 };
sRGB RGB_White = { 0xFF,0xFF,0xFF };
sRGB RGB_Light_Gray = {224,224,224 };
sRGB RGB_Gray = { 128,128,128 };
sRGB RGB_Dark_Gray = { 64,64,64 };
sRGB RGB_Red = {0xFF,0,0};
sRGB RGB_Pink = { 254,96,208 };
sRGB RGB_Purple = { 160,32,254 };
sRGB RGB_Light_Blue = { 80,208,254 };
sRGB RGB_Blue = { 0,0x20,0xFF };
sRGB RGB_Yellow_Green = { 96,254,128 };
sRGB RGB_Green = { 0,192,0 };
sRGB RGB_Yellow = { 0xFF,224,32 };
sRGB RGB_Gold = { 0x33,0xA5,0xAA }; 
sRGB RGB_Brown = { 160,128,96 };
sRGB RGB_Orange = { 0xCD, 0x91, 0x52 };

void generatePGMImage(char * image, int height, int width, char * imageFileName)
{


	FILE* pgmimg;
	
	pgmimg = fopen(imageFileName, "w");


	// Writing Magic Number to the File
	fprintf(pgmimg, "P2\n");

	// Writing Width and Height
	fprintf(pgmimg, "%d %d\n", width , height);

	// Writing the maximum gray value
	fprintf(pgmimg, "255\n");
	//int count = 0;


	for (int line = 0; line < height; line++)
	{
		for (int column = 0; column < width ; column++)
		{
			unsigned char color = image[(size_t)line * width + column];
			switch (color)
			{
			case '0':fprintf(pgmimg, "%d ", 0); break;
			case '1': fprintf(pgmimg, "%d ", 64); break;
			case '2': fprintf(pgmimg, "%d ", 128); break;
			case '3': fprintf(pgmimg, "%d ", 164); break;
			case '4':fprintf(pgmimg, "%d ", 200); break;
			case '5':fprintf(pgmimg, "%d ", 222); break;
			case '6':fprintf(pgmimg, "%d ", 254); break;
			}
		}
		fprintf(pgmimg, "\n");
	}
	int err  = fclose(pgmimg);
	if (err != 0)
		cout <<"*** Warning: Error writing at PGM Image  ****\n";
}

//only for internal proposes code clearnce
void setcolor(unsigned char* image, int start,  unsigned char red , unsigned char green , unsigned char blue )
{
	image[start] = red;
	image[start+1] = green;
	image[start+2] = blue;
}

void setcolor(unsigned char* image, int start, sRGB iRGB)
{
	image[start] = iRGB.red;
	image[start + 1] = iRGB.green;
	image[start + 2] = iRGB.blue;
}


//create bitmap from array
void generateBitmapImage(char *image, int height, int width, char* imageFileName) {

	//bmp format
	unsigned char* fileHeader = createBitmapFileHeader(height, width);
	unsigned char* infoHeader = createBitmapInfoHeader(height, width);
	unsigned char padding[] = { 0, 0, 0 };

	//one line of image 3 colors* width
	unsigned char* colors = new unsigned char[bytesPerPixel*width]();

	int paddingSize = (4 - (width*bytesPerPixel) % 4) % 4;


	FILE* imageFile;
	//int err;
	imageFile = fopen(imageFileName, "w");

	//initialize
	fwrite(fileHeader, 1, fileHeaderSize, imageFile);
	fwrite(infoHeader, 1, infoHeaderSize, imageFile);


	//from botom to up
	for (int line = height-1; line >=0 ; line--)
	{
		for (int column = 0; column < width ; column++)
		{
			unsigned char color = image[(size_t)line*width + column];
			switch (color){
			//EMPTY 	
			case '0':  setcolor(colors, 3 * column, RGB_Black);  break;
			//PERCOLATE			
			case '1': setcolor(colors, 3 * column,RGB_White );  break;
			//SOFT
			case '2':setcolor(colors, 3 * column, RGB_Red); break;
			//HARD
			case '3': setcolor(colors, 3 * column, RGB_Orange);  break;
			//BORDER
			case '4': setcolor(colors, 3 * column, RGB_Brown);  break;
			//PATH
			case '5': setcolor(colors, 3 * column, RGB_Yellow);  break;
			//SIDEPATH
			case '6': setcolor(colors, 3 * column, RGB_Light_Blue);  break;
				/*
			case '0':  setcolor(colors, 3 * column, 0x00, 0x00, 0x00);  break;
				//case '1': setcolor(colors, 3 * column, 255, 160, 16);  break;
			case '1': setcolor(colors, 3 * column, 224, 224, 224);  break;
			case '2':setcolor(colors, 3 * column, 0x70, 0x00, 0x40); break;
			case '3': setcolor(colors, 3 * column, 224, 224, 224);  break;
			case '4': setcolor(colors, 3 * column, 0x40, 0x50, 0x80);  break;
			case '5': setcolor(colors, 3 * column, 255, 224, 32);  break;
			case '6': setcolor(colors, 3 * column, 0, 192, 0);  break;*/
			}
			//if (((line % 10) ==0) || ((column % 10) == 0))
			//	setcolor(colors, 3 * column, 0x00, 0x80, 0x00);
		}
		//write line and pading
		fwrite(colors, bytesPerPixel, width, imageFile);
		fwrite(padding, 1, paddingSize, imageFile);
	}

	//close
	fclose(imageFile);
	delete[](colors);
}



//create bitmap from array
void generateBitmapImageFortranStyle(int* image, int height, int width, char* imageFileName) {

	//bmp format
	unsigned char* fileHeader = createBitmapFileHeader(height, width);
	unsigned char* infoHeader = createBitmapInfoHeader(height, width);
	unsigned char padding[] = { 0, 0, 0 };

	//one line of image 3 colors* width
	unsigned char* colors = new unsigned char[bytesPerPixel * width]();

	int paddingSize = (4 - (width * bytesPerPixel) % 4) % 4;


	FILE* imageFile;
	//int err;
	imageFile = fopen(imageFileName, "w");

	//initialize
	fwrite(fileHeader, 1, fileHeaderSize, imageFile);
	fwrite(infoHeader, 1, infoHeaderSize, imageFile);

	//from botom to up
	for (int line = height - 1; line >= 0; line--)
	{
		for (int column = 0; column < width; column++)
		{
			int color = image[(size_t)line * width + column];
			if (color==1)
				setcolor(colors, 3 * column, 0x00, 0x00, 0x00);
			else
				setcolor(colors, 3 * column, 0xFF, 0xFF, 0xFF);

				//case CellState::SOFT:setcolor(colors, 3 * column, 0x33, 0x99, 0xFF); break;
				//case BORDER: setcolor(colors, 3 * column, 0xFF, 0xFF, 0xFF);  break;


			//if (((line % 10) ==0) || ((column % 10) == 0))
			//	setcolor(colors, 3 * column, 0x00, 0x80, 0x00);
		}
		//write line and pading
		fwrite(colors, bytesPerPixel, width, imageFile);
		fwrite(padding, 1, paddingSize, imageFile);
	}

	//close
	fclose(imageFile);
	delete[](colors);

}



//create bitmap from array
void saveClustersAsBitmapImage(unsigned char *image, int height, int width, char* imageFileName) {

	//bmp format
	unsigned char* fileHeader = createBitmapFileHeader(height, width);
	unsigned char* infoHeader = createBitmapInfoHeader(height, width);
	unsigned char padding[] = { 0, 0, 0 };

	//one line of image 3 colors* width
	unsigned char* colors = new unsigned char[bytesPerPixel*width]();

	int paddingSize = (4 - (width*bytesPerPixel) % 4) % 4;


	FILE* imageFile;
//	int err;
	imageFile = fopen(imageFileName, "w");

	//initialize
	fwrite(fileHeader, 1, fileHeaderSize, imageFile);
	fwrite(infoHeader, 1, infoHeaderSize, imageFile);
	//unsigned char red = 557756 & 0xFF;
	//unsigned char blue = (557756 >> 8) & 0xFF;
	//unsigned char green = (557756 >> 16) & 0xFF;
	// 
	//from botom to up
	for (int line = height - 1; line >= 0; line--)
	{
		for (int column = 0; column < width; column++)
		{

			unsigned char color = image[(size_t) line*width + column];
			switch ((int)color)
			{
			//case 0:setcolor(colors, 3 * column, 0x00, 0x00, 0x00); cout << "1"; break;
			case 0:  setcolor(colors, 3 * column, 0x00, 0x00, 0x00);  break;
			case 1: setcolor(colors, 3 * column, 0x20, 0x20, 0x20);  break;
			case 2:setcolor(colors, 3 * column, 0x40, 0x40, 0x40); break;
			case 3: setcolor(colors, 3 * column, 0x60, 0x60, 0x60);  break;
			case 4: setcolor(colors, 3 * column, 0x80, 0x80, 0x80);  break;
			default:setcolor(colors, 3 * column, 0x00, 0x00, 0x00); break;
			}

			//setcolor(colors, 3 * column, color, color, color);

		}
		//write line and pading
		fwrite(colors, bytesPerPixel, width, imageFile);
		fwrite(padding, 1, paddingSize, imageFile);
	}

	//close
	fclose(imageFile);
	delete [] colors;
}

//create bitmap from array
void saveClustersAsBitmapImage(unsigned char *image, int height, int width, int totalColors,char* imageFileName) {

	//bmp format
	unsigned char* fileHeader = createBitmapFileHeader(height, width);
	unsigned char* infoHeader = createBitmapInfoHeader(height, width);
	unsigned char padding[] = { 0, 0, 0 };

	int trueColor;
	int colorWidth = (0xFFFFFF -1)/ (totalColors+1);

//	cout << "Color Width" << colorWidth << "\n";
	//one line of image 3 colors* width

	unsigned char* colors = new unsigned char[bytesPerPixel*width]();

	int paddingSize = (4 - (width*bytesPerPixel) % 4) % 4;


	FILE* imageFile;
	int err;
	imageFile = fopen(imageFileName, "w");

	//initialize
	fwrite(fileHeader, 1, fileHeaderSize, imageFile);
	fwrite(infoHeader, 1, infoHeaderSize, imageFile);
	unsigned char red;
	unsigned char blue;
	unsigned char green;
	//from botom to up
	for (int line = height - 1; line >= 0; line--)
	{
		for (int column = 0; column < width; column++)
		{

			unsigned char color = image[(size_t)line*width + column];
			trueColor = (int)color * colorWidth;
			red = (( char)(trueColor % (0xFF)) )|0xF;//& (0xFF);
			blue = ((char(trueColor) % 0x00FF) ) | 0xF; //(trueColor >> 8) & (0xFF);
			green = ((char(trueColor) % 0x0000FF)) | 0xF; //(trueColor >> 16) & (0xFF);
			if (color>0)
				//setcolor(colors, 3 * column, 0xFA, 0xFA, 0XFA);
					setcolor(colors, 3 * column, red, green, blue);
			else
					setcolor(colors, 3 * column, 0, 0, 0);

		}
		//write line and pading
		fwrite(colors, bytesPerPixel, width, imageFile);
		fwrite(padding, 1, paddingSize, imageFile);
	}

	//close
	err=fclose(imageFile);
	delete[] colors;
	if (err != 0)
		cout << "*** Warning: Error writing at Image File  ****\n";
}



unsigned char* createBitmapFileHeader(int height, int width) {
	int fileSize = fileHeaderSize + infoHeaderSize + bytesPerPixel * height*width;

	static unsigned char fileHeader[] = {
		0,0,     /// signature
		0,0,0,0, /// image file size in bytes
		0,0,0,0, /// reserved
		0,0,0,0, /// start of pixel array
	};

	fileHeader[0] = (unsigned char)('B');
	fileHeader[1] = (unsigned char)('M');
	fileHeader[2] = (unsigned char)(fileSize);
	fileHeader[3] = (unsigned char)(fileSize >> 8);
	fileHeader[4] = (unsigned char)(fileSize >> 16);
	fileHeader[5] = (unsigned char)(fileSize >> 24);
	fileHeader[10] = (unsigned char)(fileHeaderSize + infoHeaderSize);

	return fileHeader;
}

unsigned char* createBitmapInfoHeader(int height, int width) {
	static unsigned char infoHeader[] = {
		0,0,0,0, /// header size
		0,0,0,0, /// image width
		0,0,0,0, /// image height
		0,0,     /// number of color planes
		0,0,     /// bits per pixel
		0,0,0,0, /// compression
		0,0,0,0, /// image size
		0,0,0,0, /// horizontal resolution
		0,0,0,0, /// vertical resolution
		0,0,0,0, /// colors in color table
		0,0,0,0, /// important color count
	};

	infoHeader[0] = (unsigned char)(infoHeaderSize);
	infoHeader[4] = (unsigned char)(width);
	infoHeader[5] = (unsigned char)(width >> 8);
	infoHeader[6] = (unsigned char)(width >> 16);
	infoHeader[7] = (unsigned char)(width >> 24);
	infoHeader[8] = (unsigned char)(height);
	infoHeader[9] = (unsigned char)(height >> 8);
	infoHeader[10] = (unsigned char)(height >> 16);
	infoHeader[11] = (unsigned char)(height >> 24);
	infoHeader[12] = (unsigned char)(1);
	infoHeader[14] = (unsigned char)(bytesPerPixel * 8);

	return infoHeader;
}
