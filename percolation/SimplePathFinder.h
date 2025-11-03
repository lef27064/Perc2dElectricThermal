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

    // Εσωτερική βοήθεια συνάρτηση για την εύρεση μονοπατιού με BFS
    std::vector<std::pair<int, int>> find_path_bfs(
        std::vector<std::vector<bool>>& visited,
        int start_row, int end_row
    );
    std::vector<std::pair<int, int>> SimplePathFinder::find_path_dfs(
        std::vector<std::vector<bool>>& visited,
        int start_row, int end_row
    );

public:
    // Κατασκευαστής που δέχεται το πλέγμα, τις γραμμές και τις στήλες
    SimplePathFinder(char* input_grid, int num_rows, int num_cols);

    // Συνάρτηση που βρίσκει και αφαιρεί όλα τα μονοπάτια
    int find_all_paths();

    // Βοήθεια συνάρτηση για εκτύπωση του πλέγματος
    void print_grid();
};

