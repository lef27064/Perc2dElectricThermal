/*
This file is part of Electric,Thermal, Mechanical Properties
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

#pragma once // Ensures this header file is included only once in a compilation
//#define _CRT_SECURE_NO_WARNINGS // Uncomment to disable specific Visual Studio CRT warnings

// Standard C++ Library Includes
#include <iostream>     // For input/output operations (e.g., cout, cin)
#include <cmath>        // For mathematical functions (e.g., sqrt, pow)
#include <fstream>      // For file stream operations (e.g., ifstream, ofstream)
#include <string>       // For string manipulation
#include <random>       // For random number generation
#include <limits>       // For numeric_limits (e.g., infinity)
#include <algorithm>    // For algorithms like min, max, sort
#include <time.h>       // For time-related functions (e.g., clock)
#include <stdlib.h>     // For general utilities (e.g., malloc, free, rand)
#include <vector>       // For dynamic arrays (std::vector)
#include <sstream>      // For string stream operations (e.g., stringstream)
#include <sys/types.h>  // For system data types (often used with sys/stat.h)
#include <sys/stat.h>   // For file/directory status (e.g., mkdir)
#include <chrono>       // For time durations and points (C++11 and later)
#include <ctime>        // For C-style date and time functions
#include <iomanip>      // For output formatting (e.g., setprecision, setw)
#include <omp.h>        // For OpenMP (parallel programming)
#include <filesystem>   // For filesystem operations (C++17 and later)

// Project-specific Header Includes
#include "Shapes.h"     // Defines shape structures and related functionalities
#include "Grid.h"       // Defines the Grid class for the simulation domain
#include "Settings.h"   // Defines settings and configuration for the program
#include "FD2DEL.h"     // Likely related to Finite Difference Method for 2D Electrical problems

using namespace std; // Using the standard namespace to avoid std:: prefix

// Preprocessor defines for maximum capacities
#define maxComponents   5       // Maximum number of different material components
#define maxCases        15000   // Maximum number of simulation cases to run

/**
 * @brief The ShapeGenerator class is responsible for generating and managing
 * geometric shapes (particles) within the simulation grid.
 * It handles material properties, simulation parameters, and stores results.
 */
class ShapeGenerator {
private:
    std::mt19937 eng;           // Mersenne Twister pseudo-random number generator engine
    std::random_device r;       // Non-deterministic random number generator (used for seeding mt19937)
    //std::seed_seq seed;       // Commented out: Used for seeding random engines with multiple values
    /*
    // Commented out: Distributions for random number generation (e.g., for position, angle, size)
    std::uniform_int_distribution<int> distx;
    std::uniform_int_distribution<int> disty;
    std::uniform_int_distribution<int> distangle;
    std::uniform_int_distribution<int> distsize;
    */

public:
    string projectName = {};    // Name of the current project
    string projectDir = {};     // Directory for the project's output files
    bool swissCheese = true;    // Flag to enable/disable "swiss cheese" model (voids/holes)
    int iterations = 0;         // Number of iterations for simulations (e.g., Monte Carlo)
    bool calcElectricConductivity = false;          // Flag to calculate electric conductivity
    bool calcElectricConductivityWithFDM = false;   // Flag to calculate electric conductivity using FDM
    bool calcStatistcs = false; // Flag to calculate and report statistics
    static int objectCount;     // Static counter for generated objects (shapes)

    int width = 0;              // Width of the simulation grid
    int height = 0;             // Height of the simulation grid
    int totalComponents = 0;    // Total number of active material components
    // Array to store the real area of each component for each case
    double realComponentAreas[maxComponents * maxCases] = {};

    double components[maxComponents] = {};          // Percentage weights of each component
    double specialWeights[maxComponents] = {};      // Special weights for components (specific use case)
    double dimensionX[maxComponents] = {};          // X-dimension (e.g., width, radius_x) of shapes for each component
    double dimensionY[maxComponents] = {};          // Y-dimension (e.g., height, radius_y) of shapes for each component
    ShapeType componentsType[maxComponents] = {};   // Type of shape for each component (e.g., circle, rectangle, ellipse)
    SizeType componentsSizeType[maxComponents] = {};// Type of size distribution for components
    double componentsArea[maxComponents] = {};      // Pre-calculated area of each component (e.g., unit area)
    double hoops[maxComponents] = {};               // Hoops distance (e.g., minimum distance between particles)
    // Material properties for each component
    double materialsElectricConductivity[maxComponents] = {};   // Electric conductivities
    double materialsThermalConductivity[maxComponents] = {};    // Thermal conductivities
    double materialsYoungModulus[maxComponents] = {};           // Young's moduli
    double materialsPoissonRatio[maxComponents] = {};           // Poisson's ratios

    Grid* grid; // Pointer to the simulation grid object

    // Arrays to store simulation results for each case
    int Results[maxCases] = {};                     // General integer results per case
    float FDResults[maxCases * 2] = {};             // Finite Difference Method results (e.g., electrical potential)
    double Times[maxCases] = {};                    // Total time taken for each case
    double setUpTimes[maxCases] = {};               // Setup time for each case
    double calcComponents[maxComponents] = {};      // Calculation results specific to components
    double paths[maxCases] = {};                    // Number of paths found (e.g., percolation paths)
    double meanPathLength[maxCases] = {};           // Mean path length per case
    double meanRealPathLength[maxCases] = {};       // Mean real path length per case

    // Calculated macroscopic properties for each case
    double electricConductivity[maxCases] = {};     // Electric conductivity
    double thermalConductivities[maxCases] = {};    // Thermal conductivity
    double YoungModulus[maxCases] = {};             // Young's modulus
    double PoissonRatio[maxCases] = {};             // Poisson's ratio

    // Mean RVE (Representative Volume Element) properties for each case (related to averaging)
    double meanRVEResistances[maxCases] = {};
    double meanRVEThermalResistance[maxCases] = {};
    double meanRVEYoungModulus[maxCases] = {};
    double meanRVEPoissoonRatio[maxCases] = {};

    // Overall mean properties across all cases
    double meanElectricConductivity = 0;
    double meanThermalConductivity = 0;
    double meanYoungModulus = 0;
    double meanPoissonRatio = 0;

    int pixelsPerMinimumSize = 0;   // Resolution setting: pixels per minimum shape size
    double min = 0;                 // Minimum value for a distribution or property
    double max = 0;                 // Maximum value for a distribution or property
    double factor = 0;              // Scaling factor or other general factor

    double meanPercolation = 0;     // Mean percolation probability or threshold
    double meanTime = 0;            // Mean total simulation time
    double meanSetUpTime = 0;       // Mean setup time
    Settings* settings;             // Pointer to the program settings object
    vector<double> correleationLengths; // Vector to store correlation lengths for each case
    double correleationLength = 0;  // Mean correlation length

    double thresh = 0;              // Threshold value (e.g., for percolation)

public:
    /**
     * @brief Initializes project directories.
     */
    void initDirs(void);

    /**
     * @brief Constructor for ShapeGenerator with detailed initialization parameters.
     * @param tcomp Total number of components.
     * @param comp Array of component weights.
     * @param compType Array of component shape types.
     * @param compSizeType Array of component size types.
     * @param sWeights Array of special weights.
     * @param dimX Array of X-dimensions for shapes.
     * @param dimY Array of Y-dimensions for shapes.
     * @param ihoops Array of hoop distances.
     * @param iGrid Pointer to the Grid object.
     * @param settings Pointer to the Settings object.
     */
    ShapeGenerator(int tcomp, double comp[], ShapeType compType[], SizeType compSizeType[], double sWeights[], double dimX[], double dimY[], double ihoops[], Grid* iGrid, Settings* settings);

    /**
     * @brief Default constructor for ShapeGenerator.
     */
    ShapeGenerator();

    /**
     * @brief Solves for areas, likely related to component areas or overlaps.
     */
    void areaSolve(void);

    /**
     * @brief Checks if a given Point (double coordinates) is inside the grid.
     * @param cPoint The point to check.
     * @param iGrid Pointer to the Grid object.
     * @return True if the point is inside the grid, false otherwise.
     */
    bool isInsideGrid(Point cPoint, Grid* iGrid);

    /**
     * @brief Checks if a given iPoint (integer coordinates) is inside the grid.
     * @param cPoint The integer point to check.
     * @param iGrid Pointer to the Grid object.
     * @return True if the point is inside the grid, false otherwise.
     */
    bool isInsideGrid(iPoint cPoint, Grid* iGrid);

    /**
     * @brief Splits a string by a given delimiter.
     * @param strToSplit The string to split.
     * @param delimeter The character to split by.
     * @return A vector of strings containing the split parts.
     */
    std::vector<std::string> split(std::string strToSplit, char delimeter);

    /**
     * @brief Scales a point based on given scale factors.
     * @param orig The original point.
     * @param scaled Pointer to the scaled point.
     * @param scaleX The X-axis scaling factor.
     * @param scaleY The Y-axis scaling factor.
     */
    void scale(Point orig, Point* scaled, double scaleX, double scaleY);

    /**
     * @brief Sets up a simulation case for a lattice-based model.
     * @param caseNo The current case number.
     * @param setUpTime Pointer to store the setup time for this case.
     */
    void setupCaseLattice(int caseNo, double* setUpTime);

    /**
     * @brief Prints particle information for a specific case.
     * @param caseNo The current case number.
     * @param totalEllipsesPerComponent Vector of ellipse counts per component.
     * @param totalCirclesPerComponent Vector of circle counts per component.
     * @param totalRectanglesPerComponent Vector of rectangle counts per component.
     * @param totalSlopedRectanglesPerComponent Vector of sloped rectangle counts per component.
     */
    void printParticles(int caseNo, vector<int> totalEllipsesPerComponent, vector<int> totalCirclesPerComponent, vector<int> totalRectanglesPerComponent, vector<int> totalSlopedRectanglesPerComponent);

    /**
     * @brief Sets up a general simulation case (non-lattice specific).
     * @param caseNo The current case number.
     * @param setUpTime Pointer to store the setup time for this case.
     */
    void setupCase(int caseNo, double* setUpTime);

    /**
     * @brief Adds a single sloped rectangle to the simulation.
     * @param caseNo The current case number.
     * @param i Component index.
     * @param size Normal distribution for size generation.
     * @param totalRectangles Pointer to count of total rectangles.
     * @param totalSlopedRectangles Pointer to count of total sloped rectangles.
     * @param totalEllipses Pointer to count of total ellipses.
     * @param totalCircles Pointer to count of total circles.
     * @param hoop Pointer to hoop distance.
     * @return The generated SlopedRectangle object.
     */
    SlopedRectangle addOneSlopedRectangle(int caseNo, int i, std::normal_distribution<double> size, int* totalRectangles, int* totalSlopedRectangles, int* totalEllipses, int* totalCircles, double* hoop);

    /**
     * @brief Adds a single ellipse to the simulation.
     * @param caseNo The current case number.
     * @param ingradient Component index.
     * @param size Normal distribution for size generation.
     * @param totalRectangles Pointer to count of total rectangles.
     * @param totalSlopedRectangles Pointer to count of total sloped rectangles.
     * @param totalEllipses Pointer to count of total ellipses.
     * @param totalCircles Pointer to count of total circles.
     * @param hoop Pointer to hoop distance.
     * @return The generated Ellipse object.
     */
    Ellipse addOneEllipse(int caseNo, int ingradient, std::normal_distribution<double> size, int* totalRectangles, int* totalSlopedRectangles, int* totalEllipses, int* totalCircles, double* hoop);

    /**
     * @brief Sets up sizes for different shape types based on component properties.
     * @param i Component index.
     * @param dimensionY Y-dimension.
     * @param dimensionX X-dimension.
     * @param factor Scaling factor.
     * @param size Normal distribution for size generation.
     * @param componentsSizeType Size type of the component.
     * @param rectWidth Pointer to rectangle width.
     * @param rectHeight Pointer to rectangle height.
     * @param componentsType Shape type of the component.
     * @param maxAngle Pointer to maximum angle.
     * @param minAngle Pointer to minimum angle.
     * @param totalSlopedRectangles Pointer to count of total sloped rectangles.
     * @param totalRectangles Pointer to count of total rectangles.
     * @param totalCircles Pointer to count of total circles.
     * @param totalEllipses Pointer to count of total ellipses.
     */
    void SetupSizes(int i, double dimensionY, double dimensionX, double factor, std::normal_distribution<double> size, SizeType componentsSizeType, double* rectWidth, double* rectHeight, ShapeType componentsType, double* maxAngle, double* minAngle, int* totalSlopedRectangles, int* totalRectangles, int* totalCircles, int* totalEllipses);

    /**
     * @brief Checks and draws an ellipse for the "swiss cheese" model.
     * @param ingradient Component index.
     * @param a Semi-major axis.
     * @param b Semi-minor axis.
     * @param cEllipse Pointer to the Ellipse object.
     * @param hoop Hoop distance.
     */
    void swissCheeseCheckAndDraw(int ingradient, double a, double b, Ellipse* cEllipse, double hoop);

    /**
     * @brief Checks and draws a sloped rectangle for the "swiss cheese" model.
     * @param ingradient Component index.
     * @param rectWidth Width of the rectangle.
     * @param rectHeight Height of the rectangle.
     * @param sRectangle Pointer to the SlopedRectangle object.
     * @param hoop Hoop distance.
     */
    void swissCheeseCheckAndDraw(int ingradient, double rectWidth, double rectHeight, SlopedRectangle* sRectangle, double hoop);

    /**
     * @brief Runs the Monte Carlo simulation.
     */
    void monteCarlo(void);

    /**
     * @brief Calculates the mean correlation length.
     */
    void calcMeanCorrellationLength();

    /**
     * @brief Saves simulation results to a report file.
     * @param ireportType The type of report to generate.
     */
    void saveResultstoReport(ReportType ireportType);

    /**
     * @brief Generates a standard report.
     */
    void Report();

    /**
     * @brief Generates a report with semicolon separators.
     */
    void ReportWithSemicolon();

    /**
     * @brief Generates a statistics report with a specified separator.
     * @param seperator The separator character/string.
     */
    void ReportStatistics(string seperator);

    /**
     * @brief Generates a standard statistics report.
     */
    void ReportStatistics(void);

    /**
     * @brief Generates a sloped rectangle object.
     * @param downleft The bottom-left corner of the bounding box.
     * @param upRight The top-right corner of the bounding box.
     * @param width Width of the rectangle.
     * @param height Height of the rectangle.
     * @param maxAngle Maximum rotation angle.
     * @param minAngle Minimum rotation angle.
     * @return The generated SlopedRectangle object.
     */
    SlopedRectangle generateSlopedRectangle(Point downleft, Point upRight, double width, double height, double maxAngle, double minAngle);

    /**
     * @brief Generates an ellipse object.
     * @param downleft The bottom-left corner of the bounding box.
     * @param upRight The top-right corner of the bounding box.
     * @param ia Semi-major axis.
     * @param ib Semi-minor axis.
     * @param maxAngle Maximum rotation angle.
     * @param minAngle Minimum rotation angle.
     * @return The generated Ellipse object.
     */
    Ellipse generateEllipse(Point downleft, Point upRight, double ia, double ib, double maxAngle, double minAngle);

    /**
     * @brief Digitizes a sloped rectangle onto the grid.
     * @param ingradient Component index.
     * @param sRect The SlopedRectangle object.
     * @param iGrid Pointer to the Grid object.
     * @param state The cell state to set for digitized pixels.
     */
    void digitizeSlopedRectangle(int ingradient, SlopedRectangle sRect, Grid* iGrid, CellState state);

    /**
     * @brief Digitizes a sloped rectangle onto the grid and calculates its real area.
     * @param ingradient Component index.
     * @param sRect The SlopedRectangle object.
     * @param iGrid Pointer to the Grid object.
     * @param realArea Pointer to store the calculated real area.
     * @param state The cell state to set for digitized pixels.
     */
    void digitizeSlopedRectangle(int ingradient, SlopedRectangle sRect, Grid* iGrid, double* realArea, CellState state);

    /**
     * @brief Digitizes a sloped rectangle onto the grid including its border, and calculates its real area.
     * @param ingradient Component index.
     * @param sRect The SlopedRectangle object.
     * @param iGrid Pointer to the Grid object.
     * @param realArea Pointer to store the calculated real area.
     * @param state The cell state to set for digitized pixels.
     */
    void digitizeSlopedRectangleWithBorder(int ingradient, SlopedRectangle sRect, Grid* iGrid, double* realArea, CellState state);

    /**
     * @brief Digitizes an ellipse onto the grid and calculates its real area.
     * @param ingradient Component index.
     * @param iEllipse The Ellipse object.
     * @param iGrid Pointer to the Grid object.
     * @param realArea Pointer to store the calculated real area.
     * @param state The cell state to set for digitized pixels.
     */
    void digitizeEllipse(int ingradient, Ellipse iEllipse, Grid* iGrid, double* realArea, CellState state);

    /**
     * @brief Digitizes an ellipse onto the grid.
     * @param ingradient Component index.
     * @param iEllipse The Ellipse object.
     * @param iGrid Pointer to the Grid object.
     * @param state The cell state to set for digitized pixels.
     */
    void digitizeEllipse(int ingradient, Ellipse iEllipse, Grid* iGrid, CellState state);

    /**
     * @brief Digitizes an ellipse onto the grid including its border, and calculates its real area.
     * @param iEllipse The Ellipse object.
     * @param iGrid Pointer to the Grid object.
     * @param realArea Pointer to store the calculated real area.
     * @param state The cell state to set for digitized pixels.
     */
    void digitizeEllipseWithBorder(Ellipse iEllipse, Grid* iGrid, double* realArea, CellState state);

    /**
     * @brief Digitizes a quadrant (division by 4) of an ellipse onto the grid and calculates its real area.
     * This might be used for optimization by only digitizing one quadrant and mirroring.
     * @param ingradient Component index.
     * @param iEllipse The Ellipse object.
     * @param iGrid Pointer to the Grid object.
     * @param realArea Pointer to store the calculated real area.
     * @param state The cell state to set for digitized pixels.
     * @param x Boolean indicating X-axis mirroring.
     * @param y Boolean indicating Y-axis mirroring.
     */
    void digitizeEllipseDiv4(int ingradient, Ellipse iEllipse, Grid* iGrid, double* realArea, CellState state, bool x, bool y);

    /**
     * @brief Reads simulation parameters and settings from an input file.
     * @param inputFileName The name of the input file.
     */
    void readFromFile(char* inputFileName);

    /**
     * @brief Exports simulation data in a format suitable for Finite Difference Method (FDM) analysis.
     * @param fileName The name of the export file.
     */
    void exportForFDM(char* fileName);
};