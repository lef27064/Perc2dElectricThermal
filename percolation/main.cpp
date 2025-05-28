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

#pragma comment(linker, "/STACK:600000000") // Reserves 600 MB for the stack
#pragma comment(linker, "/HEAP:4200000000") // Reserves 4.2 GB for the heap

#include <cstdlib>   // For general utilities like EXIT_SUCCESS, EXIT_FAILURE
#include <cstring>   // For string manipulation functions (though not directly used in main)
#include <iostream>  // For input/output operations (cout, cin)
#include <string>    // For string class
#include <time.h>    // For time-related functions (though chrono is preferred for precision)
#include <chrono>    // For high-resolution time utilities (e.g., NowToString might use this)
#include "Shapes.h"      // Custom header for geometric shapes
#include "Grid.h"        // Custom header for grid structure
#include "ShapeGenerator.h" // Custom header for generating and digitizing shapes
#include "general.h"     // Custom header for general utility functions (like NowToString, delay)
#include "BatchMonteCarlo.h" // Custom header for batch Monte Carlo simulation
#include "Settings.h"    // Custom header for program settings (e.g., `info` struct)


using namespace std; // Use the standard namespace

int main() {
    //setlocale(LC_NUMERIC,".OCP"); // ".OCP" if you want to use system settings "French_Canada.1252" - commented out

    // Change to local locale for correct number formatting in output
    std::locale cpploc{ "" }; // Get the user's default locale
    std::cout.imbue(cpploc);  // Imbue cout with the local locale

    // Display program information header
    cout << "------------------------------------------------------------------------------------------------------------------------------------\n";
    cout << info.program;  // Program title
    cout << info.version;  // Program version
    cout << info.date;     // Creation date
    cout << info.author;   // Author information
    cout << info.licence;  // License information
    cout << "------------------------------------------------------------------------------------------------------------------------------------\n";
    cout << "Start at " << NowToString() << "\n"; // Display current start time

    // Create a new instance of BatchMonteCarlo to manage simulations
    BatchMonteCarlo* batchMonteCarlo = new BatchMonteCarlo();

    // Attempt to get input files and run the simulation batch
    if (batchMonteCarlo->getInputFiles() != EXIT_FAILURE) // If input files are successfully processed
    {
        batchMonteCarlo->Run(); // Execute the Monte Carlo simulation batch
        //batchMonteCarlo->saveResults(); // Save results (commented out in the original code)
    }
    else // If there was an error processing input files
        return EXIT_FAILURE; // Exit with a failure code

    // Prompt user to press a key before exiting
    cout << "Press any key to exit...";
    //delay(5); // Optional delay (commented out)

    // Read a character from input to keep the console window open until user interaction
    char resp;
    cin.get(resp);

    return EXIT_SUCCESS; // Exit successfully
}