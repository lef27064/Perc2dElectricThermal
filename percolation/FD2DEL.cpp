/*
This file is part of Electric,Thermal, Mechanical Properties
Estimation With Percolation Theory (ETMPEWPT) (2D version) program.

Created from Eleftherios Lamprou lef27064@otenet.gr during PhD thesis (2017-2024)

ETMPEWPT is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free software Foundation, either version 3 of the License, or
(at your option) any later version.

ETMPEWPT is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Foobar. If not, see <https://www.gnu.org/licenses/>.

Theory of this is published in two papers:
1. E. Lambrou and L. N. Gergidis, “A computational method for calculating the electrical and thermal properties of random composite” ,
Physica A: Statistical Mechanics and its Applications, Volume 642, 2024, 129760, ISSN 0378-4371,
https://doi.org/10.1016/j.physa.2024.129760
2. E. Lambrou and L. N. Gergidis, “A particle digitization-based computational method for continuum percolation,” Physica A: Statistical Mechanics
and its Applications, vol. 590, p. 126738, 2022

if you use this programm and write a paper or report please cite above papers

*/


#include "FD2DEL.h" // Include the header file for the FD2DEL class
#include <cstring>  // Required for memset
#include <cmath>    // Required for sqrt

// Function to calculate the volume-averaged current
void FD2DEL::current(double* icurrx, double* icurry, double* iu, double* igx, double* igy)
{
    // Initialize the volume averaged currents to zero
    *icurrx = 0.0;
    *icurry = 0.0;
    double cur1 = 0; // Current in x-direction for a single pixel
    double cur2 = 0; // Current in y-direction for a single pixel
    int m = 0;       // Index for the current pixel

    // Loop over real sites and bonds to compute the total current
    for (int j = 2; j <= ny2 - 1; j++)
    {
        for (int i = 2; i <= nx2 - 1; i++)
        {
            m = nx2 * (j - 1) + i - 1; // Calculate the 1D index for the current pixel

            // cur1, cur2 are the currents in one pixel based on potential differences and conductances
            cur1 = 0.5 * ((iu[m - 1] - iu[m]) * igx[m - 1] + (iu[m] - iu[m + 1]) * igx[m]);
            cur2 = 0.5 * ((iu[m - nx2] - iu[m]) * igy[m - nx2] + (iu[m] - iu[m + nx2]) * igy[m]);

            // Sum pixel currents into volume averages
            *icurrx = *icurrx + cur1;
            *icurry = *icurry + cur2;
        }
    }

    // Normalize the total currents by the total number of real pixels (volume)
    *icurrx = *icurrx / double(nx * ny);
    *icurry = *icurry / double(nx * ny);

    return; // Function returns
}

// Function to process pixel phase labels and map periodic boundaries
void FD2DEL::ppixel(int* ipix, double* ia, int nphase, int ntot)
{
    // Calculate the count of each phase
    for (int j = 2; j <= ny + 1; j++)
        for (int i = 2; i <= nx + 1; i++)
        {
            int m = (j - 1) * (nx + 2) + i - 1; // Calculate the 1D index
            ia[ipix[m]] = ia[ipix[m]] + 1.0;     // Increment count for the corresponding phase
        }

    // Normalize phase counts to get volume fractions
    for (int i = 0; i < nphase; i++)
        ia[i] = ia[i] / double(nx * ny);

    int j1, i1, m, m1;

    // Map periodic boundaries of pix (refer to Section 3.3, Figure 3 in manual)
    for (int j = 0; j < ny2; j++)
        for (int i = 0; i < nx2; i++)
        {
            // Check if current pixel is on the boundary
            if ((i == 0) || (i == (nx2 - 1)) || (j == 0) || (j == (ny2 - 1)))
            {
                j1 = j;
                if (j == 0)
                    j1 = j + ny; // Map top boundary to bottom
                if (j == ny2 - 1)
                    j1 = j - ny; // Map bottom boundary to top
                i1 = i;
                if (i == 0)
                    i1 = i + nx; // Map left boundary to right
                if (i == nx2 - 1)
                    i1 = i - nx; // Map right boundary to left

                m = (j)*nx2 + i;     // Index of the boundary pixel
                m1 = (j1)*nx2 + i1; // Index of the corresponding real pixel

                ipix[m] = ipix[m1]; // Assign phase label from the real pixel to the boundary pixel
            }
        }

    // Check for wrong phase labels (less than 0 or greater than nphase - 1)
    for (int im = 0; im < ns2 - 1; im++)
    {
        if (ipix[im] < 0)
            std::cout << "Phase label in pix < 0--error at " << im << "\n";

        if (ipix[im] > nphase - 1)
            std::cout << "Phase label in pix > nphase--error at " << im << "\n";
    }
}

// Subroutine that determines the correct bond conductances used for matrix A
void FD2DEL::bond(int* ipix, double* igx, double* igy, Matrix2D<double>* isigma, Matrix3D<double>* ibe, int nphase, int ntot)
{
    // Auxiliary variables involving the system size
    int j1;
    int i1;
    int m1;
    int i, j, m;

    // Set values of conductor for phase(i)-phase(j) interface, store in array ibe(i,j,m).
    // If either phase i or j has zero conductivity, then ibe(i,j,m) = 0.
    for (m = 0; m < 2; ++m) // Loop for x and y directions (0 for x, 1 for y)
    {
        for (i = 0; i < nphase; ++i) // Loop for phase i
        {
            for (j = 0; j < nphase; ++j) // Loop for phase j
            {
                if (isigma->at(i, m) == 0.0) // If conductivity of phase i is zero
                    ibe->set(i, j, m, 0.0);  // Interface conductivity is zero
                else if (isigma->at(j, m) == 0.0) // If conductivity of phase j is zero
                    ibe->set(i, j, m, 0.0);   // Interface conductivity is zero
                else
                    // Calculate effective conductivity for the interface using harmonic mean
                    ibe->set(i, j, m, 1. / ((0.5 / isigma->at(i, m)) + (0.5 / isigma->at(j, m))));
            }
        }
    }

    // Trim off x-face so that no current can flow past periodic boundaries.
    // This step is not strictly necessary as periodic boundary conditions are handled in CG.
    for (j = 1; j < ny2; ++j)
    {
        igx[nx2 * (j - 1) + nx2 - 1] = 0.0; // Set conductance on the rightmost x-bonds to zero
    }

    // Calculate conductances for y-direction bonds (igy) in the bulk
    for (i = 1; i <= nx2; ++i)
    {
        for (j = 1; j <= ny1; ++j)
        {
            m = (j - 1) * nx2 + i;     // Current pixel index
            j1 = j + 1;                // Next pixel in y-direction
            i1 = i;                    // Same x-coordinate
            m1 = (j1 - 1) * nx2 + i1; // Index of the next pixel in y-direction
            // Assign interface conductivity based on the phases of the current and next pixel
            igy[m - 1] = ibe->at(ipix[m - 1], ipix[m1 - 1], 1);
        }
    }

    // Calculate conductances for x-direction bonds (igx) in the bulk
    for (i = 1; i <= nx1; ++i)
    {
        for (j = 1; j <= ny2; ++j)
        {
            m = (j - 1) * nx2 + i;     // Current pixel index
            i1 = i + 1;                // Next pixel in x-direction
            j1 = j;                    // Same y-coordinate
            m1 = (j1 - 1) * nx2 + i1; // Index of the next pixel in x-direction
            // Assign interface conductivity based on the phases of the current and next pixel
            igx[m - 1] = ibe->at(ipix[m - 1], ipix[m1 - 1], 0);
        }
    }
    return; // Function returns
}

// Function to perform matrix-vector product (A * xw)
void FD2DEL::prod(double* igx, double* igy, double* xw, double* yw)
{
    // xw is the input vector (e.g., voltage), yw = (A)(xw) is the output vector (e.g., negative divergence of current)

    int i, j;
    // Initialize yw to zero
    std::fill_n(yw, ns2, 0);

    // Perform basic matrix multiplication for interior points, neglecting periodic boundaries for now.
    // This calculates the divergence of current at each node.
    for (i = nx2 + 1; i <= ns2 - nx2; ++i)
    {
        // This is effectively - (sum of conductances connected to node i) * xw[i]
        yw[i] = -xw[i] * (igx[i - 1] + igx[i] + igy[i] + igy[i - nx2]);
        // Add terms from neighboring nodes (conductance * neighbor's voltage)
        yw[i] = yw[i] + igx[i - 1] * xw[i - 1] + igx[i] * xw[i + 1] + igy[i] * xw[i + nx2] + igy[i - nx2] * xw[i - nx2];
    }

    // Correct terms at periodic boundaries (refer to Section 3.3 in manual)

    // x faces (left and right boundaries)
    for (j = 1; j <= ny2; ++j)
    {
        // Copy values from the opposite periodic boundary for x-direction
        yw[nx2 * (j - 1) + nx2 - 1] = yw[nx2 * (j - 1) + 1]; // Rightmost boundary influenced by left interior
        yw[nx2 * (j - 1)] = yw[nx2 * (j - 1) + nx1 - 1];     // Leftmost boundary influenced by right interior
    }

    // y faces (top and bottom boundaries)
    for (i = 1; i <= nx2; ++i)
    {
        // Copy values from the opposite periodic boundary for y-direction
        yw[i - 1] = yw[ny * nx2 + i - 1];     // Topmost boundary influenced by bottom interior
        yw[ny1 * nx2 + i - 1] = yw[nx2 + i - 1]; // Bottommost boundary influenced by top interior
    }
    return; // Function returns
}

// Subroutine for solving the system using Conjugate Gradient method for electrical properties
void FD2DEL::dembx(double* igx, double* igy, double* iu, int* ic, double* iigb, double* h, double* Ah, int* list, int nlist, double gtest)
{
    double gg, hAh, lambda, gglast, gamma, currx, curry;

    // Note: voltage gradients are maintained because in the conjugate gradient
    // relaxation algorithm, the voltage vector is only modified by adding a
    // periodic vector to it.

    // First stage: compute initial value of gradient (iigb), initialize h (conjugate gradient direction),
    // and compute norm squared of gradient vector (gg).

    // Calculate initial gradient (iigb) as A * iu
    prod(igx, igy, iu, iigb);
    // Initialize conjugate gradient direction 'h' with the gradient
    for (int i = 0; i < ns2; ++i)
        h[i] = iigb[i];

    // Variable gg is the norm squared of the gradient vector
    gg = 0.0;
    // Sum squares of gradient components only for real (interior) sites
    for (int k = 0; k < nlist; ++k)
    {
        int m = list[k];
        gg = (iigb[m - 1] * iigb[m - 1]) + gg;
    }

    hAh = 0; // Initialize hAh for the first step

    // Second stage: initialize Ah variable, compute parameter lambda,
    // make first change in voltage array, update gradient (iigb) vector.
    int icc = 0; // Conjugate gradient iteration counter
    if (gg > gtest) // Proceed if the initial gradient norm is above the tolerance
    {
        // Calculate A * h
        prod(igx, igy, h, Ah);

        // Compute hAh (h dot Ah) for real sites
        for (int k = 1; k <= nlist; ++k)
        {
            int m = list[k - 1];
            hAh = hAh + (h[m - 1] * Ah[m - 1]);
        }
        // Calculate lambda (step size)
        lambda = gg / hAh;
        // Update voltage and gradient vectors
        for (int i = 0; i < ns2; ++i)
        {
            iu[i] = iu[i] - lambda * h[i];    // Update voltage
            iigb[i] = iigb[i] - lambda * Ah[i]; // Update gradient
        }

        // Third stage: iterate conjugate gradient solution process until
        // real(gg) < gtest criterion is satisfied.
        // (USER) The parameter ncgsteps is the total number of conjugate gradient steps
        // to go through. Only in very unusual problems, like when the conductivity
        // of one phase is much higher than all the rest, will this many steps be
        // used.

        gg = 1e25; // Reset gg to a large value to enter the loop
        // Loop for conjugate gradient iterations
        for (icc = 0; ((icc < ncgsteps) && (gg > gtest)); ++icc)
        {
            gglast = gg; // Store previous gradient norm squared
            gg = 0.0;    // Reset current gradient norm squared

            // Recalculate current gradient norm squared for real sites
            for (int k = 0; k < nlist; ++k)
            {
                int m = list[k];
                gg = (iigb[m - 1] * iigb[m - 1]) + gg;
            }

            // Calculate gamma (Fletcher-Reeves or Polak-Ribiere parameter)
            gamma = gg / gglast;

            // Update conjugate gradient direction
            for (int i = 0; i < ns2; ++i)
            {
                h[i] = iigb[i] + gamma * h[i];
            }

            // Calculate A * h for the updated direction
            prod(igx, igy, h, Ah);
            hAh = 0.0; // Reset hAh

            // Compute hAh (h dot Ah) for real sites
            for (int k = 0; k < nlist; ++k)
            {
                int m = list[k];
                hAh = hAh + h[m - 1] * Ah[m - 1];
            }

            // Calculate lambda (step size)
            lambda = gg / hAh;

            // Update voltage and gradient vectors
            for (int i = 0; i < ns2; ++i)
            {
                iu[i] = iu[i] - (lambda * h[i]);    // Update voltage
                iigb[i] = iigb[i] - (lambda * Ah[i]); // Update gradient
            }

            // (USER) This piece of code forces dembx to write out the total current and
            // the norm of the gradient squared, every ncheck conjugate gradient steps,
            // in order to see how the relaxation is proceeding. If the currents become
            // unchanging before the relaxation is done, then gtest was picked to be
            // smaller than was necessary.

            int ncheck = 5000; // Check frequency

            if ((icc % ncheck) == 0) // If current iteration is a multiple of ncheck
            {
                std::cout << icc;                 // Print current iteration
                std::cout << " gg = " << gg << "\n"; // Print norm squared of gradient
                // Call current subroutine to calculate and print average currents
                current(&currx, &curry, iu, igx, igy);
                std::cout << " currx = " << currx << "\n";
                std::cout << " curry = " << curry << "\n";
                std::cout << "number of conjugate gradient cycles needed = " << icc << "\n";
            }
        }
        // If the loop finishes because icc reached ncgsteps, it means iteration failed to converge
        if (icc == ncgsteps) {
            std::cout << " iteration failed to converge after" << ncgsteps << " steps";
        }
    }

    *ic = icc; // Store the total number of iterations
    return;    // Function returns
}

// Function to initialize grid dimensions
void FD2DEL::initValues(int x, int y)
{
    width = x; heigth = y; // Store width and height
    nx = x; ny = y;         // Set dimensions for real sites
    nx1 = x + 1; ny1 = y + 1; // Set dimensions including one padding layer
    nx2 = x + 2; ny2 = y + 2; // Set dimensions including two padding layers for boundary conditions
    ns2 = nx2 * ny2;         // Total number of sites including padding
}

// Function to allocate and initialize arrays
void FD2DEL::intitArrays()
{
    // Allocate memory for various arrays using OpenMP for parallel allocation
#pragma omp parallel sections
    {
#pragma omp section
        {
            gx = new double[ns2];    // x-direction conductances
            std::memset(gx, 0, ns2 * sizeof(double)); // Initialize to zero
        }
#pragma omp section
        {
            gy = new double[ns2];    // y-direction conductances
            memset(gy, 0, ns2 * sizeof(double)); // Initialize to zero
        }
#pragma omp section
        {
            u = new double[ns2];     // Voltage field
            memset(u, 0, ns2 * sizeof(double));  // Initialize to zero
        }
#pragma omp section
        {
            gb = new double[ns2];    // Gradient vector
            memset(gb, 0, ns2 * sizeof(double)); // Initialize to zero
        }
#pragma omp section
        {
            h = new double[ns2];     // Conjugate gradient direction vector
            memset(h, 0, ns2 * sizeof(double));  // Initialize to zero
        }
#pragma omp section
        {
            ah = new double[ns2];    // A * h vector
            memset(ah, 0, ns2 * sizeof(double)); // Initialize to zero
        }
#pragma omp section
        {
            a = new double[ns2];     // Placeholder (possibly for volume fractions or temporary calculations)
            memset(a, 0, ns2 * sizeof(double));  // Initialize to zero
        }
#pragma omp section
        {
            pix = new int[ns2];;      // Pixel phase labels
            memset(pix, 0, ns2 * sizeof(int));   // Initialize to zero
        }
#pragma omp section
        {
            list = new int[ns2];     // List of real (interior) sites
            memset(list, 0, ns2 * sizeof(int));  // Initialize to zero
        }
    }
}

// Constructor for FD2DEL class when grid dimensions are known
FD2DEL::FD2DEL(int x, int y, int icomponents, int i_maxComponents) :width(x), heigth(y),
nx(x), ny(y), nx1(x + 1), ny1(y + 1), nx2(x + 2), ny2(y + 2), components(icomponents), _maxComponents(i_maxComponents)
{
    int size = (nx2) * (ny2); // Calculate total size including padding
    ns2 = size;              // Store total size
    intitArrays();           // Allocate and initialize arrays

    // sigma and be are initialized in the other constructor or handled separately
    // sigma = new Matrix2D<double>(_maxComponents, 2);
    // be = new Matrix3D<double>(_maxComponents, _maxComponents, 2);
}

// Constructor for FD2DEL class when only components information is known
FD2DEL::FD2DEL(int icomponents, int i_maxComponents) : components(icomponents), _maxComponents(i_maxComponents)
{
    // Allocate Matrix2D for conductivity and Matrix3D for interface conductances
    sigma = new Matrix2D<double>(_maxComponents, 2);
    be = new Matrix3D<double>(_maxComponents, _maxComponents, 2);
}

// Destructor for FD2DEL class to deallocate dynamically allocated memory
FD2DEL::~FD2DEL(void)
{
    // Delete all dynamically allocated arrays
    delete[] gx;
    delete[] gy;
    delete[] u;
    delete[] gb;
    delete[] h;
    delete[] ah;
    delete[] a;
    delete[] pix;
    delete[] list;
    // Call destructors for Matrix2D and Matrix3D objects
    sigma->~Matrix2D();
    be->~Matrix3D();
}

// Old run function, commented out
// void FD2DEL::run(char* inputFileName, char* outputFileName, int nx, int ny)
// {}

// Function to read pixel data from a file
void FD2DEL::readFromFile(char* inputFileName)
{
    std::string line;
    std::ifstream inputFile(inputFileName); // Open the input file
    if (inputFile.is_open())               // Check if the file opened successfully
    {
        // Read pixel values and store them in the pix array
        for (int i = 2; i <= nx + 1; i++)
            for (int j = 1; j <= ny + 1; j++)
            {
                getline(inputFile, line);              // Read a line from the file
                int m = (j - 1) * (nx + 2) + i;       // Calculate the 1D index
                pix[m - 1] = std::stoi(line) - 1; // Convert string to int and store (adjusting for 0-based indexing)
            }
    }
}

/*
// Function to get file size (commented out, likely replaced by another version)
long GetFileSize(std::string filename)
{
    struct stat stat_buf;
    int rc = stat(filename.c_str(), &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}
*/

// Function to split a string by a delimiter
std::vector<std::string> FD2DEL::split(std::string strToSplit, char delimeter)
{
    std::stringstream ss(strToSplit);  // Create a stringstream from the input string
    std::string item;                   // Temporary string to hold split items
    std::vector<std::string> splittedStrings; // Vector to store the split strings
    while (std::getline(ss, item, delimeter)) // Read items separated by the delimiter
    {
        splittedStrings.push_back(item); // Add the item to the vector
    }
    return splittedStrings; // Return the vector of split strings
}

// Function to read pixel data from an image file into a list
size_t FD2DEL::readFromImageFile(std::string filename, std::list<int>* pixels)
{
    std::string line;
    std::ifstream inputFile(filename); // Open the input file
    std::vector<std::string> strComponent; // Vector to hold string components
    size_t count = 0;                     // Counter for total pixels
    while (getline(inputFile, line))      // Read file line by line
    {
        strComponent = split(line, ' '); // Split the line by space
        for (int i = 0; i < strComponent.size(); i++)
        {
            pixels->push_back(std::stoi(strComponent[i])); // Convert to int and add to list
        }
        count = count + strComponent.size(); // Update pixel count
    }
    inputFile.close(); // Close the file
    return count;      // Return total pixel count
}

// Function to read pixel data from an image file into a list and determine dimensions
size_t FD2DEL::readFromImageFile(std::string filename, std::list<int>* pixels, int* x, int* y)
{
    std::string line;
    std::ifstream inputFile(filename); // Open the input file
    std::vector<std::string> strComponent; // Vector to hold string components
    long count = 0;                    // Counter for total pixels
    *x = 0;                             // Initialize x dimension
    *y = 0;                             // Initialize y dimension
    while (getline(inputFile, line))   // Read file line by line
    {
        strComponent = split(line, ' '); // Split the line by space
        *x = strComponent.size();        // The width is the number of components in a line
        for (int i = 0; i < strComponent.size(); i++)
        {
            pixels->push_back(std::stoi(strComponent[i])); // Convert to int and add to list
        }
        count = count + strComponent.size(); // Update pixel count
        *y = *y + 1;                         // Increment y dimension (number of rows)
    }
    inputFile.close(); // Close the file
    return count;      // Return total pixel count
}

// Function to get file size and dimensions without reading all data into memory
long FD2DEL::GetFileSize(std::string filename, int* x, int* y)
{
    std::string line;
    std::ifstream inputFile(filename); // Open the input file
    std::vector<std::string> strComponent; // Vector to hold string components
    long count = 0;                    // Counter for total pixels
    *x = 0;                             // Initialize x dimension
    *y = 0;                             // Initialize y dimension
    while (getline(inputFile, line))   // Read file line by line
    {
        strComponent = split(line, ' '); // Split the line by space
        *x = strComponent.size();        // The width is the number of components in a line
        count = count + *x;              // Update pixel count
        *y = *y + 1;                     // Increment y dimension (number of rows)
    }
    inputFile.close(); // Close the file
    return count;      // Return total pixel count
}

// Function to read pixel data from an image file sequentially (row by row)
void FD2DEL::readFromImageFileSequantially(char* inputFileName)
{
    std::string line;
    std::ifstream inputFile(inputFileName); // Open the input file

    if (inputFile.is_open())               // Check if the file opened successfully
    {
        // Iterate through the grid, reading one pixel value at a time
        for (int i = 2; i <= nx + 1; i++)
            for (int j = 1; j < ny + 1; j++)
            {
                getline(inputFile, line);              // Read a line (pixel value)
                int m = (j - 1) * (nx + 2) + i;       // Calculate 1D index for the pix array
                pix[m - 1] = std::stoi(line) - 1; // Convert string to int and store (adjusting for 0-based indexing)
            }
    }
}

// Function to read pixel data from an unsigned char array (gradients)
void FD2DEL::readFromArray(unsigned char* ingadients)
{
    // Iterate through the grid, mapping 2D input array to 1D pix array
    for (int i = 2; i <= nx + 1; i++)
        for (int j = 1; j <= ny + 1; j++)
        {
            int m = (j - 1) * nx2 + i;        // Calculate 1D index for the pix array (with padding)
            int m1 = ((j - 1) * nx) + i - 2; // Calculate 1D index for the input 'ingadients' array (without padding, 0-based)
            pix[m - 1] = ingadients[m1];      // Assign the gradient value as the pixel phase
        }
}

// Function to read pixel data from an unsigned char array and set material conductivities
void FD2DEL::readFromArray(unsigned char* ingadients, double* iMaterialsElecricConductivity)
{
    // Iterate through the grid, mapping 2D input array to 1D pix array
    for (int j = 2; j <= ny + 1; j++)
        for (int i = 2; i <= nx + 1; i++)
        {
            int m = (j - 1) * nx2 + i - 1;    // Calculate 1D index for the pix array (with padding)
            int m1 = ((j - 2) * nx) + i - 2; // Calculate 1D index for the input 'ingadients' array (without padding, 0-based)
            pix[m] = ingadients[m1];          // Assign the gradient value as the pixel phase
        }

    // Set electrical conductivities for each component (phase)
    for (int i = 0; i < components; i++)
    {
        sigma->set(i, 0, iMaterialsElecricConductivity[i]); // Set conductivity for x-direction
        sigma->set(i, 1, iMaterialsElecricConductivity[i]); // Set conductivity for y-direction
    }
}

// Function to read pixel data from an image file and initialize grid
void FD2DEL::readFromImageFile(char* inputFileName, int* x, int* y)
{
    std::list<int> data;                      // Temporary list to store pixel data
    // Get file size and dimensions, and read data into the list
    long fileSize = readFromImageFile(inputFileName, &data, x, y);

    // If either x or y dimension is 1 (meaning it's a 1D array), assume square shape
    if ((*x == 1) || (*y == 1))
    {
        *x = int(std::sqrt(fileSize)); // Calculate side length for a square image
        *y = *x;
        initValues(*x, *y);  // Initialize grid values
        intitArrays();       // Allocate arrays
    }
    else // If dimensions are already determined
    {
        initValues(*x, *y);  // Initialize grid values
        intitArrays();       // Allocate arrays
    }

    // Populate the 'pix' array from the 'data' list
    for (int j = 2; j <= ny + 1; j++)
        for (int i = 2; i <= nx + 1; i++)
        {
            int m = (j - 1) * nx2 + i;      // Calculate 1D index for the pix array
            pix[m - 1] = data.front() - 1; // Get first element, adjust for 0-based index, and store
            data.pop_front();              // Remove the element from the list
            std::cout << m - 1 << "," << pix[m - 1] << "\n"; // Debug output
        }
}

// Function to read pixel data from a file and initialize grid (similar to readFromImageFile but without list)
void FD2DEL::readFromFile(char* inputFileName, int* x, int* y)
{
    // Get file size and dimensions
    long fileSize = GetFileSize(inputFileName, x, y);
    // If either x or y dimension is 1 (meaning it's a 1D array), assume square shape
    if ((*x == 1) || (*y == 1))
    {
        *x = int(sqrt(fileSize)); // Calculate side length for a square image
        *y = *x;
        initValues(*x, *y);  // Initialize grid values
        intitArrays();       // Allocate arrays
    }
    else // If dimensions are already determined
    {
        initValues(*x, *y);  // Initialize grid values
        intitArrays();       // Allocate arrays
    }

    std::string line;
    std::ifstream inputFile(inputFileName); // Open the input file

    if (inputFile.is_open())               // Check if the file opened successfully
    {
        // Read pixel values and store them in the pix array
        for (int j = 2; j <= ny2 - 1; j++)
            for (int i = 2; i <= ny2 - 1; i++)
            {
                getline(inputFile, line);              // Read a line from the file
                int m = (j - 1) * nx2 + i;            // Calculate the 1D index
                pix[m - 1] = std::stoi(line) - 1; // Convert string to int and store (adjusting for 0-based indexing)
                std::cout << m << "," << pix[m - 1] << "\n"; // Debug output
            }
    }
    inputFile.close(); // Close the file
}

// Main function to run the simulation
void FD2DEL::run(char* inputFileName, char* outputFileName, int inphase)
{
    std::cout << "image is " << nx << " x " << ny << " no of real sites =" << nx * ny << "\n";

    // (USER) nphase is the number of phases being considered in the problem.
    // The values of pix(m) will run from 1 to nphase. ntot is the total
    // number of phases possible in the program, and is the dimension of
    // sigma, a, and be.

    int nphase = inphase;   // Number of phases in the current problem
    int ntot = 100;         // Total number of phases possible (dimension for arrays)
    // Make list of real (interior) sites, used in subroutine dembx. The 1-d
    // labelling scheme goes over all ns2 sites, so a list of the real sites
    // is needed.
    int nlist = 0;          // Counter for the list of real sites

    // Populate the 'list' array with indices of real (interior) sites
    for (int i = 2; i <= nx1; ++i)
    {
        for (int j = 2; j <= ny1; ++j)
        {
            int m = i + (j - 1) * nx2; // Calculate 1D index for the current site
            list[nlist] = m - 1;       // Store the 0-based index
            nlist++;                   // Increment real site counter
        }
    }

    gtest = 1.0e-16 * this->ns2; // Set the convergence tolerance for the conjugate gradient method

    // Compute average current in each pixel and output phase volume fractions.
    // (USER) npoints is the number of microstructures to use.
    ppixel(pix, a, nphase, ntot); // Calculate phase volume fractions and map periodic boundaries

    // Output phase volume fractions
    for (int i = 0; i < nphase; ++i)
    {
        std::cout << "phase fraction of " << i << " = " << a[i] << "\n";
    }

    // (USER) Set components of applied field, E = (ex,ey)
    double ex = 0.0; // Applied electric field in x-direction
    double ey = 1.0; // Applied electric field in y-direction

    std::cout << "Applied field components : ex =" << ex << " ey = " << ey << "\n";

    // Initialize the voltage distribution by applying a uniform field.
    for (int j = 1; j <= ny2; ++j)
    {
        for (int i = 0; i < nx2; ++i)
        {
            int m = (nx2 * (j - 1)) + i; // Calculate 1D index
            u[m] = -ex * i - ey * j;     // Set initial voltage based on applied field
        }
    }

    // (USER) input value of real conductivity tensor for each phase
    // (diagonal only). 1,2 = x,y, respectively.
    // This part assumes that `sigma` is already populated or will be.
    // The previous `readFromArray` function sets this if used.

    // bond() sets up conductor network in gx,gy 1-d arrays
    bond(pix, gx, gy, sigma, be, nphase, ntot); // Calculate bond conductances

    // Subroutine dembx accepts gx,gy and solves for the voltage field F
    // that minimizes the dissipated energy.
    int ic = 0; // Conjugate gradient iteration counter
    dembx(gx, gy, u, &ic, gb, h, ah, list, nlist, gtest); // Solve for voltage field

    // Find final current after voltage solution is done
    current(&currx, &curry, u, gx, gy); // Calculate average currents

    // Output final results
    std::cout << "\naverage current in x direction = " << currx << "\n";
    std::cout << "average current in y direction = " << curry << "\n";
    std::cout << "number of conjugate gradient cycles needed = " << ic << "\n";
}