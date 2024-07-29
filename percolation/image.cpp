

#include "image.h"




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
			char color = image[(size_t )line*width + column];
			switch (color)
			{
			case CellState::EMPTY:fprintf(pgmimg, "%d ", 0); break;
			case CellState::HARD: fprintf(pgmimg, "%d ", 64); break;
			case PERCOLATE: fprintf(pgmimg, "%d ", 128); break;
			case CellState::SOFT: fprintf(pgmimg, "%d ", 200); break;
			case BORDER:fprintf(pgmimg, "%d ", 255); break;
			}
		}
		fprintf(pgmimg, "\n");
	}
	int err  = fclose(pgmimg);
	if (err != 0)
		cout <<"*** Warning: Error writing at PGM Image  ****\n";
}

//only for internal proposes code clearnce
void setcolor(unsigned char *image,  int start, unsigned char red , unsigned char green , unsigned char blue)
{
	image[start] = red;
	image[start+1] = green;
	image[start+2] = blue;
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
			switch (color)			{
			case '0':  setcolor(colors, 3 * column, 0x00, 0x00, 0x00);  break;
			//case '1': setcolor(colors, 3 * column, 255, 160, 16);  break;
			case '1': setcolor(colors, 3 * column, 224, 224, 224);  break;
			case '2':setcolor(colors, 3 * column, 0x70, 0x00, 0x40); break;
			case '3': setcolor(colors, 3 * column, 224, 224, 224);  break;
			case '4': setcolor(colors, 3 * column, 0x40, 0x50, 0x80);  break;
			case '5': setcolor(colors, 3 * column, 255, 224, 32);  break;
			case '6': setcolor(colors, 3 * column, 0, 192, 0);  break;
			//default:setcolor(colors, 3 * column, 0x00, 0x50, 0x100); break;
			/*case CellState::EMPTY:setcolor(colors, 3 * column, 0x00, 0x00, 0x00); break;
			case CellState::HARD:  setcolor(colors, 3 * column, 0x77, 0xff, 0x10);  break;
			case CellState::PATH:setcolor(colors, 3 * column, 0x77, 0xff, 0x10);  break;
			case PERCOLATE : setcolor(colors, 3 * column, 0x77, 0xff, 0x10);  break;

			//case PERCOLATE : setcolor(colors, 3 * column, 0x33, 0x99, 0xFF);  break;

			//case CellState::PATH :setcolor(colors, 3 * column, 0x77, 0xff, 0x10);  break;
			//case CellState::PATH:setcolor(colors, 3 * column, 0x33, 0x99, 0xFF);  break;
			//case CellState::SIDEPATH:setcolor(colors, 3 * column, 0xFF, 0x99, 0x33);  break;
			case CellState::SIDEPATH: setcolor(colors, 3 * column, 0x99, 0x99, 0x99);  break;
			case CellState::SOFT:setcolor(colors, 3 * column, 0x33, 0x99, 0xFF); break;
			//case BORDER: setcolor(colors, 3 * column, 0xFF, 0xFF, 0xFF);  break;*/

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
	unsigned char red = 557756 & 0xFF;
	unsigned char blue = (557756 >> 8) & 0xFF;
	unsigned char green = (557756 >> 16) & 0xFF;
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
	fclose(imageFile);
}



unsigned char* createBitmapFileHeader(int height, int width) {
	int fileSize = fileHeaderSize + infoHeaderSize + bytesPerPixel * height*width;

	static unsigned char fileHeader[] = {
		0,0, /// signature
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
		0,0, /// number of color planes
		0,0, /// bits per pixel
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
