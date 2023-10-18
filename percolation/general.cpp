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



std::string NowToString(void)
{
    auto endt = std::chrono::system_clock::now();
    std::time_t end_time = std::chrono::system_clock::to_time_t(endt);
    std::string s = std::ctime(&end_time);

    return s;
}


bool dirExists(char* pathname)
{
	struct stat statInfo;
	bool result;
	if (stat(pathname, &statInfo) != 0)
		result = false;
	else if (statInfo.st_mode & S_IFDIR)  // S_ISDIR() doesn't exist on my windows
		result = true;
	else
		result = false;
	return result;

}


double  min_element(double values[], int first, int last)
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


double  max_element(double values[], int first, int last)
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

std::filesystem::path GetCurrentWorkingDir(void)
{
	//c++17
	return std::filesystem::current_path();
/*
		char buff[FILENAME_MAX];
	    std::filesystem::current_path(buff, FILENAME_MAX);
		
		std::string current_working_dir(buff);
		return current_working_dir;*/
}




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

/*
string ExePath()
{
	char buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);
	string::size_type pos = string(buffer).find_last_of("\\/");
	return string(buffer).substr(0, pos);
}*/
