/*This file is part of Electric Percolation (eperc2d) program.

Created from Lefteris Lamprou lef27064@otenet.gr during PhD thesis

eperc2d is free software : you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free software Foundation, either version 3 of the License, or
(at your option) any later version.

eperc2d is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Foobar.If not, see < https://www.gnu.org/licenses/>.
*/

#include "general.h"
#include <sys/stat.h>


void delay(int number_of_seconds)
{
	// Converting time into milli_seconds
	int milli_seconds = 1000 * number_of_seconds;

	// Storing start time
	clock_t start_time = clock();

	// looping till required time is not achieved
	while (clock() < start_time + milli_seconds)
		;
}

/// <summary>
/// date now to string value
/// </summary>
/// <param name=""></param>
/// <returns>now (string)</returns>
std::string NowToString(void)
{
    auto endt = std::chrono::system_clock::now();
    std::time_t end_time = std::chrono::system_clock::to_time_t(endt);
    std::string s = std::ctime(&end_time);

    return s;
}

/// <summary>
/// min element of array 
/// </summary>
/// <param name="values">source array</param>
/// <param name="first">first element of the array</param>
/// <param name="last">last element of the array</param>
/// <returns>minimum value</returns>
double  minElement(double values[], int first, int last)
{
	if (first == last)
		return values[last - 1];

	int i = first;
	double result = values[i];
	for (i = first + 1; i < last; i++)
		if (values[i] < result)
			result = values[i];
	return result;
}


/// <summary>
/// max element of array 
/// </summary>
/// <param name="values">source array</param>
/// <param name="first">first element of the array</param>
/// <param name="last">last element of the array</param>
/// <returns>maximum value</returns>
double  maxElement(double values[], int first, int last)
{
	if (first == last)
		return values[last - 1];

	int i = first;
	double result = values[i];
	for (i = first + 1; i < last; i++)
		if (values[i] > result)
			result = values[i];
	return result;
}

/// <summary>
/// Get current working directory
/// </summary>
/// <param name=""></param>
/// <returns>Current working directory (std::filesystem::path)</returns>
std::filesystem::path GetCurrentWorkingDir(void)
{
	//c++17
	return std::filesystem::current_path();
	
	//before c++17 version not multiplatform
	/*
	
		char buff[FILENAME_MAX];
	    std::filesystem::current_path(buff, FILENAME_MAX);
		
		std::string current_working_dir(buff);
		return current_working_dir;*/
}

/// <summary>
/// Average Element of an array of doubles
/// </summary>
/// <param name="values">source array</param>
/// <param name="first">first element of the array</param>
/// <param name="last">last element of the array</param>
/// <returns>maximum value</returns>
double  average_element(double values[], int first, int last)
{
	if (first == last)
		return values[last - 1];

	double sum = 0;
	int i = first;
//	double result = values[i];
	for (i = first; i < last; i++)
		sum += values[i];
	return sum / (last - first);
}
