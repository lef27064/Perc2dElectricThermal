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

#include "Settings.h" // Includes the header for the Settings class, which this file implements.

#include "general.h"   // Assumed to contain declarations for `delay` and potentially other common utilities.
#include <sys/stat.h>  // System/stat.h might be included for file status checks, though not directly used in the provided logic for reading.
#include <string>      // For `std::string` manipulation.
#include <fstream>     // For file input operations, specifically `std::ifstream`.
#include <iostream>    // For input/output operations, like `std::cout` and `std::cerr`.
#include <stdexcept>   // For exception handling, specifically `std::invalid_argument` and `std::out_of_range` used with `std::stoi`.
#include <limits>      // For `std::numeric_limits`, though not directly used in this snippet's logic.
#include <chrono>      // For `std::chrono` functionalities, like `std::chrono::seconds`.
#include <thread>      // For `std::this_thread::sleep_for`.
#include <map>         // For `std::map`, although not used in the final version of the `readFromFile` method, it was likely considered.
#include <algorithm>   // For `std::transform` and `std::tolower`, used in the `toLower` helper function.


// Helper function: toLower
// Converts a given string to lowercase. This is useful for case-insensitive comparisons
// when reading settings from a configuration file (e.g., "TRUE" vs "true").
std::string toLower(std::string s) {
    // `std::transform` applies a given operation to each element in a range.
    // Here, it iterates through the string `s` and converts each character to its lowercase equivalent.
    std::transform(s.begin(), s.end(), s.begin(),
        [](unsigned char c) { return std::tolower(c); });
    return s; // Returns the lowercase version of the string.
}

// Settings::readFromFile
// Reads configuration settings from a specified file and populates the member variables
// of the `Settings` class. It handles comments, empty lines, and basic error checking.
void Settings::readFromFile(char* fileName)
{
    std::string line;               // A temporary string to store each line read from the file.
    std::ifstream inputFile(fileName); // Opens the specified file for reading.

    // Check if the file was successfully opened.
    if (inputFile.is_open())
    {
        // Helper lambda: readNextValidLine
        // This lambda function reads lines from the input file, skipping any empty lines
        // or lines that start with '#' (which are treated as comments).
        // It returns `true` if a valid, non-comment line is read into `currentLine`,
        // and `false` if the end of the file is reached or a read error occurs.
        auto readNextValidLine = [&](std::ifstream& file, std::string& currentLine) {
            while (std::getline(file, currentLine)) {
                // Check if the line is empty or starts with '#' (comment).
                // If it is, skip to the next line.
                if (currentLine.empty() || currentLine[0] == '#') {
                    continue; // Skip empty lines or comment lines.
                }
                return true; // A valid, non-comment line was found.
            }
            return false; // End of file or read error.
            };

        // Helper lambda: readBoolSetting
        // This lambda reads a boolean setting from the file. It expects the value
        // to be "true" or "false" (case-insensitive).
        // If an unexpected end of file is encountered, it prints an error and exits.
        auto readBoolSetting = [&](bool& settingVar, const char* settingName) {
            if (!readNextValidLine(inputFile, line)) {
                std::cerr << "Error: Unexpected end of file while reading '" << settingName << "' setting.\n";
                inputFile.close(); // Close the file before exiting.
                exit(-1);          // Terminate the program with an error code.
            }
            // Convert the read line to lowercase and compare it to "true" to set the boolean variable.
            settingVar = (toLower(line) == "true");
            };

        // Helper lambda: readStringSetting
        // This lambda reads a string setting directly from the file.
        // Similar to `readBoolSetting`, it handles unexpected end of file scenarios.
        auto readStringSetting = [&](std::string& settingVar, const char* settingName) {
            if (!readNextValidLine(inputFile, line)) {
                std::cerr << "Error: Unexpected end of file while reading '" << settingName << "' setting.\n";
                inputFile.close();
                exit(-1);
            }
            settingVar = line; // Directly assign the read line to the setting variable.
            };

        // --- Read 'saveImageFile' setting ---
        // Determines whether to save image files.
        readBoolSetting(saveImageFile, "saveImageFile");

        // --- Read 'RandomSaveImageFile' setting ---
        // Determines if image saving should be random.
        readBoolSetting(RandomSaveImageFile, "RandomSaveImageFile");

        // --- Read 'totalImagesToSave' setting ---
        // Determines the total number of images to save if random saving is enabled.
        if (!readNextValidLine(inputFile, line)) {
            std::cerr << "Error: Unexpected end of file while reading 'totalImagesToSave' setting.\n";
            inputFile.close();
            exit(-1);
        }
        try {
            // Attempt to convert the read line (string) to an integer.
            totalImagesToSave = std::stoi(line);
        }
        catch (const std::invalid_argument& e) {
            // Catch error if the string is not a valid integer.
            std::cerr << "Error: Invalid argument for totalImagesToSave: '" << line << "'. " << e.what() << "\n";
            inputFile.close();
            exit(-1);
        }
        catch (const std::out_of_range& e) {
            // Catch error if the integer value is out of the representable range for `int`.
            std::cerr << "Error: totalImagesToSave value out of range: '" << line << "'. " << e.what() << "\n";
            inputFile.close();
            exit(-1);
        }

        // --- Read 'saveShapes' setting ---
        // Determines whether to save generated shapes.
        readBoolSetting(saveShapes, "saveShapes");

        // --- Read 'saveAsBmpImage' setting ---
        // Determines if images should be saved in BMP format. It reads a string
        // and checks if it's "bmp" (case-insensitive).
        readStringSetting(line, "saveAsBmpImage_format"); // Reads the format string into 'line'.
        saveAsBmpImage = (toLower(line) == "bmp");         // Sets `saveAsBmpImage` based on the format.

        // --- Read 'isLattice' setting ---
        // Determines if the simulation uses a lattice structure.
        readBoolSetting(isLattice, "isLattice");

        inputFile.close(); // Always close the file after reading.
    }
    else // If the file could not be opened.
    {
        std::cout << "\n*************************************************************************************\n";
        std::cout << "Error: Cannot open or find file: ";
        // Output the file name that could not be opened.
        std::cout << std::string(fileName) << ". Program halted...\n";
        std::cout << "*************************************************************************************\n";
        // Introduce a short delay before exiting, allowing the user to see the error message.
        delay(4);
        exit(-1); // Terminate the program with an error code.
    }
}