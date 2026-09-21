# ETMPEWPT (Electric, Thermal, Mechanical Properties Estimation With Percolation Theory) - 2D Version

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
8. [License](#license)
9. [Citation & Publications](#citation--publications)

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
