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

#include <algorithm>   // For std::fill_n, which efficiently fills a range with a value.
#include <iostream>    // For standard input/output operations, like std::cout.
#include <string>      // For std::string class.
#include <fstream>     // For file stream operations (e.g., std::ifstream, std::ofstream).
#include <sstream>     // For string stream operations (e.g., std::istringstream).
#include <sys/stat.h>  // For stat() function, used to get file information like size.
#include <vector>      // For std::vector dynamic arrays.
#include <list>        // For std::list doubly-linked list.

// --- Template Class for 2D Matrix ---
// A generic 2D matrix class that can store any data type 'T'.
template <typename T>
class Matrix2D
{
	T* array;     // Pointer to the dynamically allocated array that stores the matrix data.
	int width = 0;  // Width (number of columns, x-dimension) of the matrix.
	int height = 0; // Height (number of rows, y-dimension) of the matrix.

public:
	// Constructor: Initializes a 2D matrix with specified width (w) and height (h).
	// It allocates memory for the matrix data.
	Matrix2D(int w, int h) :
		array(new T[w * h]), // Dynamically allocate memory for w * h elements of type T.
		width(w),            // Initialize width.
		height(h)            // Initialize height.
	{
	}

	// Destructor: Deallocates the dynamically allocated memory when a Matrix2D object is destroyed.
	~Matrix2D()
	{
		delete[] array; // Frees the memory pointed to by 'array'.
	}

	// Accessor method: Returns the value at a specific (x, y) coordinate. (const version)
	// It's a const method, meaning it doesn't modify the object's state.
	T at(int x, int y) const { return array[index(x, y)]; }

	// Accessor method: Returns the value at a specific linear 'position' in the flattened array. (const version)
	T at(int position) const { return array[position]; }

	// Mutator method: Sets the value 'val' at a specific (x, y) coordinate.
	void set(int x, int y, T val)
	{
		array[index(x, y)] = val; // Sets the value in the underlying array.
	}

protected:
	// Helper method: Calculates the 1D array index from 2D (x, y) coordinates.
	// This uses row-major ordering (x varies fastest).
	int index(int x, int y) const { return x + width * y; }
};

// --- Template Class for 3D Matrix ---
// A generic 3D matrix class that can store any data type 'T'.
template <typename T>
class Matrix3D
{
	T* array;      // Pointer to the dynamically allocated array for 3D data.
	int width = 0;   // Width (x-dimension).
	int height = 0;  // Height (y-dimension).
	int depth = 0;   // Depth (z-dimension).

public:
	// Constructor: Initializes a 3D matrix with specified width (w), height (h), and depth (d).
	// It allocates memory for the matrix data.
	Matrix3D(int w, int h, int d) : width(w), height(h), depth(d), array(new T[w * h * d])
	{
	}

	// Destructor: Deallocates the dynamically allocated memory.
	~Matrix3D()
	{
		delete[] array; // Frees the memory.
	}

	// Accessor method: Returns the value at a specific (x, y, z) coordinate. (const version)
	T at(int x, int y, int z) const { return array[index(x, y, z)]; }

	// Accessor method: Returns the value at a specific linear 'position' in the flattened array. (const version)
	T at(int position) const { return array[position]; }

	// Mutator method: Sets the value 'val' at a specific (x, y, z) coordinate.
	void  set(int x, int y, int z, T val)
	{
		array[index(x, y, z)] = val; // Sets the value in the underlying array.
	}

protected:
	// Helper method: Calculates the 1D array index from 3D (x, y, z) coordinates.
	// This uses a specific linearization (x varies fastest, then y, then z).
	int index(int x, int y, int z) const { return x + (width * y) + (width * height) * z; }
};

// --- FD2DEL Class (Finite Difference 2D Electrical) ---
// This class likely implements a finite difference method for 2D electrical property estimation.
class FD2DEL
{
	// Private members for storing simulation data and parameters.
	double* gx; // Grid data for x-direction (e.g., current or potential gradient).
	double* gy; // Grid data for y-direction.
	double* u;  // Potential or voltage field.
	double* gb; // Boundary conditions or other specific grid values.
	double* h;  // Step sizes or grid spacing.
	double* ah; // Another grid related to h.

	int width = 0;    // Grid width.
	int heigth = 0;   // Grid height.
	int nx = 0, ny = 0, nx1 = 0, ny1 = 0, nx2 = 0, ny2 = 0, ns2 = 0; // Grid dimensions and indices.
	int components = 2; // Number of different material components or phases.
	int	_maxComponents = 100; // Maximum allowed number of components.

	Matrix2D<double>* sigma; // 2D matrix for material conductivities or permittivity.
	Matrix3D<double>* be;    // 3D matrix for basis functions or other material properties.
	double* a;               // General purpose array, possibly for material properties.

	int* list;       // Array for managing lists of elements (e.g., for sparse matrix solvers).
	int ncgsteps = 50000; // Number of conjugate gradient (CG) steps for iterative solver.

	// Private helper methods for the simulation logic.
	void current(double* currx, double* curry, double* u, double* gx, double* gy); // Calculates current.
	void ppixel(int* pix, double* a, int nphase, int ntot); // Processes pixel data.
	void bond(int* pix, double* gx, double* gy, Matrix2D<double>* sigma, Matrix3D<double>* be, int nphase, int ntot); // Calculates bond properties.
	void prod(double* gx, double* gy, double* xw, double* yw); // Performs a product operation.
	// This method likely performs a preconditioned conjugate gradient step or similar numerical operation.
	void dembx(double* gx, double* gy, double* u, int* ic, double* gb, double* h, double* Ah, int* list, int nlist, double gtest);
	// Utility function to split a string by a delimiter.
	std::vector<std::string> split(std::string strToSplit, char delimeter);

public:
	double currx = 0.0, curry = 0.0; // Current values in x and y directions.
	double gtest = 1.0e-16 * 100 * 100; // Global test parameter, possibly for convergence criteria.
	void initValues(int x, int y); // Initializes various values based on grid dimensions.

	void intitArrays(); // Initializes dynamically allocated arrays.
	int* pix;           // Pixel data, likely representing material phases at each grid point.

	// Constructor: Initializes FD2DEL with grid dimensions and component information.
	FD2DEL(int x, int y, int components, int imaxComponents);
	// Constructor: Initializes FD2DEL with only component information (grid dimensions to be set later).
	FD2DEL(int icomponents, int imaxComponents);
	~FD2DEL(void); // Destructor: Cleans up dynamically allocated memory.

	// --- File and Image Reading Methods ---
	// Reads simulation parameters or grid data from a file.
	void readFromFile(char* inputFileName);

	// Reads pixel data from an image file, storing it in a list of integers.
	size_t readFromImageFile(std::string filename, std::list<int>* pixels);
	// Reads pixel data from an image file, storing it in a list of integers, and returns image dimensions.
	size_t readFromImageFile(std::string filename, std::list<int>* pixels, int* x, int* y);

	// Gets the size of an image file and extracts its dimensions (x, y).
	long GetFileSize(std::string filename, int* x, int* y);
	// Reads pixel data from an image file sequentially.
	void readFromImageFileSequantially(char* inputFileName);
	// Reads pixel data from an image file and returns image dimensions.
	void readFromImageFile(char* inputFileName, int* x, int* y);
	// Reads simulation parameters from a file, including dimensions.
	void readFromFile(char* inputFileName, int* x, int* y); // Overload.

	// Reads material data (e.g., phases) from an unsigned character array.
	void readFromArray(unsigned char* ingadients);
	// Reads material data from an unsigned character array, mapping it to electric conductivities.
	void readFromArray(unsigned char* ingadients, double* iMaterialsElecricConductivity);

	// --- Simulation Execution Methods ---
	// Runs the simulation with input and output file names.
	// The commented-out version suggests an initial approach, replaced by a more specific one.
	// void run(char* inputFileName, char* outputFileName);
	// Runs the simulation with input file, output file, and number of phases.
	void run(char* inputFileName, char* outputFileName, int inphase);
};