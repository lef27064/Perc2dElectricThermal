# ETMPEWPT (Electric, Thermal, Mechanical Properties Estimation With Percolation Theory) - 2D Version

[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.22869715.svg)](https://doi.org/10.5281/zenodo.22869715)
[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![C++ Standard](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://en.cppreference.com/w/cpp/17)
[![OpenMP](https://img.shields.io/badge/Parallel-OpenMP-orange.svg)](https://www.openmp.org/)

## Overview
**ETMPEWPT** is an open-source, high-performance C++ simulation framework designed to estimate the effective electrical, thermal, and mechanical properties of 2D composite microstructures using **continuum percolation theory** and **particle digitization**. 

The program generates representative volume elements (RVEs) through Monte Carlo sampling, analyzes cluster topology (DFS / Hoshen-Kopelman), and evaluates macroscopic transport and mechanical properties based on individual constituent parameters and spatial distribution.

---

## Table of Contents
1. [Key Features](#key-features)
2. [System Requirements](#system-requirements)
3. [Quick Start & Directory Layout](#quick-start--directory-layout)
4. [Compilation](#compilation)
5. [Input File Specification](#input-file-specification)
6. [Global Settings Configuration](#global-settings-configuration)
7. [Understanding Output Results](#understanding-output-results)
8. [Quick Start with Google Colab](#quick-start-with-google-colab)
9. [License](#license)
10. [Citation & Publications](#citation--publications)

---

## Key Features
- **Flexible 2D Inclusion Geometries:** Supports Rectangles, Sloped Rectangles, Ellipses, and Matrix Phase domains.
- **Multi-Property Solver:** Calculates effective electrical conductivity ($\sigma_{eff}$), thermal conductivity ($k_{eff}$), Young's Modulus ($E$), and Poisson's Ratio ($\nu$).
- **Numerical Methods:** Supports both fast cluster-percolation analysis and Finite Difference (FD2D) methods.
- **High-Resolution Grids:** Optimized for large-scale microstructures (e.g., $1000 \times 1000$ to $10000 \times 10000$ pixels).
- **Parallel Processing:** Accelerated via OpenMP multi-threading for rapid Monte Carlo convergence (200–300 realizations).
- **Visual & Data Export:** Saves bitmap (BMP) microstructures and particle coordinate files (`shapes/`) for visual verification.

---

## System Requirements
- **Operating System:** Windows 10/11, Linux, or macOS.
- **Compiler:** C++17 compliant compiler (GCC 8+, Clang 10+, or MSVC 2019+).
- **RAM Footprint:**
  - $1000 \times 1000$ grid: ~2 GB RAM
  - $5000 \times 5000$ grid: ~8 GB RAM
  - $10000 \times 10000$ grid: ~16 GB RAM

---

## Quick Start & Directory Layout

Ensure the executable is placed in a directory with the following structure:

```text
├── ETMPEWPT.exe
├── settings.txt
├── inputs/
│   └── example_2d.txt
└── outputs/
```

---
Place your configuration files in the inputs/ directory.

Edit settings.txt to define output paths and realization limits.

Run ETMPEWPT.exe.

## Compilation
### Linux / macOS (GCC / Clang)
```text
git clone [https://github.com/lef27064/Perc2dElectricThermal.git](https://github.com/lef27064/Perc2dElectricThermal.git)
cd Perc2dElectricThermal/Percolation
g++ main.cpp cluster.cpp BatchMonteCarlo.cpp Settings.cpp Grid.cpp Shapes.cpp image.cpp FD2DEL.cpp ShapeGenerator.cpp general.cpp -o ETMPEWPT -fopenmp -std=c++17 -O3
```

### Windows (Visual Studio)
Open the project solution in Visual Studio.

Set C++ Language Standard to C++17 (/std:c++17).

Enable OpenMP Support (/openmp) in C/C++ Project Properties.

Build in Release - x64 mode.
---

## Input File Specification
Input parameters are defined via text files inside inputs/.

Example Input (inputs/example_2d.txt):
```text
#name
example_run
#total component
3
#components % weights
0.4 0.3 0.3
#Components type Geometry : RECTANGLE = 0, SLOPEDRECTANGLE = 1, ELLIPSE = 3, NONE = 4
4 3 1
#SizeType CONSTANT = 0, VARIABLE = 1, NONE = 3
3 0 0
#Special Weights
1.6532 1.3279 1.342
#Dimension X
10.0e-09 10.0e-09 1.0e-09 
#Dimension Y
10.0e-09 10.0e-09 10.0e-09 
#hoops
0.5e-09 0.5e-09 0.5e-09
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
FALSE
# CALCULATE STATISTICS
FALSE
```
---

## Global Settings Configuration
Global execution directives are defined in settings.txt:
```text
# Number of iterations for the simulation
Total Iterations to Run : 300
# Save Image Files (TRUE/FALSE)
Save image file(s) : TRUE
# How many random images to save
Save random image(s) : 3
# Save specific iterations (TRUE/FALSE)
Save image(s) for specific iteration(s) : FALSE
Iterations to save : 1, 50, 100
# Output Directory
Current directory : "./outputs/"
```
---

## Understanding Output Results
All results are automatically categorized under outputs/<project_name>/:  
report.csv:Realization-by-realization data and ensemble averages ($\sigma_{eff}$, $k_{eff}$, percolation state).  
ReportWithSemicolon.csv: CSV formatted with semicolon separators for European regional Excel settings.  
Statistics.csv: Cluster size distributions and critical percolation path data.images/:  
Exported 2D BMP microstructures for visual inspection.shapes/: Coordinates and orientation data for reconstructed inclusion geometries.

---

## Quick Start with Google Colab

You can run the full 2D Continuum Percolation simulation directly in Google Colab using our provided notebook:

- 📓 **[Open Notebook in Colab] [https://colab.research.google.com/github/lef27064/Perc2dElectricThermal/blob/master/percolation/ETMPEWPT_QuickStart.ipynb](https://colab.research.google.com/github/lef27064/Perc2dElectricThermal/blob/master/percolation/ETMPEWPT_QuickStart.ipynb)** 

The notebook handles:
- Environment cleanup and workspace setup
- Automatic repository cloning & C++17 compilation with OpenMP support
- Input file generation (`settings.txt` & `inputs/example_2d.txt`)
- Execution of the `ETMPEWPT` binary

---

## License
Distributed under the GNU General Public License v3.0 (GPL-3.0).
See LICENSE for details.

---

## Citation & Publications
Author: Eleftherios Lambrou  
Contact: lef27064@otenet.gr  
If you use ETMPEWPT in your research, please cite the following theoretical papers:

1. E. Lambrou and L. N. Gergidis, "A computational method for calculating the electrical and thermal properties of random composite", Physica A: Statistical Mechanics and its Applications, vol. 642, p. 129760, 2024. DOI: 10.1016/j.physa.2024.129760E.

2. Lambrou and L. N. Gergidis, "A particle digitization-based computational method for continuum percolation", Physica A: Statistical Mechanics and its Applications, vol. 590, p. 126738, 2022. DOI: 10.1016/j.physa.2021.126738

---
