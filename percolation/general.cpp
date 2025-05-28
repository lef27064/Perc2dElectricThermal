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
1. E. Lambrou and L. N. Gergidis, “A computational method for calculating the electrical and thermal properties of random composite” ,
Physica A: Statistical Mechanics and its Applications, Volume 642, 2024, 129760, ISSN 0378-4371,
https://doi.org/10.1016/j.physa.2024.129760
2. E. Lambrou and L. N. Gergidis, “A particle digitization-based computational method for continuum percolation,” Physica A: Statistical Mechanics
and its Applications, vol. 590, p. 126738, 2022

if you use this programm and write a paper or report please cite above papers

*/

#include "general.h" // Include the custom header file "general.h" which likely contains declarations for shared functions and enums
#include <sys/stat.h> // Include for file system status (e.g., stat function), though not directly used in the provided code snippet
#include <chrono>     // For std::chrono, used for time-related operations (e.g., precise delays, getting current time)
#include <thread>     // For std::this_thread::sleep_for, used to pause execution
#include <string>     // For std::string class, used for string manipulation
#include <ctime>      // For C-style time functions like std::time_t and std::ctime
#include <algorithm>  // For standard algorithms like std::min_element and std::max_element
#include <numeric>    // For numeric operations, specifically std::accumulate for summing array elements
#include <limits>     // For std::numeric_limits, used to get properties of numeric types (e.g., NaN)

/// <summary>
/// Provides a delay for a specified number of seconds.
/// This function pauses the program's execution for the duration specified by `number_of_seconds`.
/// </summary>
/// <param name="number_of_seconds">The number of seconds to delay the program's execution.</param>
void delay(int number_of_seconds)
{
    // Using std::this_thread::sleep_for for more accurate and efficient delay
    // compared to a busy-wait loop with clock(). This puts the current thread
    // to sleep for the specified duration, freeing up CPU resources.
    std::this_thread::sleep_for(std::chrono::seconds(number_of_seconds));
}

/// <summary>
/// Returns the current date and time as a formatted string.
/// This function captures the current system time and converts it into a human-readable string.
/// </summary>
/// <returns>A `std::string` containing the current date and time. The trailing newline character
/// typically added by `std::ctime` is removed for a cleaner result.</returns>
std::string NowToString(void)
{
    // Get the current time point from the system clock.
    auto endt = std::chrono::system_clock::now();
    // Convert the time point to a C-style time_t object.
    std::time_t end_time = std::chrono::system_clock::to_time_t(endt);
    // Convert time_t to a string representation. std::ctime returns a string
    // that includes a newline character at the end.
    std::string s = std::ctime(&end_time);
    // Check if the string is not empty and if its last character is a newline.
    // If so, remove the newline character for a cleaner output string.
    if (!s.empty() && s.back() == '\n') {
        s.pop_back(); // Removes the '\n' character at the end of the string.
    }
    return s; // Return the formatted time string.
}

/// <summary>
/// Finds the minimum element in a given array within a specified range.
/// This function iterates through a portion of a double array to find its smallest value.
/// </summary>
/// <param name="values">The source array of `double` values to search within.</param>
/// <param name="first">The starting index of the range (inclusive). The search begins from this index.</param>
/// <param name="last">The ending index of the range (exclusive). The search ends just before this index.</param>
/// <returns>The minimum `double` value found within the specified range. If the range
/// is invalid (e.g., `first` is greater than or equal to `last`, or `first` is negative),
/// `std::numeric_limits<double>::quiet_NaN()` (Not-a-Number) is returned.</returns>
double minElement(double values[], int first, int last)
{
    // Using std::min_element for safer and more efficient searching.
    // Ensure the specified range is valid. A valid range requires 'first' to be less than 'last'
    // and 'first' to be a non-negative index.
    if (first >= last || first < 0) {
        // Return NaN (Not-a-Number) for an invalid range, indicating an error or an impossible operation.
        return std::numeric_limits<double>::quiet_NaN();
    }
    // `values + first` points to the start of the desired range, and `values + last` points
    // one element past the end of the desired range. `*` dereferences the iterator returned
    // by `std::min_element` to get the actual minimum value.
    return *std::min_element(values + first, values + last);
}

/// <summary>
/// Finds the maximum element in a given array within a specified range.
/// This function iterates through a portion of a double array to find its largest value.
/// </summary>
/// <param name="values">The source array of `double` values to search within.</param>
/// <param name="first">The starting index of the range (inclusive). The search begins from this index.</param>
/// <param name="last">The ending index of the range (exclusive). The search ends just before this index.</param>
/// <returns>The maximum `double` value found within the specified range. If the range
/// is invalid (e.g., `first` is greater than or equal to `last`, or `first` is negative),
/// `std::numeric_limits<double>::quiet_NaN()` (Not-a-Number) is returned.</returns>
double maxElement(double values[], int first, int intlast)
{
    // Using std::max_element for safer and more efficient searching.
    // Ensure the specified range is valid.
    if (first >= intlast || first < 0) {
        // Return NaN for an invalid range.
        return std::numeric_limits<double>::quiet_NaN();
    }
    // `values + first` points to the start of the desired range, and `values + last` points
    // one element past the end of the desired range.
    return *std::max_element(values + first, values + intlast);
}

/// <summary>
/// Calculates the average of elements in an array of doubles within a specified range.
/// This function sums the elements in a given part of the array and divides by the count of elements.
/// </summary>
/// <param name="values">The source array of `double` values.</param>
/// <param name="first">The starting index of the range (inclusive). The sum begins from this index.</param>
/// <param name="last">The ending index of the range (exclusive). The sum ends just before this index.</param>
/// <returns>The average of the `double` values within the specified range. If the range
/// is invalid (e.g., `first` is greater than or equal to `last`, or `first` is negative),
/// `std::numeric_limits<double>::quiet_NaN()` (Not-a-Number) is returned to prevent division by zero.</returns>
double average_element(double values[], int first, int last)
{
    // Ensure the specified range is valid to avoid division by zero or out-of-bounds access.
    if (first >= last || first < 0) {
        // Return NaN for an invalid range.
        return std::numeric_limits<double>::quiet_NaN();
    }
    // Using std::accumulate for a more concise and safe sum calculation.
    // It sums elements from `values + first` up to (but not including) `values + last`,
    // starting with an initial sum of 0.0.
    double sum = std::accumulate(values + first, values + last, 0.0);
    // Calculate the average by dividing the sum by the number of elements in the range.
    return sum / (last - first);
}