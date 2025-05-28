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

#include "image.h"

// --- Global Color Definitions Initialization ---
// These are the global sRGB color variables declared in image.h.
// They are initialized here with their specific byte values for Red, Green, and Blue components.
// Note the order: {Blue, Green, Red} to match BMP's typical byte order for pixel data.
sRGB RGB_Black = { 0x0,0x0,0x0 };
sRGB RGB_White = { 0xFF,0xFF,0xFF };
sRGB RGB_Light_Gray = { 224,224,224 };
sRGB RGB_Gray = { 128,128,128 };
sRGB RGB_Dark_Gray = { 64,64,64 };
sRGB RGB_Red = { 0,0,0xFF }; // Red is 0xFF (255) for Red, 0 for Green, 0 for Blue (written as {B,G,R}).
sRGB RGB_Pink = { 208,96,254 }; // Pink (254,96,208) as {208,96,254}
sRGB RGB_Purple = { 254,32,160 }; // Purple (160,32,254) as {254,32,160}
sRGB RGB_Light_Blue = { 254,208,80 }; // Light Blue (80,208,254) as {254,208,80}
sRGB RGB_Blue = { 0xFF,0x20,0 }; // Blue (0,32,255) as {255,32,0}
sRGB RGB_Yellow_Green = { 128,254,96 }; // Yellow Green (96,254,128) as {128,254,96}
sRGB RGB_Green = { 0,192,0 }; // Green (0,192,0) as {0,192,0}
sRGB RGB_Yellow = { 32,224,0xFF }; // Yellow (255,224,32) as {32,224,255}
sRGB RGB_Gold = { 0xAA,0xA5,0x33 }; // Gold (51,165,170) as {170,165,51}
sRGB RGB_Brown = { 96,128,160 }; // Brown (160,128,96) as {96,128,160}
sRGB RGB_Orange = { 0x52,0x91,0xCD }; // Orange (205,145,82) as {82,145,205}


void generatePGMImage(char* image, int height, int width, char* imageFileName)
{
	// Use FILE* and C-style functions as per original requirement,
	// but prefer std::ofstream for C++ streams in new code.
	FILE* pgmimg = nullptr; // Initialize to nullptr for safety

	// Attempt to open the file. Check if fopen was successful.
	pgmimg = fopen(imageFileName, "w");
	if (pgmimg == nullptr) {
		// Use std::cerr for error output. It's generally unbuffered.
		std::cerr << "\n*************************************************************************************\n";
		std::cerr << "Error: Could not open or create file for writing: " << imageFileName << "\n";
		std::cerr << "*************************************************************************************\n";
		// It's often better to throw an exception or return an error code
		// instead of just returning void, but adhering to original signature.
		return;
	}

	// --- PGM Header Writing ---
	// Writing Magic Number to the File (P2 for ASCII grayscale)
	fprintf(pgmimg, "P2\n");

	// Writing Width and Height
	fprintf(pgmimg, "%d %d\n", width, height);

	// Writing the maximum gray value
	fprintf(pgmimg, "255\n"); // Max pixel value for 8-bit grayscale

	// --- Image Data Writing ---
	// Iterate through rows and columns to write pixel data.
	for (int line = 0; line < height; ++line) // Use pre-increment for slight optimization
	{
		for (int column = 0; column < width; ++column) // Use pre-increment
		{
			// Calculate the correct index into the 1D 'image' array.
			// Explicitly cast to size_t for array indexing to prevent potential
			// overflow issues with large dimensions on 32-bit systems.
			unsigned char color_code = image[static_cast<size_t>(line) * width + column];
			int gray_value; // Use a temporary variable for the gray value

			// Use if-else if structure for cleaner logic than switch for non-contiguous cases.
			// Or, ideally, use a map or array for lookup if there are many mappings.
			if (color_code == EMPTY) {
				gray_value = 0;
			}
			else if (color_code == CellState::PERCOLATE) {
				gray_value = 64;
			}
			else if (color_code == CellState::SOFT) {
				gray_value = 128;
			}
			else if (color_code == CellState::HARD) {
				gray_value = 164;
			}
			else if (color_code == CellState::SIDEPATH) {
				gray_value = 200;
			}
			else if (color_code == CellState::PATH) {
				gray_value = 254;
			}
			else {
				// Handle unexpected color_code values.
				// You might want to log a warning or assign a default value.
				gray_value = 0; // Default to black for unknown codes
				std::cerr << "Warning: Unknown color code encountered: " << static_cast<int>(color_code) << " at (" << line << ", " << column << ")\n";
			}

			// Print the gray value followed by a space.
			// Using %hhu (for unsigned char) or casting to int (%d) for fprintf is common.
			// %d is generally safer as it promotes to int.
			fprintf(pgmimg, "%d ", gray_value);
		}
		// After each row, add a newline character for PGM format readability.
		fprintf(pgmimg, "\n");
	}

	// --- File Closing and Error Checking ---
	// Close the file and check the return value for potential write errors.
	int err = fclose(pgmimg);
	if (err != 0) {
		std::cerr << "\n*** Warning: Error writing to PGM Image file '" << imageFileName << "' ****\n";
	}
}
//only for internal proposes code clearnce
void setcolor(unsigned char* image, int start, unsigned char red, unsigned char green, unsigned char blue)
{
	image[start] = red;
	image[start + 1] = green;
	image[start + 2] = blue;
}

void setcolor(unsigned char* image, int start, sRGB iRGB)
{
	image[start] = iRGB.red;
	image[start + 1] = iRGB.green;
	image[start + 2] = iRGB.blue;
}


//create bitmap from array
void generateBitmapImage(char* image, int height, int width, char* imageFileName)
{

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
			unsigned char color = image[(size_t)line * width + column];
			switch (color) {

			case SOFT:setcolor(colors, 3 * column, RGB_Red); break;
			case EMPTY:setcolor(colors, 3 * column, RGB_Black); break;
			case HARD:  setcolor(colors, 3 * column, RGB_Orange);  break;
			case PERCOLATE: setcolor(colors, 3 * column, RGB_White);  break;
			case PATH:setcolor(colors, 3 * column, RGB_Yellow);  break;
			case SIDEPATH:setcolor(colors, 3 * column, RGB_Light_Blue);  break;

			}
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
			if (color == 1)
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
void saveClustersAsBitmapImage(unsigned char* image, int height, int width, char* imageFileName) {

	//bmp format
	unsigned char* fileHeader = createBitmapFileHeader(height, width);
	unsigned char* infoHeader = createBitmapInfoHeader(height, width);
	unsigned char padding[] = { 0, 0, 0 };

	//one line of image 3 colors* width
	unsigned char* colors = new unsigned char[bytesPerPixel * width]();

	int paddingSize = (4 - (width * bytesPerPixel) % 4) % 4;


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

			unsigned char color = image[(size_t)line * width + column];
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
	delete[] colors;
}

//create bitmap from array
void saveClustersAsBitmapImage(unsigned char* image, int height, int width, int totalColors, char* imageFileName) {

	//bmp format
	unsigned char* fileHeader = createBitmapFileHeader(height, width);
	unsigned char* infoHeader = createBitmapInfoHeader(height, width);
	unsigned char padding[] = { 0, 0, 0 };

	int trueColor;
	int colorWidth = (0xFFFFFF - 1) / (totalColors + 1);

	//	cout << "Color Width" << colorWidth << "\n";
		//one line of image 3 colors* width

	unsigned char* colors = new unsigned char[bytesPerPixel * width]();

	int paddingSize = (4 - (width * bytesPerPixel) % 4) % 4;


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

			unsigned char color = image[(size_t)line * width + column];
			trueColor = (int)color * colorWidth;
			red = ((char)(trueColor % (0xFF))) | 0xF;//& (0xFF);
			blue = ((char(trueColor) % 0x00FF)) | 0xF; //(trueColor >> 8) & (0xFF);
			green = ((char(trueColor) % 0x0000FF)) | 0xF; //(trueColor >> 16) & (0xFF);
			if (color > 0)
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
	err = fclose(imageFile);
	delete[] colors;
	if (err != 0)
		cout << "*** Warning: Error writing at Image File  ****\n";
}


/**
 * @brief Creates a BMP file header as a byte array.
 * @param height The height of the image.
 * @param width The width of the image.
 * @return A pointer to a static unsigned char array containing the file header bytes.
 */
unsigned char* createBitmapFileHeader(int height, int width) {
	// Calculate the size of the pixel data, ensuring each row is padded to a multiple of 4 bytes.
	int fileSize = fileHeaderSize + infoHeaderSize + bytesPerPixel * height * width;

	static unsigned char fileHeader[] = {
		0,0,    /// signature
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
		0,0,    /// number of color planes
		0,0,    /// bits per pixel
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