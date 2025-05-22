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
2. E. Lambrou and L. N. Gergidis, “A particle digitization-based computational method for continuum percolation,” Physica A: Statistical Mechanics
and its Applications, vol. 590, p. 126738, 2022

if you use this programm and write a paper or report please cite above papers

*/

#include "general.h"
#include <sys/stat.h>
#include <chrono>   // For std::chrono
#include <thread>   // For std::this_thread::sleep_for
#include <string>   // For std::string
#include <ctime>    // For std::time_t, std::ctime
#include <algorithm> // For std::min_element, std::max_element
#include <numeric>   // For std::accumulate
#include <limits>   // For std::numeric_limits

/// <summary>
/// Provides a delay for a specified number of seconds.
/// </summary>
/// <param name="number_of_seconds">The number of seconds to delay.</param>
void delay(int number_of_seconds)
{
    // Using std::this_thread::sleep_for for more accurate and efficient delay
    // compared to a busy-wait loop with clock().
    std::this_thread::sleep_for(std::chrono::seconds(number_of_seconds));
}

/// <summary>
/// Returns the current date and time as a string.
/// </summary>
/// <returns>The current date and time (string).</returns>
std::string NowToString(void)
{
    auto endt = std::chrono::system_clock::now();
    std::time_t end_time = std::chrono::system_clock::to_time_t(endt);
    // std::ctime returns a string ending with a newline character.
    // We remove the newline for a cleaner result.
    std::string s = std::ctime(&end_time);
    if (!s.empty() && s.back() == '\n') { // Check to ensure string is not empty before popping
        s.pop_back(); // Removes the '\n' character at the end
    }
    return s;
}

/// <summary>
/// Finds the minimum element in an array within a specified range.
/// </summary>
/// <param name="values">The source array.</param>
/// <param name="first">The starting index of the range (inclusive).</param>
/// <param name="last">The ending index of the range (exclusive).</param>
/// <returns>The minimum value within the specified range, or NaN if the range is invalid.</returns>
double minElement(double values[], int first, int last)
{
    // Using std::min_element for safer and more efficient searching.
    // Ensure the range is valid.
    if (first >= last || first < 0) {
        // Return NaN (Not-a-Number) for an invalid range.
        return std::numeric_limits<double>::quiet_NaN();
    }
    return *std::min_element(values + first, values + last);
}

/// <summary>
/// Finds the maximum element in an array within a specified range.
/// </summary>
/// <param name="values">The source array.</param>
/// <param name="first">The starting index of the range (inclusive).</param>
/// <param name="last">The ending index of the range (exclusive).</param>
/// <returns>The maximum value within the specified range, or NaN if the range is invalid.</returns>
double maxElement(double values[], int first, int last)
{
    // Using std::max_element for safer and more efficient searching.
    // Ensure the range is valid.
    if (first >= last || first < 0) {
        return std::numeric_limits<double>::quiet_NaN();
    }
    return *std::max_element(values + first, values + last);
}

/// <summary>
/// Calculates the average of elements in an array of doubles within a specified range.
/// </summary>
/// <param name="values">The source array.</param>
/// <param name="first">The starting index of the range (inclusive).</param>
/// <param name="last">The ending index of the range (exclusive).</param>
/// <returns>The average of the values within the specified range, or NaN if the range is invalid.</returns>
double average_element(double values[], int first, int last)
{
    // Ensure the range is valid to avoid division by zero.
    if (first >= last || first < 0) {
        return std::numeric_limits<double>::quiet_NaN();
    }
    // Using std::accumulate for a more concise and safe sum calculation.
    double sum = std::accumulate(values + first, values + last, 0.0);
    return sum / (last - first);
}