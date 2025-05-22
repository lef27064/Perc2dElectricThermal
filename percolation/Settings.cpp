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

#include "Settings.h"

#include "general.h" // Assuming this contains necessary definitions for Settings class members
#include <sys/stat.h> // Potentially for file status, though not directly used in the read logic
#include <string>     // For std::string
#include <fstream>    // For std::ifstream
#include <iostream>   // For std::cout, std::cerr
#include <stdexcept>  // For std::stoi potential exceptions
#include <limits>     // For std::numeric_limits
#include <chrono>     // For std::chrono
#include <thread>     // For std::this_thread::sleep_for
#include <map>        // For std::map to potentially store settings (not used here to preserve original structure)
#include <algorithm>  // For std::transform and std::tolower


// Helper function to convert a string to lowercase for case-insensitive comparisons
std::string toLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
        [](unsigned char c) { return std::tolower(c); });
    return s;
}

void Settings::readFromFile(char* fileName)
{
    std::string line;
    std::ifstream inputFile(fileName);

    if (inputFile.is_open())
    {
        // Helper lambda to read a line and skip comments/empty lines.
        // Returns false if EOF is reached or a read error occurs.
        auto readNextValidLine = [&](std::ifstream& file, std::string& currentLine) {
            while (std::getline(file, currentLine)) {
                // Trim leading/trailing whitespace (optional, but good practice for config files)
                // For simplicity, not adding full trim here to minimize changes to core logic.
                // If currentLine[0] is used, make sure string is not empty first.
                if (currentLine.empty() || currentLine[0] == '#') {
                    continue; // Skip empty lines or lines starting with '#' (comments)
                }
                return true; // Found a valid, non-comment line
            }
            return false; // End of file or read error
            };

        // Helper lambda to read a boolean setting
        auto readBoolSetting = [&](bool& settingVar, const char* settingName) {
            if (!readNextValidLine(inputFile, line)) {
                std::cerr << "Error: Unexpected end of file while reading '" << settingName << "' setting.\n";
                inputFile.close();
                exit(-1);
            }
            settingVar = (toLower(line) == "true"); // Case-insensitive comparison
            };

        // Helper lambda to read a string setting (e.g., for "BMP" vs others)
        auto readStringSetting = [&](std::string& settingVar, const char* settingName) {
            if (!readNextValidLine(inputFile, line)) {
                std::cerr << "Error: Unexpected end of file while reading '" << settingName << "' setting.\n";
                inputFile.close();
                exit(-1);
            }
            settingVar = line; // Directly assign the read line
            };

        // --- Read save image file setting ---
        readBoolSetting(saveImageFile, "saveImageFile");

        // --- Read random save image file setting ---
        readBoolSetting(RandomSaveImageFile, "RandomSaveImageFile");

        // --- Read total images to save setting ---
        if (!readNextValidLine(inputFile, line)) {
            std::cerr << "Error: Unexpected end of file while reading 'totalImagesToSave' setting.\n";
            inputFile.close();
            exit(-1);
        }
        try {
            totalImagesToSave = std::stoi(line);
        }
        catch (const std::invalid_argument& e) {
            std::cerr << "Error: Invalid argument for totalImagesToSave: '" << line << "'. " << e.what() << "\n";
            inputFile.close();
            exit(-1);
        }
        catch (const std::out_of_range& e) {
            std::cerr << "Error: totalImagesToSave value out of range: '" << line << "'. " << e.what() << "\n";
            inputFile.close();
            exit(-1);
        }

        // --- Read save shapes setting ---
        readBoolSetting(saveShapes, "saveShapes");

        // --- Read save as BMP setting ---
        // We need the original line here to check for "BMP"/"bmp"
        readStringSetting(line, "saveAsBmpImage_format"); // Read into 'line'
        saveAsBmpImage = (toLower(line) == "bmp");

        // --- Read is Lattice shapes setting ---
        readBoolSetting(isLattice, "isLattice");

        inputFile.close(); // Close the file explicitly
    }
    else // File could not be opened
    {
        std::cout << "\n*************************************************************************************\n";
        std::cout << "Error: Cannot open or find file: ";
        // Using std::string(fileName) for safer output and potential string operations
        std::cout << std::string(fileName) << ". Program halted...\n";
        std::cout << "*************************************************************************************\n";
        // Some delay is better than waiting for user response
        delay(4);
        exit(-1);
    }
}