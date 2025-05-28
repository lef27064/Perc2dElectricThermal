# ETMPEWPT (Electric, Thermal, Mechanical Properties Estimation With Percolation Theory) - 2D Version

## Project Description

ETMPEWPT is an open-source software developed for calculating the electrical, thermal, and mechanical properties of 2D composite materials, utilizing percolation theory and cluster analysis. The program simulates microstructures through a Monte Carlo approach and calculates macroscopic properties based on the geometry and properties of the individual components.

## 1. System Requirements

For ETMPEWPT to run smoothly, ensure your system meets the following requirements:

* **Operating System:** Windows 10/11 (other OS are supported, but the executable is provided for Windows).
* **Processor:** x64 compatible processor. The program is single-threaded, so core frequency is more important than core count.
* **RAM:** Highly dependent on the grid size defined in the input file.
    * For a 1000x1000 grid: Approximately 2 GB RAM.
    * For a 5000x5000 grid: Approximately 8 GB RAM.
    * For a 10000x10000 grid: Approximately 16 GB RAM.
* **Disk Space:** Sufficient space for storing output files, especially if you choose to save images or `shapes` files for each realization.
* **DirectX:** For certain visualization functions, DirectX compatibility may be required.

## 2. Quick Start

To get started quickly with ETMPEWPT:

1.  **Download the executable:** Obtain the `ETMPEWPT.exe` (or the equivalent executable for your operating system) from the [Releases](link_to_your_releases_page) section of the repository.
2.  **Create folder structure:** In the same folder as the executable, create the following subfolders:
    * `inputs/`
    * `outputs/`
3.  **Settings File (`settings.txt`):** Place the `settings.txt` file in the same folder as the executable. An example `settings.txt` is:
    ```
    # Number of iterations for the simulation
    Total Iterations to Run : 300
    # Save Image Files (TRUE/FALSE)
    Save image file(s) : TRUE
    # How many random images to save if "Save image file(s)" is TRUE
    Save random image(s) : 3
    # If a specific image file needs to be saved (TRUE/FALSE)
    Save image(s) for specific iteration(s) : FALSE
    # The specific iterations to save (comma-separated, e.g., 1,50,100)
    Iterations to save : 1, 50, 100
    # The current directory for images and shapes files
    Current directory : "C:/Users/User/Desktop/ETMPEWPT/outputs/"
    ```
    * **`Total Iterations to Run`**: The total number of iterations to run for each input problem. If this number is less than the iterations defined in the input file, it will be ignored. If it is greater, it will be supplemented by the iterations from the input file.
    * **`Save image file(s)`**: If set to `TRUE`, the program will save images of the microstructures.
    * **`Save random image(s)`**: If `Save image file(s)` is `TRUE`, this specifies how many random microstructure images to save.
    * **`Save image(s) for specific iteration(s)`**: If set to `TRUE`, you can specify particular iterations for which you want images to be saved.
    * **`Iterations to save`**: The specific iteration numbers for saving images, if `Save image(s) for specific iteration(s)` is `TRUE`.
    * **`Current directory`**: The directory where output folders for each simulation will be created.
4.  **Input File (`.txt`):** Create a `.txt` file with your simulation parameters (see Section 4) and place it in the `inputs/` subfolder.
5.  **Run the program:** Double-click `ETMPEWPT.exe`. Results will be saved in the `outputs/` subfolder, within a new directory named after the project you specified.

## 3. Compilation

ETMPEWPT is developed in C++17 and requires a compiler that supports this standard (e.g., MinGW, GNU g++, or Visual Studio).

For compilation with **Visual Studio**:
1.  Open the project in Visual Studio.
2.  Ensure the compiler is set to C++17.
3.  Execute the "Build Solution" command.
4.  The executable file will be generated in the `x64/Release/` or `x64/Debug/` directory of the project.

For compilation with **GNU g++ (MinGW/Linux)**:
Open a terminal in the directory containing the source code files and execute a command similar to the following:
```bash
wget https://github.com/lef27064/Perc2dElectricThermal/archive/refs/heads/master.zip -O Perc2d.zip
unzip Perc2d.zip
cd Perc2dElectricThermal-master/Percolation
g++ main.cpp cluster.cpp BatchMonteCarlo.cpp Settings.cpp Grid.cpp Shapes.cpp image.cpp FD2DEL.cpp ShapeGenerator.cpp general.cpp -o Perc2d -fopenmp -std=c++17
```
-std=c++17: Specifies the C++17 standard.
-O3: Optimization level (recommended for performance).
-o Perc2d: The name of the executable file.
-main.cpp cluster.cpp BatchMonteCarlo.cpp Settings.cpp Grid.cpp Shapes.cpp image.cpp FD2DEL.cpp ShapeGenerator.cpp general.cpp: The project's source code files.
-fopenmp use of omp library for parallel execution

4. Input File (.txt)
The ETMPEWPT program reads simulation parameters from a text file with a .txt extension. These files must be placed in the inputs/ subdirectory, which should be located in the same folder as the program's executable.

Each input file contains a series of parameters, with one parameter per line. Lines beginning with the # symbol are treated as comments and are ignored by the program.

Below is a detailed description of each parameter based on a provided example:
```
#name
example
#total component
3
#components % weights
0.4 0.3 0.3
#Components type Geometry : RECTANGLE = 0, SLOPEDRECTANGLE = 1, ELLIPSE = 3, ΝΟΝΕ = 4 (disolver)
4 3 1
#SizeType CONSTANT = 0 (Constant Size), VARIABLE = 1, (Variable Size), NONE = 3 None(disolver)
3 0 0
#Special Weights
1.6532 1.327999999 1.342
#Dimension X
10.0e-09 10.0e-09 1.0e-09 
#Dimension Y
10.0e-09 10.0e-09 10.0e-09 
#hoops
.5e-09 .5e-09 .5e-09
#Electric Conductivities
1.e-16 1.0 0.5
#Thermal Conductivities
1.e-16 1.0 0.5
#Young Modulus
1.e+5 1.e+10 0.5e+9
#Poisson Ratio
0.3 0.32 0.31
#width - height
1000 1000
#iterations
200
#pixels per min Sphere
101
#Swiss chesse case TRUE or FALSE
FALSE
# CALC ELECTRIC CONDUCTIVITY
TRUE
# CALC ELECTRIC CONDUCTIVITY WITH FINITE DIFFERENCES
false
# CALCULATE STATISTICS
FALSE
Detailed Parameter Description
#name
```
Description: The name assigned to the current simulation series (project name). This name will be used to create the output directory where results will be stored.
Data Type: String.
Possible Values: Any valid string suitable for a folder name.
#total components

Description: The total number of different components (materials or phases) that will be part of the composite material's composition.
Data Type: Integer.
Possible Values: Positive integer (e.g., 2, 3, etc.).
#components % weights

Description: The percentage composition (in decimal form) by weight for each component participating in the material's composition. The number of values must correspond to #total components. The sum of the values should be 1.0.
Data Type: Double.
Possible Values: Positive decimal numbers (e.g., 0.2, 0.33, etc.) whose sum equals 1.0.
#Components type Geometry

Description: The geometry type for each component. The number of values must correspond to #total components.
Data Type: Integer.
Possible Values:
0: RECTANGLE
1: SLOPEDRECTANGLE
3: ELLIPSE (Ellipse / Circle)
4: NONE (Dissolver / Matrix, does not represent particles)
#SizeType CONSTANT = 0 (Constant Size), VARIABLE = 1, (Variable Size), NONE = 3 None(disolver)

Description: Determines whether the particle size for each component is constant or variable. The number of values must correspond to #total components.
Data Type: Integer.
Possible Values:
0: CONSTANT (Constant Size)
1: VARIABLE (Variable Size, which is determined by internal program mechanisms).
3: NONE (For a component that acts as a dissolver/matrix and has no specific size).
#Special Weights

Description: Special weights applied to each component. The number of values must correspond to #total components.
Data Type: Float.
Possible Values: Positive decimal numbers (e.g., 0.99, 1.23, etc.).
#Dimension X

Description: The X-dimension (width) of an elementary particle for each component, in meters. The order of values corresponds to the components (first value for the first component, and so on).
Data Type: Float.
Possible Values: Positive decimal numbers (e.g., 10.0e-9, 1.0e-9).
#Dimension Y

Description: The Y-dimension (height) of an elementary particle for each component, in meters. The order of values corresponds to the components (first value for the first component, and so on).
Data Type: Float.
Possible Values: Positive decimal numbers (e.g., 10.0e-9, 1.0e-9).
#hoops

Description: The "hoop" distance in meters for each component, which is considered "percolated" if the distance between two particles is less than this value. The order of values corresponds to the components.
Data Type: Float.
Possible Values: Positive decimal numbers (e.5e-9).
#Electric Conductivities

Description: The electric conductivity for each component. The order of values corresponds to the components.
Data Type: Float.
Possible Values: Positive decimal numbers (e.g., 1.e-16, 1.0).
#Thermal Conductivities

Description: The thermal conductivity for each component. The order of values corresponds to the components.
Data Type: Float.
Possible Values: Positive decimal numbers (e.g., 1.e-16, 1.0).
#Young Modulus

Description: The Young's modulus for each component. The order of values corresponds to the components.
Data Type: Float.
Possible Values: Positive decimal numbers (e.g., 1.e+5, 1.e+10).
#Poisson Ratio

Description: The Poisson's ratio for each component. The order of values corresponds to the components.
Data Type: Float.
Possible Values: Decimal numbers (e.g., 0.3, 0.32).
#width - height

Description: The dimensions (width and height) of the grid in pixels.
Data Type: Integer.
Possible Values: Positive integers, typically ranging from 5 to 10,000, depending on the available system memory.
#iterations

Description: The total number of iterations (realizations) for the Monte Carlo sampling. Each iteration generates a new random microstructure.
Data Type: Integer.
Possible Values: Positive integer (e.g., 200, 1000).
#pixels per min Sphere

Description: The number of pixels corresponding to the minimum sphere (or minimum characteristic size) on the grid. This parameter affects the resolution of particle digitization.
Data Type: Integer.
Possible Values: Positive integer (e.g., 101).
#Swiss chesse case TRUE or FALSE

Description: Determines if the simulation will treat the problem as a "Swiss cheese case" (i.e., the inverse problem where conductive "pores" are created within a non-conductive matrix).
Data Type: Boolean.
Possible Values: TRUE or FALSE.
#CALC ELECTRIC CONDUCTIVITY

Description: Determines whether electric conductivity will be calculated. If TRUE, the program will perform calculations for electric conductivity.
Data Type: Boolean.
Possible Values: TRUE or FALSE.
#CALC ELECTRIC CONDUCTIVITY WITH FINITE DIFFERENCES

Description: Determines whether electric conductivity will also be calculated using the Finite Differences Method (FDM).
Note: The FDM code used is based on a publication by Edward Garboczi. This code has certain speed limitations and is not recommended for very large problems.
Data Type: Boolean.
Possible Values: TRUE or FALSE.
#CALCULATE STATISTICS

Description: Determines whether statistical parameters of the clusters (e.g., mean cluster radius, inertia, etc.) will be calculated and saved.
Data Type: Boolean.
Possible Values: TRUE or FALSE.
5. Understanding the Results
For each simulation executed by ETMPEWPT, a separate directory is created (named after the project defined in the input file) where the results are stored.

Output Files per Project
Within the project directory, you can find the following files and subdirectories:

report.csv: The primary report file, containing comma-separated values. It includes detailed data for each simulation realization, as well as summary statistics at the end.
ReportWithSemicolon.csv: Identical to report.csv, but with values separated by semicolons (;) for compatibility with certain software.
Statistics.csv: This file is created only if the CALCULATE STATISTICS parameter is set to TRUE in the input file. It contains statistical data related to the generated clusters for each realization.
microstructure.txt: An output file representing the composite material's microstructure, suitable for use in Finite Difference Method (FDM) simulations, if the relevant option is enabled.
images/ (Subdirectory): If the Save image file(s) setting in settings.txt is TRUE, this subdirectory will contain images of the random grid realizations, depending on the settings.txt configurations (e.g., whether to save random or specific realizations).
shapes/ (Subdirectory): This subdirectory stores the component (shapes) data for each realization. The data is in tabular form, describing the position and dimensions of each particle, allowing for geometry reconstruction if needed.
report.csv File Structure
The report.csv file begins with header information and the input parameters, followed by the detailed results for each realization.

Example Header and Data Table (report.csv):

Απόσπασμα κώδικα

Electric,Thermal, Mechanical Properties Estimation With Percolation Theory (ETMPEWPT) (2D version)
Version 2.0 Percolation, Cluster Analysis,  Electric - Thermal - Mechanical properties
Report starting: ,Thu May 22 09:31:53 2025
---------------------------------------------------------------------------------------------------
Inputs
     |#, % Weights, % Spec.Weights, X Dimension, Y Dimension, Type, Size type,        Area,      hoop,
       0,      0.53,             1,        1e-08,        1e-08,    4,         3,        0.53,         0
       1,      0.47,             1,        1e-08,        1e-08,    2,         0,        0.47,         0
Pixels Per minimum Size (ppms):,101
Factor [ppms / minimumSize]:,1.01e+10
Maximum Dimension:,1e-08
Minimum Dimension:,1e-08
Grid size,300,x,300
Total iterations: ,300
---------------------------------------------------------------------------------------------------
Realization No,PercolateY[1]/N[0],Process Time,Setup Time              ,% Real Area Comp[0]            ,% Real Area Comp[1],Electric Conductivity,Thermal Conductivity,Young Modulus,Poisson Ratio,Total Conductive Paths,Mean Real Path Length,Current Electric Conductivity
      1,                   1,      0.003,          0.002            ,0.502967             ,0.497033,115.71233808994294634,115.71233808994294634,5823086.67912218533456326,0.30027136259208237,1.00000000000000000,471.00000000000000000,0.00002880706922318
      2,                   0,0.00200000000000000,0.00100000000000000            ,0.52548888888888889             ,0.47451111111111111,0.00000000000000000,0.00000000000000000,100000.00000000000000000,0.29999999999999999,0.00000000000000000,0.00000000000000000,0.00000000000000000
shapes/ File Structure
Each file within the shapes/ subdirectory describes the characteristics of individual particles for a specific realization.

Example shapes/ file:

Component 1 maximum area=0.47 [on 300x300]
Component type: Circle
---------------------------------------------------------------------------------------
--------------|------- Center-----|----- dimensions---------|----------------------
component| id|     X         |     Y         |  Width/a      | height/b |   hoop   |   Slope  
---------------------------------------------------------------------------------------
           1,    1,         47,           5,        50.5,        50.5,          0  ,          0
           1,    2,          8,         154,        50.5,        50.5,          0  ,          0
           1,    3,        263,         206,        50.5,        50.5,          0  ,          0
           1,    4,         13,         265,        50.5,        50.5,          0  ,          0
           1,    5,        181,         276,        50.5,        50.5,          0  ,          0
           1,    6,        175,         206,        50.5,        50.5,          0  ,          0
           1,    7,        214,         180,        50.5,        50.5,          0  ,          0
           1,    8,         293,          95,        50.5,        50.5,          0  ,          0
           1,    9,         41,         156,        50.5,        50.5,          0  ,          0
------------------------------------------------------------------------------
Component[1] Total Spheres = 9
Summary Report (Main Directory)
If multiple problems are executed sequentially (via multiple input files in the inputs directory), a summary report file is generated in the main directory (where the executable is located). This file aggregates key results from all executed simulations for a quick overview.

Example Summary Report:

Απόσπασμα κώδικα

-------------------------------------------------------------------------------------------------------------------------
Electric,Thermal, Mechanical Properties Estimation With Percolation Theory (ETMPEWPT) (2D version) 
Version 2.0 Percolation, Cluster Analysis,  Electric - Thermal - Mechanical properties
Last update: 1/7/2024
Author:Eleftherios Lamprou
Licence:This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License.
Developed during PhD Thesis (2017-2024)
-------------------------------------------------------------------------------------------------------------------------
Start at Fri May 23 12:54:48 2025
Case,Mean Percolation Probability,Electric Conductivity,Thermal Conductivity,Young Modulus,Poisson Ratio,Total Conductive Paths,Mean Conductive Length,Log Electric Conductivity,Log Thermal Conductivity,Log Young Modulus,Log Poisson Ratio,Log Total Conductive Paths,Log Mean Conductive Length,Process Time,Preperation Time,Grid(X),Grid(Y),ppms,% Area - 0,% Area - 1,% Area - 2,% Real Area - 0,% Real Area - 1,% Real Area - 2,Size X -0,Size X -1,Size X -2,Size Y -0,Size Y -1,Size Y -2,Hoop Size - 0,Hoop Size - 1,Hoop Size - 2,
ellipse_1iter300ppmC_101width_500_Area0.6,1,0.0446882,0.0446882,2.93745e+07,0.301419,88.71,1591.6,-3.10804,-3.10804,17.1956,-1.19926,4.48537,7.3725,0.13424,0.06885,1000,1000,101,0.349947,0.326731,0.323322,0.348029,0.328438,0.323533,1e-08,1e-08,1e-09,1e-08,1e-08,1e-08,5e-10,5e-10,5e-10
6. License
ETMPEWPT is distributed as open-source software and is licensed under the GNU General Public License (GPL) Version 3.

This means you are free to:

Use the program for any purpose.
Study how the program works and adapt the code to your needs.
Redistribute copies of the program.
Improve the program and release your improvements to the public, so that the whole community benefits.
For more details on the GPL v3 license, please refer to the official GNU GPL v3 documentation.

7. Troubleshooting
If you encounter issues while using ETMPEWPT, please consider the following:

System Requirements: Ensure your system meets the minimum RAM and processor requirements, especially for large grids.
Compilation: Verify that you are using a C++17 compliant compiler and that compilation completed without errors. For Visual Studio users, ensure the project is configured correctly.
File Structure: Double-check the file and directory structure. settings.txt must be next to the executable, and input files in the inputs/ subdirectory.
Input File: Carefully review your input .txt file for any syntax errors, missing values, or invalid parameter values. Ensure that the sum of components % weights equals 1.0.
Error Messages: Note any error messages displayed in the console. These can provide significant clues about the cause of the problem.
8. Contact and Support
For any questions, bug reports, or ideas for improving ETMPEWPT, you can contact the author via email at:

lef27064@otenet.gr

Note: ETMPEWPT was developed as part of a PhD thesis. Due to the author's professional and family commitments, maintaining the project at a rapid pace may not be feasible. However, all reports and suggestions are highly valued and will be taken into consideration.

9. Credits
Author and Lead Developer: Eleftherios Lamprou
Documentation Support / Authoring (via AI): Gemini (Google)
Theory: The theory behind this project is published in two papers:
E. Lambrou and L. N. Gergidis, “A computational method for calculating the electrical and thermal properties of random composite”, Physica A: Statistical Mechanics and its Applications, Volume 642, 2024, 129760, ISSN 0378-4371, https://doi.org/10.1016/j.physa.2024.129760
E. Lambrou and L. N. Gergidis, “A particle digitization-based computational method for continuum percolation,” Physica A: Statistical Mechanics and its Applications, vol. 590, p. 126738, 2022
Code References: (As mentioned in the Finite Differences section) Edward Garboczi (Garboczi, E. (1998). Finite Element and Finite Difference Programs for Computing the Linear Electric and Elastic Properties of Digital Images of Random Materials. NIST Interagency/Internal Report (NISTIR), National Institute of Standards and Technology, Gaithersburg, MD, url="https://tsapps.nist.gov/publication/get_pdf.cfm?pub_id=860168")