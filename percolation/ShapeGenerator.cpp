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

#include "ShapeGenerator.h"

// Default constructor for ShapeGenerator
ShapeGenerator::ShapeGenerator()
{
	// Initialize member variables
	totalComponents = 0;
	iterations = 400;
	pixelsPerMinimumSize = 10;
	settings = NULL; // Pointer to settings object, initialized to null
	grid = NULL;     // Pointer to grid object, initialized to null
}

// Initializes project directories for shapes and images
void ShapeGenerator::initDirs(void)
{
	// C++17 filesystem operations for directory creation
	string iprojectDir = "./" + projectName; // Define project directory path
	string ishapesDir = iprojectDir + "/shapes/"; // Define shapes directory path
	string iimagesDir = iprojectDir + "/images/"; // Define images directory path

	// Check if project directory exists, if not, create it
	if (!std::filesystem::exists(iprojectDir))
		std::filesystem::create_directory(iprojectDir);

	// Check if shapes directory exists, if not, create it
	if (!std::filesystem::exists(ishapesDir))
		std::filesystem::create_directory(ishapesDir);

	// Check if images directory exists, if not, create it
	if (!std::filesystem::exists(iimagesDir))
		std::filesystem::create_directory(iimagesDir);
}

// Constructor for ShapeGenerator with parameters
ShapeGenerator::ShapeGenerator(int tcomp, double comp[], ShapeType compType[], SizeType compSizeType[], double sWeights[], double dimX[], double dimY[], double ihoops[], Grid* iGrid, Settings* isettings) : totalComponents(tcomp), grid(iGrid), settings(isettings)
{
	// Loop through components and assign properties
	for (int i = 0; i < totalComponents; i++)
	{
		components[i] = comp[i];             // Component area/weight
		componentsType[i] = compType[i];     // Type of shape for the component
		specialWeights[i] = sWeights[i];     // Special weights for component
		dimensionX[i] = dimX[i];             // X dimension for the component
		dimensionY[i] = dimY[i];             // Y dimension for the component
		componentsArea[i] = comp[i];         // Initial component area
		hoops[i] = ihoops[i];                // Hoop size for the component
		componentsSizeType[i] = compSizeType[i]; // Size type (fixed/variable)
	}
}

// Solves for component areas based on given weights
void ShapeGenerator::areaSolve(void)
{
	double sum = 0.0;        // Sum of (component_area * special_weight)
	double sumError = 0.0;   // Sum of absolute errors
	double epsilon = 1.e-10; // Tolerance for convergence
	double sumAreas = 0.0;   // Sum of calculated component areas

	int iter = 0; // Iteration counter

	// Initialize component areas
	for (int i = 0; i < totalComponents; i++)
		componentsArea[i] = components[i]; // Start with initial component values

	do
	{
		iter++; // Increment iteration counter
		sum = 0; // Reset sum for each iteration

		// Compute ÓAi*ñi (sum of component area times special weight)
		for (int i = 0; i < totalComponents; i++)
		{
			sum = sum + components[i] * specialWeights[i];
		}

		// Calculate current Wi = (Ai*ñi) / ÓAj*ñj and compute errors
		sumError = 0; // Reset sumError for each iteration
		for (int i = 0; i < totalComponents; i++)
		{
			calcComponents[i] = (componentsArea[i] * specialWeights[i]) / sum; // Calculate normalized weight
			sumError = sumError + abs(calcComponents[i] - components[i]);    // Accumulate absolute error
			// debug cout <<"COMPONENT["<<i<<"] Area="<< componentsArea[i] <<" Error="<< calcComponents[i] - components[i] <<"\n";
		}

		sumAreas = 0; // Reset sumAreas for each iteration
		// Find Ai = W * ÓAj*ñj / ñi (re-calculate component areas)
		for (int i = 0; i < totalComponents; i++)
		{
			componentsArea[i] = components[i] * sum / specialWeights[i]; // Adjust component area
			sumAreas += componentsArea[i]; // Accumulate total adjusted area
		}

	} while (sumError > epsilon); // Continue until error is below epsilon

	cout << "Solve from % Weights to % Area\n";
	// Print some information about the solution
	cout << "Component            %Area          %Weight        Estimation Error \n";
	for (int i = 0; i < totalComponents; i++)
	{
		componentsArea[i] = componentsArea[i] / sumAreas; // Normalize component areas
		cout << setw(13) << i << setw(13) << componentsArea[i] << setw(13) << components[i] << setw(25) << calcComponents[i] - components[i] << "\n";
	}
	cout << "Total error e=" << sumError << " - Total iterations= " << iter << "\n";
	cout << "--------------------------------------------------------------------------------------------------------------\n";
}

// Checks if a Point (double coordinates) is inside the grid
bool ShapeGenerator::isInsideGrid(Point cPoint, Grid* iGrid)
{
	// Check if x and y coordinates are within grid boundaries
	if ((cPoint.x >= 0 && cPoint.x < iGrid->width) && (cPoint.y >= 0 && cPoint.y < iGrid->height))
		return true; // Point is inside
	else
		return false; // Point is outside
}

// Checks if an iPoint (integer coordinates) is inside the grid
// to do: may be needs template
bool ShapeGenerator::isInsideGrid(iPoint cPoint, Grid* iGrid)
{
	// Check if x and y coordinates are within grid boundaries
	if ((cPoint.x >= 0 && cPoint.x < iGrid->width) && (cPoint.y >= 0 && cPoint.y < iGrid->height))
		return true; // Point is inside
	else
		return false; // Point is outside
}

// Scales a point's coordinates
void ShapeGenerator::scale(Point orig, Point* scaled, double scaleX, double scaleY)
{
	scaled->x = orig.x * scaleX; // Scale x-coordinate
	scaled->y = orig.y * scaleY; // Scale y-coordinate
}

// This function is currently implemented only for binary media.
// It will be fixed for multi-phase (3) and above media in the future.
void ShapeGenerator::setupCaseLattice(int caseNo, double* setUpTime)
{
	clock_t start = clock(); // Start timer
	int i, j;
	double what; // Random value
	size_t totalSites = 0; // Count of 'HARD' sites

	std::random_device rd;   // Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd());  // Standard mersenne_twister_engine seeded with rd()
	std::uniform_real_distribution<> dis(0.000, 1.00000); // Uniform distribution for random numbers

	thresh = componentsArea[1]; // Threshold based on the second component's area
	for (i = 0; i < width; i++)
	{
#pragma omp parallel for schedule(dynamic) // OpenMP directive for parallel processing
		for (j = 0; j < height; j++)
		{
			what = dis(gen); // Generate a random number
			if (what < thresh)
			{
				grid->set(i, j, CellState::HARD); // Set cell state to HARD
				grid->set(grid->ingadients, i, j, 1); // Set gradient to 1
				totalSites++; // Increment HARD site count
			}
			else
			{
				grid->set(i, j, CellState::EMPTY); // Set cell state to EMPTY
				grid->set(grid->ingadients, i, j, 0); // Set gradient to 0
			}
		}
	}

	clock_t end = clock(); // End timer
	*setUpTime = ((double)(end - start)) / CLOCKS_PER_SEC; // Calculate setup time

	// Calculate real component areas based on total sites
	realComponentAreas[caseNo * 2 + 1] = 1.0 * totalSites / (width * height);
	realComponentAreas[caseNo * 2] = 1.0 - realComponentAreas[caseNo * 2 + 1];
	cout << "% Real component Area[0]=" << realComponentAreas[caseNo * 2] << "\n";
}

// Prints particle information for a given case
void ShapeGenerator::printParticles(int caseNo, vector<int> totalEllipsesPerComponent, vector<int> totalCirclesPerComponent, vector<int> totalRectanglesPerComponent, vector<int> totalSlopedRectanglesPerComponent)
{
	double matrixRealArea = 1.0; // Real area of the matrix component
	int whereMatrix = 0; // Index of the matrix component

	cout << "Component  |     Particle Type      | Total Praticles | Real Area in Lattice % | Goal Area % |  Error %    \n";
	for (int i = 0; i < totalComponents; i++)
	{
		if (componentsType[i] != ShapeType::NOTHING) // If it's a particle component
		{
			matrixRealArea = matrixRealArea - realComponentAreas[caseNo * totalComponents + i]; // Subtract particle area from matrix area
			cout << setw(10) << i; // Print component index

			// Print particle type and total particles based on ShapeType
			switch (componentsType[i]) {
			case ShapeType::SLOPEDRECTANGLE:
				cout << setw(21) << "Sloped Rectangles" << setw(12) << totalSlopedRectanglesPerComponent[i];
				break;
			case ShapeType::RECTANGLE:
				cout << setw(21) << "Rectangles" << setw(12) << totalRectanglesPerComponent[i];
				break;
			case ShapeType::ELLIPSE:
				cout << setw(21) << "Ellipses" << setw(12) << totalEllipsesPerComponent[i];
				break;
			case ShapeType::CIRCLE:
				cout << setw(21) << "Circles" << setw(12) << totalCirclesPerComponent[i];
				break;
			}

			// Print real area, goal area, and error
			cout << setw(25) << realComponentAreas[caseNo * totalComponents + i] << setw(22) << componentsArea[i] << setw(17) << componentsArea[i] - realComponentAreas[caseNo * totalComponents + i] << "\n";
		}
		else
			whereMatrix = i; // Store index of the matrix component
	}

	// Print information for the matrix component
	cout << setw(10) << whereMatrix << setw(21) << "Matrix" << setw(12) << "N/A" << setw(25) << matrixRealArea << setw(22) << componentsArea[whereMatrix] << setw(17) << componentsArea[whereMatrix] - matrixRealArea << "\n";
}

// Sets up the simulation case, generating shapes and populating the grid
void ShapeGenerator::setupCase(int caseNo, double* setUpTime)
{
	// Vectors to store counts of different shape types per component
	vector<int> totalEllipsesPerComponent;
	vector<int> totalCirclesPerComponent;
	vector<int> totalRectanglesPerComponent;
	vector<int> totalSlopedRectanglesPerComponent;

	// Local variables to count total shapes of each type
	int totalEllipses;
	int totalCircles;
	int totalRectangles;
	int totalSlopedRectangles;

	// Initialize base shape objects (used for temporary calculations)
	Ellipse cEllipse(Point(0, 0), 0, 0, 0);
	iPoint iCircle(0, 0); // Note: iCircle is not used
	Rectangle cRectangle(Point(0, 0), Point(0, 0)); // Note: cRectangle is not used directly
	SlopedRectangle sRectangle(Point(0, 0), 0, 0, 0);

	clock_t start = clock(); // Start timer for setup time

	double realComponentsArea = 0; // Accumulator for real area of all components

	for (int i = 0; i < totalComponents; i++)
	{
		ofstream componentFile; // Output file stream for component data
		string Base = "_shapes.txt"; // Base filename extension
		string FileName = projectName + "/shapes/case_" + to_string(caseNo) + "_component_" + to_string(i) + Base; // Full filename

		// Open report file if saving shapes and it's a particle component
		if ((settings->saveShapes) && (componentsType[i] != ShapeType::NOTHING))
			componentFile.open(FileName);

		// Initialize shape counters for the current component
		totalEllipses = 0;
		totalCircles = 0;
		totalRectangles = 0;
		totalSlopedRectangles = 0;

		// Initialize random engine for size distribution (normal distribution)
		std::normal_distribution<double> size(dimensionX[i] * factor, 0.25 * dimensionX[i] * factor); // Mean and standard deviation

		// If it's a particle component (not NOTHING)
		if (componentsType[i] != ShapeType::NOTHING)
		{
			// Calculate maximum allowed area for the current component in grid pixels
			double maxComp = componentsArea[i] * grid->width * grid->height;

			// Print headers to the component file if saving shapes
			if (settings->saveShapes)
			{
				componentFile << "Component " << i << " maximum area=" << componentsArea[i] << " [on " << grid->width << "x" << grid->height << "]\n";
				switch (componentsType[i]) {
				case ShapeType::SLOPEDRECTANGLE:
					componentFile << "Component type: Sloped rectangle\n";
					break;
				case ShapeType::RECTANGLE:
					componentFile << "Component type: Rectangle\n";
					break;
				case ShapeType::ELLIPSE:
					componentFile << "Component type: Ellipse\n";
					break;
				case ShapeType::CIRCLE:
					componentFile << "Component type: Circle\n";
					break;
				}
				componentFile << "---------------------------------------------------------------------------------------\n";
				componentFile << "--------------|------- Center-----|----- dimensions---------|----------------------\n";
				componentFile << "component| id|   X        |     Y    |  Width/a     | height/b |   hoop   |   Slope    \n";
				componentFile << "---------------------------------------------------------------------------------------\n";
			}

			// Initialize total real area accumulated for this component
			realComponentAreas[caseNo * totalComponents + i] = 0;

			// Loop until the real area of the component reaches its maximum allowed area
			do
			{
				// If the shape is a rectangle or sloped rectangle
				if ((componentsType[i] == ShapeType::SLOPEDRECTANGLE) || (componentsType[i] == ShapeType::RECTANGLE))
				{
					double hoop; // Hoop distance for the shape
					// Add one sloped rectangle to the grid
					sRectangle = addOneSlopedRectangle(caseNo, i, size, &totalRectangles, &totalSlopedRectangles, &totalEllipses, &totalCircles, &hoop);

					// Save information to file if required
					if (settings->saveShapes)
						if (totalRectangles > 0) // If a regular rectangle was added
							componentFile << setw(11) << i << setw(1) << "," << setw(5) << totalRectangles << setw(1) << "," << setw(11) << sRectangle.center.x << setw(1) << "," << setw(11) << sRectangle.center.y << setw(1) << "," << setw(11) << sRectangle.width << setw(1) << "," << setw(11) << sRectangle.height << setw(1) << "," << setw(11) << hoop << setw(4) << "," << setw(11) << sRectangle.slope << "\n";
					if (totalSlopedRectangles > 0) // If a sloped rectangle was added
						componentFile << setw(11) << i << setw(1) << "," << setw(5) << totalSlopedRectangles << setw(1) << "," << setw(11) << sRectangle.center.x << setw(1) << "," << setw(11) << sRectangle.center.y << setw(1) << "," << setw(11) << sRectangle.width << setw(1) << "," << setw(11) << sRectangle.height << setw(1) << "," << setw(11) << hoop << setw(4) << "," << setw(11) << sRectangle.slope << "\n";
				}

				// If the shape is a circle or ellipse
				if ((componentsType[i] == ShapeType::CIRCLE) || (componentsType[i] == ShapeType::ELLIPSE))
				{
					double hoop; // Hoop distance for the shape

					// Add one ellipse to the grid
					cEllipse = addOneEllipse(caseNo, i, size, &totalRectangles, &totalSlopedRectangles, &totalEllipses, &totalCircles, &hoop);

					// Save information to file if required
					if (settings->saveShapes)
						if (totalCircles > 0) // If a circle was added
							componentFile << setw(11) << i << setw(1) << "," << setw(5) << totalCircles << setw(1) << "," << setw(11) << cEllipse.center.x << setw(1) << "," << setw(11) << cEllipse.center.y << setw(1) << "," << setw(11) << cEllipse.a << setw(1) << "," << setw(11) << cEllipse.b << setw(1) << "," << setw(11) << hoop << setw(4) << "," << setw(11) << cEllipse.slope << "\n";
					if (totalEllipses > 0) // If an ellipse was added
						componentFile << setw(11) << i << setw(1) << "," << setw(5) << totalEllipses << setw(1) << "," << setw(11) << cEllipse.center.x << setw(1) << "," << setw(11) << cEllipse.center.y << setw(1) << "," << setw(11) << cEllipse.a << setw(1) << "," << setw(11) << cEllipse.b << setw(1) << "," << setw(11) << hoop << setw(4) << "," << setw(11) << cEllipse.slope << "\n";
				}
			} while (realComponentAreas[caseNo * totalComponents + i] < maxComp); // Continue until target area is met

			if (swissCheese)
			{
				grid->inverse(); // Invert the grid for "Swiss Cheese" effect
				cout << "Inverting array for solving Swiss Cheese problem...\n";
			}

			// Normalize the real component area
			realComponentAreas[caseNo * totalComponents + i] = realComponentAreas[caseNo * totalComponents + i] / ((grid->width) * grid->height);

			// Store the total count of each shape type for the current component
			switch (componentsType[i])
			{
			case ShapeType::SLOPEDRECTANGLE:
				totalCirclesPerComponent.push_back(0);
				totalEllipsesPerComponent.push_back(0);
				totalRectanglesPerComponent.push_back(0);
				totalSlopedRectanglesPerComponent.push_back(totalSlopedRectangles);
				break;
			case ShapeType::RECTANGLE:
				totalCirclesPerComponent.push_back(0);
				totalEllipsesPerComponent.push_back(0);
				totalRectanglesPerComponent.push_back(totalRectangles);
				totalSlopedRectanglesPerComponent.push_back(0);
				break;
			case ShapeType::ELLIPSE:
				totalCirclesPerComponent.push_back(0);
				totalEllipsesPerComponent.push_back(totalEllipses);
				totalRectanglesPerComponent.push_back(0);
				totalSlopedRectanglesPerComponent.push_back(0);
				break;
			case ShapeType::CIRCLE:
				totalCirclesPerComponent.push_back(totalCircles);
				totalEllipsesPerComponent.push_back(0);
				totalRectanglesPerComponent.push_back(0);
				totalSlopedRectanglesPerComponent.push_back(0);
				break;
			}

			// Print summary to file if saving shapes
			if (settings->saveShapes)
			{
				componentFile << "------------------------------------------------------------------------------\n";
				componentFile << "Component[" << i << "]";

				switch (componentsType[i]) {
				case ShapeType::SLOPEDRECTANGLE:
					componentFile << " Total Sloped Parallelepiped = " << totalSlopedRectangles;
					break;
				case ShapeType::RECTANGLE:
					componentFile << " Total total Parallelepiped = " << totalRectangles;
					break;
				case ShapeType::ELLIPSE:
					componentFile << " Total Ellipsoids = " << totalEllipses;
					break;
				case ShapeType::CIRCLE:
					componentFile << " Total Spheres = " << totalCircles;
					break;
				}
				componentFile << "\n";
				componentFile << "Component[" << i << "] Real area in [" << grid->width << "x" << grid->height << "]= " << realComponentAreas[caseNo * totalComponents + i] << "%=\n";
				componentFile << "------------------------------------------------------------------------------\n";
			}
			realComponentsArea = realComponentsArea + realComponentAreas[caseNo * totalComponents + i]; // Accumulate real area
			componentFile.close(); // Close the component file
		}
		else // If it's a "NOTHING" component (matrix)
		{
			// Push back zeros for all shape types as no particles are generated
			totalSlopedRectanglesPerComponent.push_back(0);
			totalCirclesPerComponent.push_back(0);
			totalEllipsesPerComponent.push_back(0);
			totalRectanglesPerComponent.push_back(0);
		}
	}

	clock_t end = clock(); // End timer
	*setUpTime = ((double)(end - start)) / CLOCKS_PER_SEC; // Calculate setup time

	// Calculate the real area of the matrix component
	realComponentAreas[caseNo * totalComponents] = 1.0 - realComponentsArea;
	// Print particle summary for the case
	printParticles(caseNo, totalEllipsesPerComponent, totalCirclesPerComponent, totalRectanglesPerComponent, totalSlopedRectanglesPerComponent);
}

// Adds one sloped rectangle to the grid
SlopedRectangle ShapeGenerator::addOneSlopedRectangle(int caseNo, int ingradient, std::normal_distribution<double> size, int* totalRectangles, int* totalSlopedRectangles, int* totalEllipses, int* totalCircles, double* hoop)
{
	iPoint Center(0, 0); // Integer center coordinates
	iPoint dims(0, 0);   // Integer dimensions
	Point dCenter(0, 0); // Double center coordinates (not used)
	double maxAngle = 0; // Maximum rotation angle
	double minAngle = 0; // Minimum rotation angle

	double rectWidth;  // Width of the rectangle
	double rectHeight; // Height of the rectangle

	SlopedRectangle sRectangle(Point(0, 0), 0, 0, 0); // Temporary SlopedRectangle object

	// Setup sizes and angles for the rectangle based on component properties
	SetupSizes(ingradient, dimensionY[ingradient], dimensionX[ingradient], factor, size, componentsSizeType[ingradient], &rectWidth, &rectHeight, componentsType[ingradient],
		&maxAngle, &minAngle, totalSlopedRectangles, totalRectangles, totalCircles, totalEllipses);

	*hoop = hoops[ingradient] * factor; // Calculate hoop distance

	// Generate a random sloped rectangle within the grid boundaries
	sRectangle = generateSlopedRectangle(Point(0.0, 0.0), Point(grid->width - 1, grid->height - 1), rectWidth, rectHeight, maxAngle, minAngle);
	// Convert double coordinates to integer grid coordinates
	Center = grid->toLocaldimensions(sRectangle.center.x, sRectangle.center.y);
	dims = grid->toLocaldimensionsUp(sRectangle.width, sRectangle.height);
	double pSlope = sRectangle.slope; // Get the slope of the generated rectangle

	// Create the final SlopedRectangle object with grid-aligned coordinates
	sRectangle = SlopedRectangle(Point(Center.x, Center.y), double(dims.x), double(dims.y), pSlope);
	// sharedArea = grid->countPixelsInsidelopedRectange(Center.x, Center.y, dims.x, dims.y, sRectangle.slope, CellState::HARD);

	// Apply Swiss Cheese logic and draw the shape
	swissCheeseCheckAndDraw(ingradient, rectWidth, rectHeight, &sRectangle, *hoop);

	// Accumulate the real area of the component
	realComponentAreas[caseNo * totalComponents + ingradient] = realComponentAreas[caseNo * totalComponents + ingradient] + sRectangle.realArea; // -sharedArea;

	return sRectangle; // Return the generated sloped rectangle
}

// Adds one ellipse to the grid (simplified setup case)
Ellipse ShapeGenerator::addOneEllipse(int caseNo, int ingradient, std::normal_distribution<double> size, int* totalRectangles, int* totalSlopedRectangles, int* totalEllipses, int* totalCircles, double* hoop)
{
	double a; // Semi-major axis
	double b; // Semi-minor axis

	double maxAngle = 0; // Maximum rotation angle
	double minAngle = 0; // Minimum rotation angle
	// double sharedArea = 0; // Shared area with existing shapes (commented out)

	Ellipse cEllipse(Point(0, 0), 0, 0, 0); // Temporary Ellipse object

	// Setup sizes and angles for the ellipse based on component properties
	SetupSizes(ingradient, dimensionY[ingradient], dimensionX[ingradient], factor, size, componentsSizeType[ingradient], &a, &b, componentsType[ingradient], &maxAngle, &minAngle, totalRectangles, totalSlopedRectangles, totalCircles, totalEllipses);

	// Calculate hoop distance (half)
	*hoop = 0.5 * hoops[ingradient] * factor;

	// randomize maxTries (commented out)
	// maxTries = 3;

	iPoint Center(0, 0); // Integer center coordinates
	iPoint axes(0, 0);   // Integer axes dimensions
	Point dCenter(0, 0); // Double center coordinates (not used)

	// Generate a random ellipse within the grid boundaries
	cEllipse = generateEllipse(Point(0, 0), Point(grid->width - 1, grid->height - 1), a, b, maxAngle, minAngle);
	// Convert double coordinates to integer grid coordinates
	Center = grid->toLocaldimensions(cEllipse.center.x, cEllipse.center.y);
	axes = grid->toLocaldimensionsUp(cEllipse.a, cEllipse.b);
	double pSlope = cEllipse.slope; // Get the slope of the generated ellipse
	// Create the final Ellipse object with grid-aligned coordinates
	cEllipse = Ellipse(Point(Center.x, Center.y), double(axes.x), double(axes.y), pSlope);

	// sharedArea = grid->countPixelsInsideEllipse(Center.x, Center.y, axes.x, axes.y, cEllipse.slope, CellState::HARD);

	// Apply Swiss Cheese logic and draw the shape
	swissCheeseCheckAndDraw(ingradient, a, b, &cEllipse, *hoop);

	// Accumulate the real area of the component
	realComponentAreas[caseNo * totalComponents + ingradient] = realComponentAreas[caseNo * totalComponents + ingradient] + cEllipse.realArea; // -sharedArea;
	// debug only cout << "Area=" << realComponentAreas[caseNo * totalComponents + ingradient] << " " << cEllipse.realArea << "\n";

	return cEllipse; // Return the generated ellipse
}

// Sets up dimensions and angles for shapes based on component type and size type
void ShapeGenerator::SetupSizes(int ingradient, double idimensionY, double idimensionX, double ifactor, std::normal_distribution<double> size,
	SizeType icomponentsSizeType, double* rectWidth, double* rectHeight, ShapeType icomponentsType,
	double* maxAngle, double* minAngle, int* totalSlopedRectangles, int* totalRectangles, int* totalCircles, int* totalEllipses)
{
	bool haveSlope = false; // Flag to indicate if the shape can have a slope
	double shapeFactor = 1.0; // Factor to adjust shape dimensions

	// Determine shape type and update counters/shapeFactor/haveSlope
	switch (icomponentsType)
	{
	case RECTANGLE:
		*totalRectangles = *totalRectangles + 1; shapeFactor = 1;  break;
	case SLOPEDRECTANGLE:
		*totalSlopedRectangles = *totalSlopedRectangles + 1; haveSlope = true; shapeFactor = 1;
		break;
	case CIRCLE:
		*totalCircles = *totalCircles + 1; shapeFactor = 0.5; // Circle uses radius, hence 0.5 factor
		break;
	case ELLIPSE:
		*totalEllipses = *totalEllipses + 1; shapeFactor = 0.5; haveSlope = true; // Ellipse uses semi-axes, hence 0.5 factor
		break;
	case NOTHING:
		break;
	default:
		break;
	} // componentsType; (This line seems like a typo, should probably be removed or integrated)

	// Determine shape dimensions based on size type (VARIABLE or FIXED)
	if (icomponentsSizeType == VARIABLE)
	{
		// Width is randomized based on normal distribution
		*rectWidth = size(eng) * shapeFactor;
		// Height is calculated based on aspect ratio
		*rectHeight = *rectWidth * idimensionY / idimensionX;
	}
	else // FIXED size
	{
		*rectWidth = idimensionX * ifactor * shapeFactor;
		*rectHeight = idimensionY * ifactor * shapeFactor;
	}

	// Set max/min angles based on whether the shape has a slope
	if (haveSlope)
	{
		*maxAngle = 90;  // Allows for rotation
		*minAngle = -90; // Allows for rotation
	}
	else
	{
		*maxAngle = 0; // No rotation
		*minAngle = 0; // No rotation
	}
}

// Handles drawing an ellipse with or without "Swiss Cheese" effect
void ShapeGenerator::swissCheeseCheckAndDraw(int ingradient, double a, double b, Ellipse* cEllipse, double hoop)
{
	if (!swissCheese) // Standard drawing (no "Swiss Cheese" effect)
	{
		// Add core plus hoop area
		if (hoop > 0)
		{
			cEllipse->a = a + hoop; // Increase semi-major axis for hoop
			cEllipse->b = b + hoop; // Increase semi-minor axis for hoop
			digitizeEllipse(ingradient, *cEllipse, grid, CellState::SOFT); // Draw hoop as SOFT
		}
		// Add only core area
		cEllipse->a = a; // Reset semi-major axis to original size
		cEllipse->b = b; // Reset semi-minor axis to original size
		digitizeEllipse(ingradient, *cEllipse, grid, &cEllipse->realArea, CellState::HARD); // Draw core as HARD
	}
	else // "Swiss Cheese" effect (draw matrix as HARD, particles as holes)
	{
		// Draw the outer boundary of the hole as SOFT (the particle itself)
		if (hoop > 0)
		{
			cEllipse->a = a; // Use original semi-major axis
			cEllipse->b = b; // Use original semi-minor axis
			digitizeEllipse(ingradient, *cEllipse, grid, CellState::SOFT); // Draw particle boundary as SOFT
		}
		// Draw the "hole" (inner part of the particle) as HARD (matrix phase)
		cEllipse->a = a - hoop; // Decrease semi-major axis for the hole
		cEllipse->b = b - hoop; // Decrease semi-minor axis for the hole
		digitizeEllipse(ingradient, *cEllipse, grid, &cEllipse->realArea, CellState::HARD); // Draw inner hole as HARD
	}
}

// Handles drawing a sloped rectangle with or without "Swiss Cheese" effect
void ShapeGenerator::swissCheeseCheckAndDraw(int ingradient, double rectWidth, double rectHeight, SlopedRectangle* sRectangle, double hoop)
{
	if (!swissCheese) // Standard drawing
	{
		// Add core plus hoop area
		if (hoop > 0)
		{
			// Create a temporary SlopedRectangle with hoop size
			SlopedRectangle sRectWithHoop(Point(sRectangle->center.x, sRectangle->center.y), rectWidth + hoop, rectHeight + hoop, sRectangle->slope);
			digitizeSlopedRectangle(ingradient, sRectWithHoop, grid, CellState::SOFT); // Draw hoop as SOFT
		}

		sRectangle->width = rectWidth;   // Reset width to original
		sRectangle->height = rectHeight; // Reset height to original
		digitizeSlopedRectangle(ingradient, *sRectangle, grid, &sRectangle->realArea, CellState::HARD); // Draw core as HARD
	}
	else // "Swiss Cheese" effect
	{
		// Draw the outer boundary of the hole as SOFT (the particle itself)
		if (hoop > 0)
		{
			sRectangle->width = rectWidth;   // Use original width
			sRectangle->height = rectHeight; // Use original height
			digitizeSlopedRectangle(ingradient, *sRectangle, grid, CellState::SOFT); // Draw particle boundary as SOFT
		}
		// Draw the "hole" (inner part of the particle) as HARD (matrix phase)
		SlopedRectangle sRectWithHoop(Point(sRectangle->center.x, sRectangle->center.y), rectWidth - hoop, rectHeight - hoop, sRectangle->slope);
		digitizeSlopedRectangle(ingradient, sRectWithHoop, grid, &sRectangle->realArea, CellState::HARD); // Draw inner hole as HARD
	}
}

// Performs Monte Carlo simulations
void ShapeGenerator::monteCarlo(void)
{
	string sPercolate[2] = { "RSE Is NOT percolate\n", "RSE IS percolate\n" }; // Percolation status messages
	double sumPercolation = 0;       // Sum of percolation results
	double sumTime = 0;              // Sum of simulation times
	double sumSetupTime = 0;         // Sum of setup times

	double sumElectricConductivity = 0; // Sum of electric conductivities
	double sumThemalConductivity = 0;   // Sum of thermal conductivities
	double sumYoungModulus = 0;         // Sum of Young's moduli
	double sumPoissonRatio = 0;         // Sum of Poisson's ratios
	double sumPaths = 0;                // Total number of paths found
	double meanRVEPaths = 0;            // Mean paths per RVE
	double sumPathLength = 0;           // Sum of path lengths
	double meanRVEPathLength = 0;       // Mean path length per RVE
	double meanRVEPathWidth = 0;        // Mean path width
	int totalConductivesRSE = 0;        // Count of RSEs that conducted

	cout << "--------------------------------------------------------------------------------------------------------------\n";
	cout << "Pixels Per minimum Size (ppms)=" << pixelsPerMinimumSize << "\n";
	cout << "Minimum Size " << min << "\n"; // Note: 'min' is a global or member variable, not passed as argument
	cout << "Factor [ppms/ minimumSize]" << factor << "\n";

	for (int i = 0; i < iterations; i++) // Loop for each Monte Carlo iteration
	{
		std::seed_seq seed{ r(), r(), r(), r(), r(), r(), r(), r(), r() }; // Seed for random number engine
		eng.seed(seed); // Apply the seed

		cout << "\n--------------------------------------------";
		cout << "Case " << setw(6) << (i + 1) << " of " << setw(6) << iterations;
		cout << "--------------------------------------------\n";
		string Base;

		// Determine output image file extension
		if (!settings->saveAsBmpImage)
			Base = "_Result.pgm";
		else
			Base = "_Result.bmp";
		Results[i] = 0; // Initialize result for current case
		grid->clear();  // Clear the grid for a new simulation

		// Setup the case (lattice or detailed shape generation)
		if (settings->isLattice)
			setupCaseLattice(i, &setUpTimes[i]);
		else
			setupCase(i, &setUpTimes[i]);

		// Initialize path-related variables for current case
		paths[i] = 0;
		meanPathLength[i] = 0;

		// Calculate electric conductivity if enabled
		if (calcElectricConductivity)
		{
			// Perform percolation with real path length calculation
			Results[i] = grid->percolateWithRealPathLength(&paths[i], &meanPathLength[i],
				&meanRealPathLength[i], &Times[i], materialsElectricConductivity, &this->meanRVEResistances[i],
				materialsThermalConductivity, &this->meanRVEThermalResistance[i], materialsYoungModulus,
				&this->meanRVEYoungModulus[i], materialsPoissonRatio, &this->meanRVEPoissoonRatio[i]);
		}
		else
		{
			// Perform simple percolation check
			Results[i] = grid->percolate(&Times[i]);

			// If percolates and electric conductivity isn't calculated, assume at least one path
			if (Results[i])
				paths[i] = 1;
		}

		if (!Results[i]) // If RSE did not percolate
		{
			paths[i] = 0; // No paths found
			cout << sPercolate[Results[i]]; // "RSE not percolated\n";
		}
		else // If RSE percolated
		{
			sumPathLength += (meanRealPathLength[i] * paths[i]); // Accumulate path length
			sumPaths += paths[i];         // Accumulate total paths
			sumPercolation += Results[i]; // Accumulate percolation success
			cout << sPercolate[Results[i]]; // "RSE percolated\n";
		}

		sumTime += Times[i];       // Accumulate simulation time
		sumSetupTime += setUpTimes[i]; // Accumulate setup time

		// Calculate mean RVE path length
		if (sumPaths > 0)
			meanRVEPathLength = sumPathLength / sumPaths;
		else
			meanRVEPathLength = 0;

		// Calculate material properties if paths exist
		if (paths[i] > 0)
		{
			electricConductivity[i] = 1 / meanRVEResistances[i];      // Electric conductivity
			thermalConductivities[i] = 1 / meanRVEThermalResistance[i]; // Thermal conductivity
			YoungModulus[i] = meanRVEYoungModulus[i];                 // Young's modulus
			PoissonRatio[i] = meanRVEPoissoonRatio[i];               // Poisson's ratio
			totalConductivesRSE++;                                    // Increment count of conductive RSEs
		}
		// Print current percolation probability
		cout << "RSE's Percolation Probability:" << ((1.0 * totalConductivesRSE) / (i + 1.0)) * 100.0 << "% \n";

		// Accumulate material properties for overall mean calculation
		sumElectricConductivity += electricConductivity[i];
		sumThemalConductivity += thermalConductivities[i];
		sumYoungModulus += YoungModulus[i];
		sumPoissonRatio += PoissonRatio[i];

		// Calculate electric conductivity using Finite Differences Method if enabled
		if (calcElectricConductivityWithFDM)
		{
			cout << "--------------------------------------------------------------------------------------------------------------\n";
			cout << "Calculate Electric conductivity with Finite Differences Method..\n";
			int x = this->grid->width;
			int y = x; // Assuming square grid for FDM
			FD2DEL fd2Del(totalComponents, maxComponents); // Create FDM solver object
			fd2Del.initValues(x, y);                       // Initialize values
			fd2Del.intitArrays();                          // Initialize arrays
			fd2Del.readFromArray(this->grid->ingadients, materialsElectricConductivity); // Read material data from grid
			char m[] = "m1.dat";   // Input file for FDM (not directly used in `run` with grid data)
			char out[] = "out.bmp"; // Output bitmap file name

			fd2Del.run(m, out, totalComponents); // Run FDM simulation

			generateBitmapImageFortranStyle(fd2Del.pix, height + 2, width + 2, out); // Generate bitmap image

			FDResults[i * 2] = fd2Del.currx;     // Store FDM result for x-direction
			FDResults[i * 2 + 1] = fd2Del.curry; // Store FDM result for y-direction
		}

		// Calculate statistics if enabled
		if (this->calcStatistcs)
		{
			cout << "--------------------------------------------------------------------------------------------------------------\n";
			cout << "Calculate Statistics.. Mark Clusters,";

			// Cluster cCluster = grid->markClusters(); // Mark clusters (commented out)
			double corrLength = grid->findCorrelationLength(grid->Clusters); // Find correlation length
			cout << "Correlation length=" << corrLength << "\n";

			correleationLengths.push_back(corrLength); // Store correlation length
		}

		char* fileName = "cluster.bmp"; // Filename for cluster image

		// Save image file if enabled
		if (settings->saveImageFile)
		{
			string FileName = to_string(i) + Base; // Construct image filename
			FileName = projectName + "/images/" + FileName; // Add project path

			if (!settings->RandomSaveImageFile) // Save at fixed intervals
			{
				if (i % (iterations / settings->totalImagesToSave) == 0) // Check if it's time to save
				{
					cout << "Save image file .. " << FileName << "\n";
					grid->saveToDisk(&FileName[0u], settings->saveAsBmpImage); // Save grid to disk
					if (calcStatistcs)
					{
						saveClustersAsBitmapImage(grid->cluster, grid->height, grid->width, 0xAA, fileName); // Save clusters as image
						cout << "\nSave Clusters as image\n";
					}
				}
			}
			else // Save randomly
			{
				if ((rand() % iterations) < settings->totalImagesToSave) // Random chance to save
				{
					cout << "Save images files .. " << FileName << "\n";
					grid->saveToDisk(&FileName[0u], settings->saveAsBmpImage); // Save grid to disk
				}
			}
		}
	}

	// Calculate overall mean values after all iterations
	meanRVEPaths = sumPaths / iterations;                      // Mean paths per RVE
	meanPercolation = sumPercolation / iterations;             // Mean percolation probability
	meanTime = sumTime / iterations;                           // Mean simulation time
	meanSetUpTime = sumSetupTime / iterations;                 // Mean setup time
	// Calculate mean electric conductivity (mixture rule applied)
	meanElectricConductivity = (sumElectricConductivity / iterations) + ((width - meanRVEPaths) * materialsElectricConductivity[0] / width);
	// Calculate mean thermal conductivity (mixture rule applied)
	meanThermalConductivity = (sumThemalConductivity / iterations) + ((width - meanRVEPaths) * materialsThermalConductivity[0] / width);
	// Calculate mean path width
	meanRVEPathWidth = (meanElectricConductivity / materialsElectricConductivity[1]) * (meanRVEPathLength / meanRVEPaths);

	/*
	// Various commented-out attempts at estimating Young's Modulus and Poisson's Ratio
	// based on mixture laws or other theories.
	// The final estimation below is likely the one being used.
	*/

	/* This estimation is based on assumption that k = (nw)^(-S/L) which gives f about 4*/
	if (meanPercolation > 0) // If there is percolation
	{
		// Estimate Young's Modulus using a specific model
		meanYoungModulus = pow((materialsYoungModulus[1] * meanRVEPaths * meanRVEPathWidth), (width / meanRVEPathLength)) + (materialsYoungModulus[0] * (width - (meanRVEPaths * meanRVEPathWidth)) / width);
		// Estimate Poisson's Ratio using a mixture rule
		meanPoissonRatio = (materialsPoissonRatio[1] * meanRVEPaths * meanRVEPathWidth / width) + (materialsPoissonRatio[0] * (width - (meanRVEPaths * meanRVEPathWidth)) / width);
	}
	else // If no percolation
	{
		// Assume properties are those of the base material (matrix)
		meanYoungModulus = materialsYoungModulus[0];
		meanPoissonRatio = materialsPoissonRatio[0];
	}

	// Print final results
	cout << "-----------------------------------Results (RSE)-------------------------------------------------------------\n";
	cout << "Mean Percolation  =" << meanPercolation << "\n";
	cout << "Mean Process Time  =" << meanTime << "\n";
	cout << "Mean SetUp Time  =" << meanSetUpTime << "\n";
	cout << "Mean Paths per RSE=" << meanRVEPaths << "\n";
	cout << "Mean Paths Length per RSE=" << meanRVEPathLength << "\n";
	cout << "Mean path Width =" << meanRVEPathWidth << "\n";
	cout << "Mean Special Electric Conductivity =" << meanElectricConductivity << "\n";
	cout << "Mean Special Thermal Conductivity =" << meanThermalConductivity << "\n";
	cout << "Mean Young Modulus =" << meanYoungModulus << "\n";
	cout << "Mean Poisson Ratio " << meanPoissonRatio << "\n";
	cout << "--------------------------------------------------------------------------------------------------------------\n";

	grid->clear(); // Clear the grid at the end of the simulation
}

void ShapeGenerator::calcMeanCorrellationLength()
{
    double sum = 0; // Initialize sum of correlation lengths

    // Sum up all stored correlation lengths
    for (size_t i = 0; i < correleationLengths.size(); i++)
        sum = sum + correleationLengths[i];
    // Calculate the mean correlation length
    correleationLength = sum / correleationLengths.size();
}

void ShapeGenerator::saveResultstoReport(ReportType ireportType)
{
    string seperator; // Separator character for CSV (comma or semicolon)
    string FileName;  // Name of the report file
    ofstream File;    // Output file stream

    // Determine separator and filename based on report type
    if (ireportType == SEMICOLON)
    {
        seperator = ";";
        FileName = projectName + "/ReportWithSemicolon.csv";
        File.open(FileName);
        std::locale cpploc{ "" }; // Set locale for correct number formatting
        File.imbue(cpploc);
    }
    else // Default to COMMA
    {
        seperator = ",";
        FileName = projectName + "/report.csv";
        File.open(FileName);
    }

    // Write general information to the report
    File << info.program;  // Program name
    File << info.version;  // Program version
    File << "Report starting: " << seperator << NowToString() << "\n"; // Report start time

    File << "---------------------------------------------------------------------------------------------------\n";
    File << "Inputs\n";
    // Write header for input parameters
    File << setw(6) << "|#" << seperator <<
        setw(9) << " % Weights" << seperator <<
        setw(15) << " % Spec.Weights" << seperator <<
        setw(12) << "X Dimension" << seperator <<
        setw(12) << "Y Dimension" << seperator <<
        setw(5) << "Type" << seperator <<
        setw(10) << "Size type" << seperator <<
        setw(11) << "Area" << seperator <<
        setw(9) << "hoop" << seperator <<
        //setw(9) << "Dist" << // Commented out, possibly an old field
        "\n";
    // Write each component's input parameters
    for (int i = 0; i < totalComponents; i++)
        File << setw(6) << i << seperator <<
        setw(9) << components[i] << seperator <<
        setw(15) << specialWeights[i] << seperator <<
        setw(12) << dimensionX[i] << seperator <<
        setw(12) << dimensionY[i] << seperator <<
        setw(5) << componentsType[i] << seperator <<
        setw(10) << componentsSizeType[i] << seperator <<
        setw(11) << componentsArea[i] << seperator <<
        setw(9) << hoops[i] << "\n";
    // Write other simulation input parameters
    File << "Pixels Per minimum Size (ppms):" << seperator << pixelsPerMinimumSize << "\n";
    File << "Factor [ppms / minimumSize]:" << seperator << factor << "\n";
    File << "Maximum Dimension:" << seperator << max << "\n"; // Max dimension used in generation
    File << "Minimum Dimension:" << seperator << min << "\n"; // Min dimension used in generation
    File << "Grid size" << seperator << grid->width << seperator << "x" << seperator << grid->height << "\n";
    File << "Total iterations: " << seperator << iterations << "\n";

    File << "---------------------------------------------------------------------------------------------------\n";

    // Write header for simulation results
    File << "Realization No" << seperator << "Percolate" "Y[1]/N[0]" << seperator << "Process Time" << seperator << "Setup Time ";
    // Add columns for real area of each component
    for (int i = 0; i < totalComponents; i++)
        File << setw(15) << seperator << "% Real Area Comp[" << i << "]";

    // Add columns for statistics if enabled
    if (calcStatistcs)
        File << seperator << "Max" << seperator << "Cluster Radius" << seperator << "Correlation Length";

    // Add columns for conductivity and mechanical properties if enabled
    if (calcElectricConductivity)
        File << seperator << "Electric Conductivity" << seperator << "Thermal Conductivity" << seperator << "Young Modulus" << seperator << "Poisson Ratio"
        << seperator << "Total Conductive Paths" << seperator << "Mean Real Path Length" << seperator << "Current Electric Conductivity";

    // Add columns for FDM results if enabled
    if (calcElectricConductivityWithFDM)
        File << seperator << "FDM Ix" << seperator << "FDM Iy" << seperator << "FDM ro";

    File << "\n";

    // Write results for each iteration
    for (int i = 0; i < iterations; i++)
    {
        File << setw(7) << i + 1 << setw(1) << seperator << setw(19) << Results[i] << seperator << setw(10) << Times[i] << seperator << setw(15) << setUpTimes[i];

        // Write real area for each component in the current iteration
        for (int j = 0; j < totalComponents; j++)
        {
            File << setw(13) << seperator << realComponentAreas[(i * totalComponents) + j];
        }
        // Write statistics results if enabled
        if (calcStatistcs)
            File << seperator << setw(13) << this->grid->cMaxClusterRadius[i]
            << seperator << setw(13) << correleationLengths[i];
        double eleCondu;
        // Calculate electric conductivity based on paths
        if (paths[i] > 0)
            eleCondu = (1 / this->meanRVEResistances[i]) * this->paths[i] / width;
        else
            eleCondu = 0;
        // Write conductivity and mechanical properties if enabled
        if (calcElectricConductivity)
            File << seperator << setw(13) << std::fixed << std::setprecision(std::numeric_limits<double>::max_digits10) << this->meanRVEResistances[i]
            << seperator << setw(13) << std::fixed << std::setprecision(std::numeric_limits<double>::max_digits10) << this->meanRVEThermalResistance[i]
            << seperator << setw(13) << std::fixed << std::setprecision(std::numeric_limits<double>::max_digits10) << this->meanRVEYoungModulus[i]
            << seperator << setw(13) << std::fixed << std::setprecision(std::numeric_limits<double>::max_digits10) << this->meanRVEPoissoonRatio[i]
            << seperator << setw(13) << std::fixed << std::setprecision(std::numeric_limits<double>::max_digits10) << this->paths[i]
            << seperator << setw(13) << std::fixed << std::setprecision(std::numeric_limits<double>::max_digits10) << this->meanRealPathLength[i]
            << seperator << setw(13) << std::fixed << std::setprecision(std::numeric_limits<double>::max_digits10) << eleCondu;

        // Write FDM results if enabled
        if (calcElectricConductivityWithFDM)
        {
            File << seperator << setw(13) << this->FDResults[2 * i] << seperator << setw(13) << this->FDResults[(2 * i) + 1] << seperator;
            if (this->FDResults[2 * i] != 0)
                File << setw(13) << (1.0 / this->FDResults[(2 * i)]);
            else
                File << setw(13) << 0.0;

            if (this->FDResults[2 * i + 1] != 0)
                File << setw(13) << (1.0 / this->FDResults[(2 * i) + 1]);
            else
                File << setw(13) << 0.0;
        }
        File << "\n";
    }
    File << "---------------------------------------------------------------------------------------------------\n";

    // Write mean values of overall simulation results
    File << "Mean Percolation=" << seperator << meanPercolation << "\n";
    File << "Mean Processing Time=" << seperator << meanTime << "\n";
    File << "Mean set up time=" << seperator << meanSetUpTime << "\n";
    // Write mean correlation length if statistics are enabled
    if (calcStatistcs)
        File << "Mean correlation Length=" << correleationLength << "\n";

    // Calculate and write mean area for each component
    for (int j = 0; j < totalComponents; j++)
    {
        double mean = 0;
        for (int i = 0; i < iterations; i++)
            mean = mean + realComponentAreas[i * totalComponents + j];
        mean = mean / iterations;

        File << "Mean Area of component[" << j << "]=" << seperator << mean << "\n";
    }
    File.close(); // Close the report file
}

// Saves results to a report with comma as a separator
void ShapeGenerator::Report()
{
    saveResultstoReport(ReportType::COMMA);
}

// Saves results to a report with semicolon as a separator
void ShapeGenerator::ReportWithSemicolon()
{
    saveResultstoReport(ReportType::SEMICOLON);
}

// Generates and saves a statistics report
void ShapeGenerator::ReportStatistics(string seperator)
{
    ofstream File; // Output file stream

    string FileName = projectName + "/Statistics.csv"; // Statistics report filename

    File.open(FileName); // Open the file

    File << info.program;  // Program name
    File << info.version;  // Program version

    File << "Report starting: " << NowToString() << "\n"; // Report start time

    File << "---------------------------------------------------------------------------------------------------\n";
    File << "Inputs\n";
    // Write header for input parameters
    File << "|#  |%Weights|%Spec. Weights|X Dimension|Y Dimension|Type|Size type|       Area|hoop Dist\n";
    // Write each component's input parameters
    for (int i = 0; i < totalComponents; i++)
        File << setw(4) << i << setw(9) << components[i] << setw(15) << specialWeights[i] << setw(12) << dimensionX[i] << setw(12) << dimensionY[i] << setw(5) << componentsType[i] << setw(10) << componentsSizeType[i] << setw(11) << componentsArea[i] << setw(9) << hoops[i] << "\n";

    // Write other simulation input parameters
    File << "Pixels Per minimum Size(ppms):" << pixelsPerMinimumSize << "\n";
    File << "Factor [ppms / minimumSize]:" << factor << "\n";
    File << "Maximum Dimension:" << seperator << max << "\n";
    File << "Minimum Dimension:" << seperator << min << "\n";
    File << "Grid size =[" << seperator << grid->width << "x" << seperator << grid->height << "]" << "\n";
    File << "Total iterations: " << seperator << iterations << "\n";

    File << "---------------------------------------------------------------------------------------------------\n";
    File << "Total  Points  |Mean Radius | Inertia \n"; // Header for cluster statistics

    //clustersSize = int(0.5 * sqrt(grid->height * grid->height + grid->width* grid->width)); // Commented out
    // Write statistics for each cluster
    for (size_t i = 0; i < grid->Clusters.size(); i++)
    {
        if (grid->Clusters[i].totalPoints > 0) // Only if cluster has points
        {
            File << grid->Clusters[i].totalPoints; // Total points in cluster
            File << seperator << setw(10) << grid->Clusters[i].radius; // Mean radius of cluster
            File << seperator << setw(10) << grid->Clusters[i].inertia; // Inertia of cluster
        }
        File << "\n";
    }
    File << "---------------------------------------------------------------------------------------------------\n";
    File.close(); // Close the file
}

// Generates and saves a statistics report with semicolon as a separator (default)
void ShapeGenerator::ReportStatistics(void)
{
    ReportStatistics(";");
}

// Generates a sloped rectangle with random center and optional random angle
SlopedRectangle ShapeGenerator::generateSlopedRectangle(Point downleft, Point upRight, double iwidth, double iheight, double maxAngle, double minAngle)
{
    double angle; // Rotation angle
    // Generate random center point within the specified bounds
    Point center(std::uniform_real_distribution<>{downleft.x, upRight.x }(eng), std::uniform_real_distribution<> {downleft.y, upRight.y }(eng));

    // Generate random angle if maxAngle and minAngle are different
    if (maxAngle != minAngle)
        angle = std::uniform_real_distribution<>{ minAngle, maxAngle }(eng);
    else
        angle = maxAngle; // Use fixed angle if max and min are the same

    angle = angle * M_PI / (180); // Convert angle from degrees to radians

    SlopedRectangle Result(center, iwidth, iheight, angle); // Create SlopedRectangle object

    return Result; // Return the generated sloped rectangle
}

// Generates an ellipse with random center and optional random angle
Ellipse ShapeGenerator::generateEllipse(Point downleft, Point upRight, double ia, double ib, double maxAngle, double minAngle)
{
    // Generate random center point within the specified bounds
    Point center(std::uniform_real_distribution<>{downleft.x, upRight.x }(eng), std::uniform_real_distribution<> {downleft.y, upRight.y }(eng));

    double angle; // Rotation angle
    // Generate random angle if maxAngle and minAngle are different
    if (maxAngle != minAngle)
        angle = std::uniform_real_distribution<>{ minAngle, maxAngle }(eng);
    else
        angle = maxAngle; // Use fixed angle if max and min are the same

    angle = angle * M_PI / (180); // Convert angle from degrees to radians
    Ellipse Result(center, ia, ib, angle); // Create Ellipse object

    return Result; // Return the generated ellipse
}

// Digitizes a sloped rectangle onto the grid without calculating real area
void ShapeGenerator::digitizeSlopedRectangle(int ingradient, SlopedRectangle sRect, Grid* iGrid, CellState state)
{
    int i, j;
    // Calculate the bounding box (region) for iterating over grid cells
    Range region(iGrid->toLocaldimensionsDown(sRect.minX() - 1, sRect.minY() - 1), iGrid->toLocaldimensionsUp(sRect.maxX() + 1, sRect.maxY() + 1));

    // Clamp region coordinates to grid boundaries
    if (region.from.x < 0)
        region.from.x = 0;
    if (region.from.y < 0)
        region.from.y = 0;
    if (region.to.x > iGrid->width)
        region.to.x = iGrid->width;
    if (region.to.y > iGrid->height)
        region.to.y = iGrid->height;

    // Iterate over grid cells within the bounding box
    for (i = region.from.x; i < region.to.x; i++)
        for (j = region.from.y; j < region.to.y; j++)
        {
            // Transform (rotate) the current grid cell's coordinates back to the rectangle's local coordinate system
            Point rotated = Point::transform(i, j, sRect.center.x, sRect.center.y, -sRect.slope);
            // Check if the rotated point is inside the rectangle's dimensions
            if ((1.0 * rotated.x <= (1.0 * sRect.center.x + (sRect.width / 2.0)))
                && (1.0 * rotated.x >= (1.0 * sRect.center.x - (sRect.width / 2.0)))
                && (1.0 * rotated.y <= (1.0 * sRect.center.y + (sRect.height / 2.0)))
                && (1.0 * rotated.y >= (1.0 * sRect.center.y - (sRect.height / 2.0))))
            {
                if (state == CellState::HARD) // If drawing a HARD cell
                {
                    iGrid->set(i, j, state);                  // Set cell state to HARD
                    iGrid->ingadients[j * width + i] = (unsigned char)(ingradient); // Set component gradient
                }
                else // If drawing a SOFT cell (e.g., hoop)
                    if (iGrid->get(i, j) != CellState::HARD) // Only set if not already HARD
                    {
                        iGrid->ingadients[j * width + i] = (unsigned char)(ingradient); // Set component gradient
                        iGrid->set(i, j, state); // Set cell state to SOFT
                    }
            }
        }
}

// Digitizes a sloped rectangle onto the grid and calculates its real area
void ShapeGenerator::digitizeSlopedRectangle(int ingradient, SlopedRectangle sRect, Grid* iGrid, double* realArea, CellState state)
{
    int i, j;
    // Calculate the bounding box (region) for iterating over grid cells
    Range region(iGrid->toLocaldimensionsDown(sRect.minX() - 1, sRect.minY() - 1), iGrid->toLocaldimensionsUp(sRect.maxX() + 1, sRect.maxY() + 1));
    *realArea = 0; // Initialize real area count

    // Clamp region coordinates to grid boundaries
    if (region.from.x < 0)
        region.from.x = 0;
    if (region.from.y < 0)
        region.from.y = 0;
    if (region.to.x > iGrid->width)
        region.to.x = iGrid->width;
    if (region.to.y > iGrid->height)
        region.to.y = iGrid->height;

    // Iterate over grid cells within the bounding box
    for (i = region.from.x; i < region.to.x; i++)
        for (j = region.from.y; j < region.to.y; j++)
        {
            // Transform (rotate) the center of the current grid cell back to the rectangle's local coordinate system
            Point rotated = Point::transform(i + 0.5, j + 0.5, sRect.center.x, sRect.center.y, -sRect.slope);
            // Check if the rotated point is inside the rectangle's dimensions
            if ((rotated.x <= (sRect.center.x + (sRect.width / 2))) && (rotated.x >= (sRect.center.x - (sRect.width / 2))) && ((rotated.y <= (sRect.center.y + (sRect.height / 2))) && rotated.y >= (sRect.center.y - (sRect.height / 2))))
            {
                char cPixel = iGrid->get(i, j); // Get current cell state
                if (state == CellState::HARD)    // If drawing a HARD cell
                {
                    // If the cell is not already occupied by a hard phase or border (for "Swiss Cheese" effect)
                    if ((cPixel != 0) && (cPixel != CellState::HARD) && (cPixel != BORDER)) // in one step
                    {
                        iGrid->ingadients[j * width + i] = (unsigned char)(ingradient); // Set component gradient
                        *realArea += 1.0; // Increment real area
                    }
                    iGrid->set(i, j, state); // Set cell state to HARD
                }
                else // If drawing a SOFT cell (e.g., hoop)
                    if (cPixel != CellState::HARD) // Only set if not already HARD
                        iGrid->set(i, j, state); // Set cell state to SOFT
            }
        }
}

// Digitizes a sloped rectangle with a border onto the grid and calculates real area
void ShapeGenerator::digitizeSlopedRectangleWithBorder(int ingradient, SlopedRectangle sRect, Grid* iGrid, double* realArea, CellState state)
{
    int i, j;
    double zone = 0.90; // Defines the inner "core" zone as a percentage of the dimension
    // Calculate the bounding box (region) for iterating over grid cells
    Range region(iGrid->toLocaldimensionsDown(sRect.minX() - 1, sRect.minY() - 1), iGrid->toLocaldimensionsUp(sRect.maxX() + 1, sRect.maxY() + 1));
    *realArea = 0; // Initialize real area count

    // Clamp region coordinates to grid boundaries
    if (region.from.x < 0)
        region.from.x = 0;
    if (region.from.y < 0)
        region.from.y = 0;
    if (region.to.x > iGrid->width)
        region.to.x = iGrid->width;
    if (region.to.y > iGrid->height)
        region.to.y = iGrid->height;

    // Iterate over grid cells within the bounding box
    for (i = region.from.x; i < region.to.x; i++)
        for (j = region.from.y; j < region.to.y; j++)
        {
            // Transform (rotate) the center of the current grid cell back to the rectangle's local coordinate system
            Point rotated = Point::transform(i + 0.5, j + 0.5, sRect.center.x, sRect.center.y, -sRect.slope);

            // Check if the rotated point is inside the overall rectangle's dimensions
            if ((rotated.x <= (sRect.center.x + (sRect.width / 2))) && (rotated.x >= (sRect.center.x - (sRect.width / 2))) && ((rotated.y <= (sRect.center.y + (sRect.height / 2))) && rotated.y >= (sRect.center.y - (sRect.height / 2))))
            {
                // Check if the point is within the "border" zone
                if ((rotated.x <= sRect.center.x + (sRect.width / 2) && rotated.x >= sRect.center.x + (zone * sRect.width / 2)) ||
                    (rotated.x >= sRect.center.x - (sRect.width / 2) && rotated.x <= sRect.center.x - (zone * sRect.width / 2)) ||
                    (rotated.y <= sRect.center.y + (sRect.height / 2) && rotated.y >= sRect.center.y + (zone * sRect.height / 2)) ||
                    (rotated.y >= sRect.center.y - (sRect.height / 2) && rotated.y <= sRect.center.y - (zone * sRect.height / 2)))
                {
                    if (state == CellState::HARD) // If drawing a HARD cell (core)
                    {
                        if (iGrid->get(i, j) != 0) // If the cell is not empty
                        {
                            *realArea += 1.0; // Increment real area
                            iGrid->ingadients[j * width + i] = (unsigned char)(ingradient); // Set component gradient
                        }
                        iGrid->set(i, j, BORDER); // Set cell state to BORDER
                    }
                    else // If drawing a SOFT cell (e.g., hoop)
                        if (iGrid->get(i, j) != CellState::HARD) // Only set if not already HARD
                            iGrid->set(i, j, BORDER); // Set cell state to BORDER
                }
                else // Point is within the "core" of the rectangle
                    if (state == CellState::HARD) // If drawing a HARD cell (core)
                    {
                        if (iGrid->get(i, j) != 0) // If the cell is not empty
                        {
                            *realArea += 1.0; // Increment real area
                            iGrid->ingadients[j * width + i] = (unsigned char)(ingradient); // Set component gradient
                        }
                        iGrid->set(i, j, state); // Set cell state to HARD
                    }
                    else // If drawing a SOFT cell (e.g., hoop)
                        if (iGrid->get(i, j) != CellState::HARD) // Only set if not already HARD
                            iGrid->set(i, j, state); // Set cell state to SOFT
            }
        }
}

// Digitizes an ellipse onto the grid and calculates its real area using OpenMP for parallelization
void ShapeGenerator::digitizeEllipse(int ingradient, Ellipse iEllipse, Grid* iGrid, double* realArea, CellState state)
{
    // omp_set_num_threads(4); // Set number of OpenMP threads (commented out)
    *realArea = 0;      // Initialize real area count
    double subsum[4] = {}; // Array to store partial sums from parallel sections

#pragma omp parallel // Start OpenMP parallel region
#pragma omp sections nowait // Divide work into sections, no waiting at the end of each section
    {
#pragma omp section // First section: digitize top-right quadrant
        digitizeEllipseDiv4(ingradient, iEllipse, iGrid, &subsum[0], state, true, true);

#pragma omp section // Second section: digitize top-left quadrant
        digitizeEllipseDiv4(ingradient, iEllipse, iGrid, &subsum[1], state, false, true);

#pragma omp section // Third section: digitize bottom-right quadrant
        digitizeEllipseDiv4(ingradient, iEllipse, iGrid, &subsum[2], state, true, false);

#pragma omp section // Fourth section: digitize bottom-left quadrant
        digitizeEllipseDiv4(ingradient, iEllipse, iGrid, &subsum[3], state, false, false);
    }

    // Sum up partial results from all sections to get total real area
    for (int i = 0; i < 4; i++)
    {
        *realArea = subsum[i] + *realArea;
    }
}

// Helper function to digitize one quadrant of an ellipse
void ShapeGenerator::digitizeEllipseDiv4(int ingradient, Ellipse iEllipse, Grid* iGrid, double* realArea, CellState state, bool x, bool y)
{
    int i, j;

    double distance;   // Distance from ellipse center in normalized coordinates
    Point rotated(0, 0); // Rotated point coordinates
    iPoint iRotated(0, 0); // Integer (grid) coordinates of the rotated point

    *realArea = 0;   // Initialize real area for this quadrant
    double density = 1.8; // Density factor for sampling (higher density means more sampled points)

    bool isCircle = (iEllipse.a == iEllipse.b); // Check if it's a circle
    if (isCircle)
        density = 1.0; // Use lower density for circles as they are simpler

    double invDensity = 1 / density;     // Inverse of density
    double sqInvDensity = invDensity * invDensity; // Square of inverse density

    double costhetaX = cos(iEllipse.slope); // Cosine of ellipse slope
    double sinthetaX = sin(iEllipse.slope); // Sine of ellipse slope

    double sqEllipsoidA = (iEllipse.a * iEllipse.a); // Square of semi-major axis
    double sqEllipsoidB = (iEllipse.b * iEllipse.b); // Square of semi-minor axis

    int maxi, maxj; // Maximum loop bounds for i and j

    maxi = int(density * (iEllipse.a + 2)); // Calculate max i for sampling
    maxj = int(density * (iEllipse.b + 2)); // Calculate max j for sampling

    int muli = 1; // Multiplier for x-direction (1 for positive, -1 for negative)
    int mulj = 1; // Multiplier for y-direction (1 for positive, -1 for negative)

    if (!x)
        muli = -1; // If 'x' is false, iterate in negative x-direction
    if (!y)
        mulj = -1; // If 'y' is false, iterate in negative y-direction

    int ii, jj; // Iteration variables for sampled points

    {
        for (i = 0; i < maxi; i++)
            for (j = 0; j < maxj; j++)
            {
                ii = i * muli; // Apply x-direction multiplier
                jj = j * mulj; // Apply y-direction multiplier

                // Calculate normalized distance from ellipse center (equation of an ellipse)
                distance =
                    ((sqInvDensity * ii * ii) / sqEllipsoidA) +
                    ((sqInvDensity * jj * jj) / sqEllipsoidB);

                if (distance <= 1.0) // If the sampled point is inside the ellipse
                {
                    if (!isCircle) // If it's an ellipse, rotate the sampled point
                        rotated = Point::transform(invDensity * ii, invDensity * jj, 0, 0,
                            iEllipse.slope, costhetaX, sinthetaX);
                    else
                    {
                        rotated = { (double)ii,(double)jj }; // For circles, no rotation needed (simpler)
                    }

                    rotated = { rotated.x + iEllipse.center.x, // Translate to global coordinates
                        rotated.y + iEllipse.center.y };

                    iRotated = rotated.round(); // Round to nearest integer grid coordinates

                    if (isInsideGrid(iRotated, iGrid)) // Check if the point is within grid boundaries
                    {
                        size_t where = (size_t)iRotated.y * width + iRotated.x; // Calculate 1D array index
                        char cPixel = iGrid->cell[where]; // Get current cell state

                        if (state == CellState::HARD) // If drawing a HARD cell
                        {
                            if (cPixel != CellState::HARD) // If not already HARD
                            {
                                iGrid->ingadients[where] = (unsigned char)(ingradient); // Set component gradient
                                *realArea = 1.0 + *realArea; // Increment real area
                            }
                            iGrid->cell[where] = CellState::HARD; // Set cell state to HARD
                        }
                        else // If drawing a SOFT cell (e.g., hoop)
                            if (cPixel != CellState::HARD) // If not already HARD
                            {
                                iGrid->cell[where] = state; // Set cell state to SOFT
                                iGrid->ingadients[where] = (unsigned char)(ingradient); // Set component gradient
                            }
                    }
                }
            }
    }
}

// Digitizes an ellipse onto the grid without calculating real area
void ShapeGenerator::digitizeEllipse(int ingradient, Ellipse iEllipse, Grid* iGrid, CellState state)
{
    int i, j;

    // Calculate the bounding box (region) for iterating over grid cells
    Range region(iGrid->toLocaldimensionsDown(iEllipse.minX() - 1, iEllipse.minY() - 1), iGrid->toLocaldimensionsUp(iEllipse.maxX() + 1, iEllipse.maxY() + 1));

    // Clamp region coordinates to grid boundaries
    if (region.from.x < 0)
        region.from.x = 0;
    if (region.from.y < 0)
        region.from.y = 0;
    if (region.to.x > iGrid->width)
        region.to.x = iGrid->width;
    if (region.to.y > iGrid->height)
        region.to.y = iGrid->height;

    // Iterate over grid cells within the bounding box
    for (i = region.from.x; i < region.to.x; i++)
        for (j = region.from.y; j < region.to.y; j++)
        {
            // Transform (rotate) the center of the current grid cell back to the ellipse's local coordinate system
            Point rotated = Point::transform(i + 0.5, j + 0.5, iEllipse.center.x, iEllipse.center.y, -iEllipse.slope);

            // Check if the rotated point is inside the ellipse (using ellipse equation)
            if (pow(1.0 * (rotated.x - iEllipse.center.x) / (1.0 * iEllipse.a), 2.0) + pow(1.0 * (rotated.y - iEllipse.center.y) / (1.0 * iEllipse.b), 2.0) <= 1.0)
            {
                if (iGrid->get(i, j) != CellState::HARD) // If the cell is not already HARD
                {
                    if (state == CellState::HARD) // If drawing a HARD cell
                    {
                        iGrid->set(i, j, state);                  // Set cell state to HARD
                        iGrid->set(iGrid->ingadients, i, j, ingradient); // Set component gradient
                    }
                    else // If drawing a SOFT cell
                        if (iGrid->get(i, j) != CellState::HARD) // Again, only if not already HARD
                        {
                            iGrid->set(i, j, state);                  // Set cell state to SOFT
                            iGrid->set(iGrid->ingadients, i, j, ingradient); // Set component gradient
                        }
                }
            }
        }
}

// Digitizes an ellipse with a border onto the grid and calculates its real area
void ShapeGenerator::digitizeEllipseWithBorder(Ellipse iEllipse, Grid* iGrid, double* realArea, CellState state)
{
    int i, j;
    double distance; // Distance from ellipse center in normalized coordinates
    // Calculate the bounding box (region) for iterating over grid cells
    Range region(iGrid->toLocaldimensionsDown(iEllipse.minX() - 1, iEllipse.minY() - 1), iGrid->toLocaldimensionsUp(iEllipse.maxX() + 1, iEllipse.maxY() + 1));

    *realArea = 0; // Initialize real area count

    // Clamp region coordinates to grid boundaries
    if (region.from.x < 0)
        region.from.x = 0;
    if (region.from.y < 0)
        region.from.y = 0;
    if (region.to.x > iGrid->width)
        region.to.x = iGrid->width;
    if (region.to.y > iGrid->height)
        region.to.y = iGrid->height;

    // Iterate over grid cells within the bounding box
    for (i = region.from.x; i < region.to.x; i++)
        for (j = region.from.y; j < region.to.y; j++)
        {
            // Transform (rotate) the center of the current grid cell back to the ellipse's local coordinate system
            Point rotated = Point::transform(i + 0.5, j + 0.5, iEllipse.center.x, iEllipse.center.y, -iEllipse.slope);
            // Calculate normalized distance from ellipse center
            distance = ((rotated.x - iEllipse.center.x) * (rotated.x - iEllipse.center.x) / (iEllipse.a * iEllipse.a)) + ((rotated.y - iEllipse.center.y) * (rotated.y - iEllipse.center.y) / (iEllipse.b * iEllipse.b));
            // Check if the point is within the "border" zone (between 0.8 and 1.0 normalized distance)
            if ((distance <= 1.0) && (distance >= 0.8))
            {
                if (state == CellState::HARD) // If drawing a HARD cell (core)
                {
                    if (iGrid->get(i, j) != 0) // If the cell is not empty
                        *realArea += 1.0;     // Increment real area

                    iGrid->set(i, j, BORDER); // Set cell state to BORDER
                }
                else // If drawing a SOFT cell (e.g., hoop)
                    if (iGrid->get(i, j) != CellState::HARD) // Only set if not already HARD
                        iGrid->set(i, j, state); // Set cell state to SOFT (this will actually set to BORDER based on the outer condition)
            }
            else // Point is within the "core" of the ellipse (less than 0.8 normalized distance)
                if ((distance < 0.8))
                {
                    if (state == CellState::HARD) // If drawing a HARD cell (core)
                    {
                        if (iGrid->get(i, j) != 0) // If the cell is not empty
                            *realArea += 1.0;     // Increment real area

                        iGrid->set(i, j, CellState::HARD); // Set cell state to HARD
                    }
                    else // If drawing a SOFT cell (e.g., hoop)
                        if (iGrid->get(i, j) != CellState::HARD) // Only set if not already HARD
                            iGrid->set(i, j, state); // Set cell state to SOFT
                }
        }
}

// Splits a string by a given delimiter and returns a vector of substrings
std::vector<std::string> ShapeGenerator::split(std::string strToSplit, char delimeter)
{
    std::stringstream ss(strToSplit); // Create a stringstream from the input string
    std::string item;                 // Temporary string to hold each split item
    std::vector<std::string> splittedStrings; // Vector to store the split substrings
    // Read items from the stringstream, separated by the delimiter
    while (std::getline(ss, item, delimeter))
    {
        splittedStrings.push_back(item); // Add each item to the vector
    }
    return splittedStrings; // Return the vector of split strings
}

// Reads simulation parameters from an input file
void ShapeGenerator::readFromFile(char* inputFileName)
{
    string line;          // String to hold each line read from the file
    ifstream inputFile(inputFileName); // Input file stream

    if (inputFile.is_open()) // Check if the file was opened successfully
    {
        while (getline(inputFile, line)) // Read file line by line
        {
            // Skip lines that start with '#' (comments)
            while (line[0] == '#')
            {
                getline(inputFile, line);
            }

            // Read project Name
            projectName = line;

            getline(inputFile, line);
            // Skip comments
            while (line[0] == '#')
            {
                getline(inputFile, line);
            }
            totalComponents = std::stoi(line); // Read total number of components

            //--------------------------------------
            getline(inputFile, line);
            while (line[0] == '#')
            {
                getline(inputFile, line);
            }

            // Read % weight of components
            std::vector<std::string> strComp = split(line, ' ');
            for (int i = 0; i < totalComponents; i++)
            {
                components[i] = std::stod(strComp[i]); // Convert string to double
            }

            getline(inputFile, line);
            while (line[0] == '#')
            {
                getline(inputFile, line);
            }

            // Read component types (Circle, Rectangle, Ellipse etc.)
            std::vector<std::string> strCompType = split(line, ' ');
            for (int i = 0; i < totalComponents; i++)
            {
                componentsType[i] = ShapeType(std::stoi(strCompType[i])); // Convert string to ShapeType enum
            }

            getline(inputFile, line);
            while (line[0] == '#')
            {
                getline(inputFile, line);
            }

            // Read component size types (Constant, Variable)
            std::vector<std::string> strCompSizeType = split(line, ' ');
            for (int i = 0; i < totalComponents; i++)
            {
                componentsSizeType[i] = SizeType(std::stoi(strCompSizeType[i])); // Convert string to SizeType enum
            }

            getline(inputFile, line);
            while (line[0] == '#')
            {
                getline(inputFile, line);
            }

            // Read special weight for each component
            std::vector<std::string> strWeights = split(line, ' ');
            for (int i = 0; i < totalComponents; i++)
            {
                specialWeights[i] = std::stod(strWeights[i]);
            }

            getline(inputFile, line);
            while (line[0] == '#')
            {
                getline(inputFile, line);
            }

            // Read first dimension (e.g., width or 'a' for ellipse) for each component
            std::vector<std::string> strdimX = split(line, ' ');
            for (int i = 0; i < totalComponents; i++)
            {
                dimensionX[i] = std::stod(strdimX[i]);
            }

            getline(inputFile, line);
            while (line[0] == '#')
            {
                getline(inputFile, line);
            }

            // Read second dimension (e.g., height or 'b' for ellipse) for each component
            std::vector<std::string> strdimY = split(line, ' ');
            for (int i = 0; i < totalComponents; i++)
            {
                dimensionY[i] = std::stod(strdimY[i]);
            }

            // Read hoop distances for each component
            getline(inputFile, line);
            while (line[0] == '#')
            {
                getline(inputFile, line);
            }
            std::vector<std::string> strHoops = split(line, ' ');
            for (int i = 0; i < totalComponents; i++)
            {
                hoops[i] = std::stod(strHoops[i]);
            }

            // Read electric conductivity for each material
            getline(inputFile, line);
            while (line[0] == '#')
            {
                getline(inputFile, line);
            }
            std::vector<std::string> streConductivity = split(line, ' ');
            for (int i = 0; i < totalComponents; i++)
            {
                materialsElectricConductivity[i] = std::stod(streConductivity[i]);
            }

            // Read thermal conductivity for each material
            getline(inputFile, line);
            while (line[0] == '#')
            {
                getline(inputFile, line);
            }
            std::vector<std::string> streThermalConductivity = split(line, ' ');
            for (int i = 0; i < totalComponents; i++)
            {
                materialsThermalConductivity[i] = std::stod(streThermalConductivity[i]);
            }

            // Read Young's modulus for each material
            getline(inputFile, line);
            while (line[0] == '#')
            {
                getline(inputFile, line);
            }
            std::vector<std::string> streYoungModulus = split(line, ' ');
            for (int i = 0; i < totalComponents; i++)
            {
                materialsYoungModulus[i] = std::stod(streYoungModulus[i]);
            }

            // Read Poisson's Ratio for each material
            getline(inputFile, line);
            while (line[0] == '#')
            {
                getline(inputFile, line);
            }
            std::vector<std::string> strPoissonRatio = split(line, ' ');
            for (int i = 0; i < totalComponents; i++)
            {
                materialsPoissonRatio[i] = std::stod(strPoissonRatio[i]);
            }

            getline(inputFile, line);
            while (line[0] == '#')
            {
                getline(inputFile, line);
            }
            std::vector<std::string> strSizes = split(line, ' ');
            width = std::stoi(strSizes[0]);  // Read grid width
            height = std::stoi(strSizes[1]); // Read grid height

            getline(inputFile, line);
            while (line[0] == '#')
            {
                getline(inputFile, line);
            }
            iterations = std::stoi(line); // Read total number of iterations

            getline(inputFile, line);
            while (line[0] == '#')
            {
                getline(inputFile, line);
            }
            pixelsPerMinimumSize = std::stoi(line); // Read pixels per minimum size

            getline(inputFile, line);
            while (line[0] == '#')
            {
                getline(inputFile, line);
            }

            // Read boolean for "Swiss Cheese" effect
            if ((line == "TRUE") || (line == "true"))
                swissCheese = true;
            else
                swissCheese = false;

            // Read boolean for calculating electric conductivity
            getline(inputFile, line);
            while (line[0] == '#')
            {
                getline(inputFile, line);
            }
            if ((line == "TRUE") || (line == "true"))
                calcElectricConductivity = true;
            else
                calcElectricConductivity = false;

            // Read boolean for calculating electric conductivity with FDM
            getline(inputFile, line);
            while (line[0] == '#')
            {
                getline(inputFile, line);
            }
            if ((line == "TRUE") || (line == "true"))
                calcElectricConductivityWithFDM = true;
            else
                calcElectricConductivityWithFDM = false;

            // Read boolean for calculating statistics
            getline(inputFile, line);
            while (line[0] == '#')
            {
                getline(inputFile, line);
            }
            if ((line == "TRUE") || (line == "true"))
                calcStatistcs = true;
            else
                calcStatistcs = false;
        }

        inputFile.close(); // Close the input file

        // Initialize min and max dimensions and factor
        min = minElement(dimensionY, 0, totalComponents); // Find minimum Y dimension
        max = maxElement(dimensionX, 0, totalComponents); // Find maximum X dimension

        //pixelsPerMinimumCircle = sqrt(width*min / max); // Commented out
        //pixelsPerMinimumCircle =38; // Commented out

        factor = pixelsPerMinimumSize / min; // Calculate scaling factor
    }
    else // If file opening failed
    {
        cout << "*************************************************************************************\n";
        cout << "Error cannot find ";
        std::cout.write(inputFileName, strlen(inputFileName)); // Print filename
        cout << " Program halted..." << '\n';
        cout << "*************************************************************************************\n";
        delay(4); // Some delay
        exit(-1); // Exit program with an error code
    }
}

// Exports grid data for Finite Difference Method (FDM) simulation
void ShapeGenerator::exportForFDM(char* fileName)
{
    ofstream File; // Output file stream
    File.open(fileName); // Open the file

    // Iterate over grid cells and write material gradients to file (Fortran-style indexing)
    for (int j = 2; j <= this->grid->width + 1; j++) // Loop for y-coordinates (with padding)
        for (int i = 2; i <= this->grid->height + 1; i++) // Loop for x-coordinates (with padding)
        {
            int m = (j - 2) * this->grid->width + (i - 2); // Calculate 1D index from 2D (adjusting for padding)
            File << int(this->grid->ingadients[m]) + 1 << "\n"; // Write material gradient (adding 1, possibly for FDM software)
        }

    File.close(); // Close the file
}