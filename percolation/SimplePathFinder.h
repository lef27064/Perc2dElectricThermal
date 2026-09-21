#pragma once

#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <utility>

class SimplePathFinder {
private:
    char* grid;
    int rows;
    int cols;

    // ÅóùôåñéêÞ âïÞèåéá óõíÜñôçóç ãéá ôçí åýñåóç ìïíïðáôéïý ìå BFS
    std::vector<std::pair<int, int>> find_path_bfs(
        std::vector<std::vector<bool>>& visited,
        int start_row, int end_row
    );
    std::vector<std::pair<int, int>> find_path_dfs(
        std::vector<std::vector<bool>>& visited,
        int start_row, int end_row
    );

public:
    // ÊáôáóêåõáóôÞò ðïõ äÝ÷åôáé ôï ðëÝãìá, ôéò ãñáììÝò êáé ôéò óôÞëåò
    SimplePathFinder(char* input_grid, int num_rows, int num_cols);

    // ÓõíÜñôçóç ðïõ âñßóêåé êáé áöáéñåß üëá ôá ìïíïðÜôéá
    int find_all_paths();

    // ÂïÞèåéá óõíÜñôçóç ãéá åêôýðùóç ôïõ ðëÝãìáôïò
    void print_grid();
};

