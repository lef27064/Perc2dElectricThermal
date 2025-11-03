#include "Edmonds_karp.h"

    // Εσωτερική συνάρτηση για τη μετατροπή συντεταγμένων σε ID κόμβου
    int Edmonds_karp::get_node_id(int r, int c, bool is_in_node) {
        return (r * num_cols + c) * 2 + (is_in_node ? 0 : 1);
    }

    // Δημιουργία του γραφήματος ροής
    std::vector<std::vector<int>> Edmonds_karp::create_flow_network(const char* grid) {
        num_nodes = (num_rows * num_cols) * 2 + 2;
        source = num_nodes - 2;
        sink = num_nodes - 1;

        std::vector<std::vector<int>> capacity(num_nodes, std::vector<int>(num_nodes, 0));

        // Κόμβοι μέσα στο πλέγμα
        for (int r = 0; r < num_rows; ++r) {
            for (int c = 0; c < num_cols; ++c) {
                int node_in = get_node_id(r, c, true);
                int node_out = get_node_id(r, c, false);

                if (grid[r * num_cols + c] == '1') {
                    capacity[node_in][node_out] = 1;
                }

                int dr[] = { -1, 1, 0, 0 };
                int dc[] = { 0, 0, -1, 1 };

                for (int i = 0; i < 4; ++i) {
                    int nr = r + dr[i];
                    int nc = c + dc[i];
                    if (nr >= 0 && nr < num_rows && nc >= 0 && nc < num_cols) {
                        int neighbor_in = get_node_id(nr, nc, true);
                        capacity[node_out][neighbor_in] = 1;
                    }
                }
            }
        }

        // Συνδέσεις από την πηγή και προς τον προορισμό
        for (int c = 0; c < num_cols; ++c) {
            capacity[source][get_node_id(0, c, true)] = 1;
            capacity[get_node_id(num_rows - 1, c, false)][sink] = 1;
        }

        return capacity;
    }

    // BFS για την εύρεση επαυξητικού μονοπατιού
    bool Edmonds_karp::bfs(const std::vector<std::vector<int>>& capacity, const std::vector<std::vector<int>>& flow, std::vector<int>& parent) {
        parent.assign(num_nodes, -1);
        std::queue<int> q;
        q.push(source);
        parent[source] = source;

        while (!q.empty()) {
            int u = q.front();
            q.pop();

            if (u == sink) {
                return true;
            }

            for (int v = 0; v < num_nodes; ++v) {
                if (parent[v] == -1 && capacity[u][v] - flow[u][v] > 0) {
                    parent[v] = u;
                    q.push(v);
                }
            }
        }
        return false;
    }


    // Συνάρτηση για την εύρεση μονοπατιών
    int Edmonds_karp::find_paths(const char* grid, int rows, int cols, std::vector<std::vector<std::vector<int>>>& paths_coords) {
        num_rows = rows;
        num_cols = cols;

        std::vector<std::vector<int>> capacity = create_flow_network(grid);
        std::vector<std::vector<int>> flow(num_nodes, std::vector<int>(num_nodes, 0));
        int max_flow = 0;

        std::vector<int> parent;
        while (bfs(capacity, flow, parent)) {
            int path_flow = 1;
            max_flow += path_flow;

            std::vector<std::vector<int>> current_path_coords;
            int v = sink;
            while (v != source) {
                int u = parent[v];

                flow[u][v] += path_flow;
                flow[v][u] -= path_flow;

                if (v != sink && v % 2 != 0) { // Μόνο οι out_nodes, που αντιστοιχούν σε κελιά του πλέγματος
                    int r = (v / 2) / num_cols;
                    int c = (v / 2) % num_cols;
                    current_path_coords.push_back({ r, c });
                }
                v = u;
            }

            std::reverse(current_path_coords.begin(), current_path_coords.end());
            paths_coords.push_back(current_path_coords);
        }

        return max_flow;
    }