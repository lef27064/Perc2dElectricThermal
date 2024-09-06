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

#include <algorithm>    // std::fill_n
#include <iostream>     // std::cout
#include <string>       // string
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <vector>
#include <list>

template <typename T>
class Matrix2D
{
	T* array;
	int width=0;   //x
	int height=0;  //y

public: Matrix2D(int w, int h) :
	
	array(new T[w * h]),
	width(w),
	height(h) {}

	  ~Matrix2D()
	  {
		  delete[] array;
	  }

	  T at(int x, int y) const { return array[index(x, y)]; }
	  T at(int position) const { return array[position]; }

	  void set(int x, int y, T val)
	  {
		  array[index(x, y)] = val;
	  }

protected: int index(int x, int y) const { return x + width * y; }
};

template <typename T>
class Matrix3D
{
	T* array;
	int width=0;  //x
	int height=0; //y;
	int depth=0;  //z;

public: Matrix3D(int w, int h, int d) : width(w), height(h), depth(d), array(new T[w * h * d])
{}

	  ~Matrix3D()
	  {
		  delete[] array;
	  }

	  T at(int x, int y, int z) const { return array[index(x, y, z)]; }
	  T at(int position) const { return array[position]; }

	  void  set(int x, int y, int z, T val)
	  {
		  array[index(x, y, z)] = val;
	  }


protected: int index(int x, int y, int z) const { return x + (width * y) + (width * height) * z; }

};

class FD2DEL
{

	double* gx;
	double* gy;
	double* u;
	double* gb;
	double* h;
	double* ah;
	
	int width=0;
	int heigth=0;
	int nx=0, ny=0, nx1=0, ny1=0, nx2=0, ny2=0, ns2=0;
	int components=2;
	int	_maxComponents = 100;

	Matrix2D<double>* sigma;
	Matrix3D<double>* be;
	double* a;
	
	int* list;
	int ncgsteps = 50000;

	void current(double* currx, double* curry, double* u, double* gx, double* gy);
	void ppixel(int* pix, double* a, int nphase, int ntot);
	void bond(int* pix, double* gx, double* gy, Matrix2D<double>* sigma, Matrix3D<double>* be, int nphase, int ntot);
	void prod(double* gx, double* gy, double* xw, double* yw);
	void dembx(double* gx, double* gy, double* u, int* ic, double* gb, double* h, double* Ah, int* list, int nlist, double gtest);
	std::vector<std::string> split(std::string strToSplit, char delimeter);

public:
	double currx=0.0, curry=0.0;
	double gtest = 1.0e-16 * 100*100;
	void initValues(int x, int y);

	void intitArrays();
	int* pix;

	FD2DEL(int x, int y, int components, int imaxComponents);
	FD2DEL(int icomponents, int imaxComponents);
	~FD2DEL(void);


	void readFromFile(char* inputFileName);
	
	size_t readFromImageFile(std::string filename, std::list<int>* pixels);

	size_t readFromImageFile(std::string filename, std::list<int>* pixels, int* x, int* y);

	//long GetFileSize(std::string filename);
	long GetFileSize(std::string filename, int* x, int* y);
	void readFromImageFileSequantially(char* inputFileName);
	void readFromImageFile(char* inputFileName, int* x, int* y);
	//void readFromFile(char* inputFileName, int* x, int* y, int components, int maxComponents);
	void readFromFile(char* inputFileName, int* x, int* y);
	void readFromArray(unsigned char* ingadients);
	void readFromArray(unsigned char* ingadients, double* iMaterialsElecricConductivity);
	//void run(char* inputFileName, char* outputFileName);
	void run(char* inputFileName, char* outputFileName, int inphase);

};
