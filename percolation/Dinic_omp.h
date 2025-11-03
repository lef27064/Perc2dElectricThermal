#pragma once

#include <vector>
#include <limits>

class Dinicomp {
public:
    Dinicomp(int rows, int cols);
    ~Dinicomp();

    long long max_flow(const char* grid, long long capacity_val);

private:
    struct Edge {
        int to;
        long long capacity;
        int rev;
    };

    int num_rows;
    int num_cols;
    int num_nodes;
    int source;
    int sink;
    std::vector<std::vector<Edge>> graph;
    std::vector<int> level;
    std::vector<int> iter;
    const char* original_grid;
    long long capacity_val;

    // Helper functions for the algorithm
    int get_node_id(int r, int c, bool is_in_node);
    void add_edge(int from, int to, long long capacity);
    bool bfs();
    long long dfs(int v, long long f);

    // New routines for visualization
    char* create_visualization_data();
    void save_grid_to_bmp(const char* filename, const char* final_grid_vis);
};