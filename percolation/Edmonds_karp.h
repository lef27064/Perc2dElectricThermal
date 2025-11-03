#pragma once
#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>

class Edmonds_karp {
private:
    int num_rows;
    int num_cols;
    int source;
    int sink;
    int num_nodes;

    // Εσωτερική συνάρτηση για τη μετατροπή συντεταγμένων σε ID κόμβου
    int get_node_id(int r, int c, bool is_in_node);
    // Δημιουργία του γραφήματος ροής
    std::vector<std::vector<int>> create_flow_network(const char* grid);

    // BFS για την εύρεση επαυξητικού μονοπατιού
    bool bfs(const std::vector<std::vector<int>>& capacity, const std::vector<std::vector<int>>& flow, std::vector<int>& parent);

public:
    // Συνάρτηση για την εύρεση μονοπατιών
    int find_paths(const char* grid, int rows, int cols, std::vector<std::vector<std::vector<int>>>& paths_coords);

};