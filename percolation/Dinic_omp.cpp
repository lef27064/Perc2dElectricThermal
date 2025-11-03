#include "Dinic_omp.h"
#include <iostream>
#include <queue>
#include <stack>
#include <string>
#include <cstdio>
#include <omp.h> // Περιλαμβάνουμε την βιβλιοθήκη του OpenMP

const long long INF = std::numeric_limits<long long>::max();

// Implementation of helper functions
Dinicomp::Dinicomp(int rows, int cols) {
    num_rows = rows;
    num_cols = cols;
    num_nodes = (num_rows * num_cols) * 2 + 2;
    source = num_nodes - 2;
    sink = num_nodes - 1;
    graph.assign(num_nodes, std::vector<Edge>());
}

Dinicomp::~Dinicomp() {}

int Dinicomp::get_node_id(int r, int c, bool is_in_node) {
    return (r * num_cols + c) * 2 + (is_in_node ? 0 : 1);
}

void Dinicomp::add_edge(int from, int to, long long capacity) {
    // Χρησιμοποιούμε critical section για να αποφύγουμε race conditions
    // κατά την πρόσβαση στο κοινόχρηστο γράφημα.
#pragma omp critical
    {
        graph[from].push_back({ to, capacity, (int)graph[to].size() });
        graph[to].push_back({ from, 0, (int)graph[from].size() - 1 });
    }
}

bool Dinicomp::bfs() {
    level.assign(num_nodes, -1);
    std::queue<int> q;
    q.push(source);
    level[source] = 0;

    while (!q.empty()) {
        int v = q.front();
        q.pop();
        for (const auto& edge : graph[v]) {
            if (edge.capacity > 0 && level[edge.to] < 0) {
                level[edge.to] = level[v] + 1;
                q.push(edge.to);
            }
        }
    }
    return level[sink] != -1;
}

long long Dinicomp::dfs(int v, long long f) {
    if (v == sink) return f;
    for (int& i = iter[v]; i < graph[v].size(); ++i) {
        Edge& e = graph[v][i];
        if (e.capacity > 0 && level[v] < level[e.to]) {
            long long d = dfs(e.to, std::min(f, e.capacity));
            if (d > 0) {
                e.capacity -= d;
                graph[e.to][e.rev].capacity += d;
                return d;
            }
        }
    }
    return 0;
}

// Main function to run the algorithm and call visualization
long long Dinicomp::max_flow(const char* grid, long long cap_val) {
    original_grid = grid;
    capacity_val = cap_val;

    // Παραλληλίζουμε την κατασκευή του γραφήματος
#pragma omp parallel for collapse(2)
    for (int r = 0; r < num_rows; ++r) {
        for (int c = 0; c < num_cols; ++c) {
            int node_in = get_node_id(r, c, true);
            int node_out = get_node_id(r, c, false);

            if (grid[r * num_cols + c] == '1') {
                add_edge(node_in, node_out, capacity_val);
            }

            int dr[] = { -1, 1, 0, 0 };
            int dc[] = { 0, 0, -1, 1 };

            for (int i = 0; i < 4; ++i) {
                int nr = r + dr[i];
                int nc = c + dc[i];
                if (nr >= 0 && nr < num_rows && nc >= 0 && nc < num_cols) {
                    int neighbor_in = get_node_id(nr, nc, true);
                    add_edge(node_out, neighbor_in, INF);
                }
            }
        }
    }

    // Προσθέτουμε τις ακμές από την πηγή και στον προορισμό
#pragma omp parallel for
    for (int c = 0; c < num_cols; ++c) {
        add_edge(source, get_node_id(0, c, true), INF);
    }
#pragma omp parallel for
    for (int c = 0; c < num_cols; ++c) {
        add_edge(get_node_id(num_rows - 1, c, false), sink, INF);
    }

    // Η κύρια ρουτίνα του Dinic παραμένει σειριακή
    long long flow = 0;
    while (bfs()) {
        iter.assign(num_nodes, 0);
        long long f;
        while ((f = dfs(source, INF)) > 0) {
            flow += f;
        }
    }

    // Call the new routine to create the visualization data
    char* final_grid_vis = create_visualization_data();
    // Call the routine to save the data to a BMP file
    save_grid_to_bmp("final_flow_grid.bmp", final_grid_vis);

    delete[] final_grid_vis;

    return flow;
}

// New routine to create the visualization data
char* Dinicomp::create_visualization_data() {
    char* final_grid_vis = new char[num_rows * num_cols];
    // Παραλληλίζουμε τη δημιουργία των δεδομένων οπτικοποίησης
#pragma omp parallel for collapse(2)
    for (int r = 0; r < num_rows; ++r) {
        for (int c = 0; c < num_cols; ++c) {
            int node_in = get_node_id(r, c, true);
            int node_out = get_node_id(r, c, false);
            bool is_path_node = false;

            for (const auto& edge : graph[node_out]) {
                if (edge.to == node_in && edge.capacity > 0) {
                    is_path_node = true;
                    break;
                }
            }

            if (is_path_node) {
                final_grid_vis[r * num_cols + c] = 2; // Path color
            }
            else if (original_grid[r * num_cols + c] == '1') {
                final_grid_vis[r * num_cols + c] = 1; // Wall color
            }
            else {
                final_grid_vis[r * num_cols + c] = 0; // Background color
            }
        }
    }
    return final_grid_vis;
}

// Routine to save the grid to a BMP file
void Dinicomp::save_grid_to_bmp(const char* filename, const char* final_grid_vis) {
    char file_header[14] = {
        'B', 'M', // ID
        0, 0, 0, 0, // Size
        0, 0, 0, 0, // Reserved
        54, 0, 0, 0 // Data offset
    };
    char info_header[40] = {
        40, 0, 0, 0, // Header size
        0, 0, 0, 0, // Width
        0, 0, 0, 0, // Height
        1, 0, // Planes
        24, 0, // Bits per pixel
        0, 0, 0, 0, // Compression method
        0, 0, 0, 0, // Image size
        0, 0, 0, 0, // Horizontal resolution
        0, 0, 0, 0, // Vertical resolution
        0, 0, 0, 0, // Colors in palette
        0, 0, 0, 0  // Important colors
    };

    int padding_size = (4 - (num_cols * 3) % 4) % 4;
    int data_size = num_cols * num_rows * 3 + padding_size * num_rows;
    int file_size = 54 + data_size;

    file_header[2] = (unsigned char)(file_size);
    file_header[3] = (unsigned char)(file_size >> 8);
    file_header[4] = (unsigned char)(file_size >> 16);
    file_header[5] = (unsigned char)(file_size >> 24);

    info_header[4] = (unsigned char)(num_cols);
    info_header[5] = (unsigned char)(num_cols >> 8);
    info_header[6] = (unsigned char)(num_cols >> 16);
    info_header[7] = (unsigned char)(num_cols >> 24);
    info_header[8] = (unsigned char)(num_rows);
    info_header[9] = (unsigned char)(num_rows >> 8);
    info_header[10] = (unsigned char)(num_rows >> 16);
    info_header[11] = (unsigned char)(num_rows >> 24);

    FILE* fp = fopen(filename, "wb");
    if (!fp) {
        std::cerr << "Error: Could not open file for writing." << std::endl;
        return;
    }

    fwrite(file_header, 1, 14, fp);
    fwrite(info_header, 1, 40, fp);

    for (int r = num_rows - 1; r >= 0; --r) {
        for (int c = 0; c < num_cols; ++c) {
            char pixel[3];
            char color_code = final_grid_vis[r * num_cols + c];
            if (color_code == 2) {
                // Bright Yellow color
                pixel[0] = 0;   // Blue
                pixel[1] = 255; // Green
                pixel[2] = 255; // Red
            }
            else if (color_code == 1) {
                // Black for 'walls'
                pixel[0] = 0;
                pixel[1] = 0;
                pixel[2] = 0;
            }
            else {
                // White for empty space
                pixel[0] = 255;
                pixel[1] = 255;
                pixel[2] = 255;
            }
            fwrite(pixel, 1, 3, fp);
        }
        for (int i = 0; i < padding_size; ++i) {
            fputc(0, fp);
        }
    }
    fclose(fp);
    std::cout << "Successfully saved grid to " << filename << std::endl;
}
