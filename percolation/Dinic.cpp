#include "Dinic_omp.h"

class Dinic_omp {
private:
    struct Edge {
        int to;
        long long capacity;
        int rev; // index of the reverse edge
    };

    int num_rows, num_cols;
    int num_nodes;
    int source, sink;
    std::vector<std::vector<Edge>> graph;
    std::vector<int> level;
    std::vector<int> iter;

    int get_node_id(int r, int c, bool is_in_node) {
        return (r * num_cols + c) * 2 + (is_in_node ? 0 : 1);
    }

    void add_edge(int from, int to, long long capacity) {
        graph[from].push_back({ to, capacity, (int)graph[to].size() });
        graph[to].push_back({ from, 0, (int)graph[from].size() - 1 });
    }

    bool bfs() {
        level.assign(num_nodes, -1);
        std::queue<int> q;
        level[source] = 0;
        q.push(source);
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

    // A version of BFS that uses OpenMP
    bool bfs_omp() {
        level.assign(num_nodes, -1);
        std::vector<int> current_layer;
        current_layer.push_back(source);
        level[source] = 0;

        while (!current_layer.empty()) {
            std::vector<int> next_layer;
#pragma omp parallel for
            for (int v_idx = 0; v_idx < current_layer.size(); ++v_idx) {
                int v = current_layer[v_idx];
                for (const auto& edge : graph[v]) {
                    if (edge.capacity > 0 && level[edge.to] < 0) {
                        level[edge.to] = level[v] + 1;
#pragma omp critical
                        {
                            next_layer.push_back(edge.to);
                        }
                    }
                }
            }
            current_layer = next_layer;
        }
        return level[sink] != -1;
    }

    long long dfs(int v, long long f) {
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

public:
    Dinic_omp(int rows, int cols) {
        num_rows = rows;
        num_cols = cols;
        num_nodes = (num_rows * num_cols) * 2 + 2;
        source = num_nodes - 2;
        sink = num_nodes - 1;
        graph.assign(num_nodes, std::vector<Edge>());
    }

    long long max_flow(const char* grid, long long capacity_val) {
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

        for (int c = 0; c < num_cols; ++c) {
            add_edge(source, get_node_id(0, c, true), INF);
            add_edge(get_node_id(num_rows - 1, c, false), sink, INF);
        }

        long long flow = 0;
        while (bfs_omp()) {
            iter.assign(num_nodes, 0);
            long long f;
#pragma omp parallel
            {
#pragma omp single
                {
                    while ((f = dfs(source, INF)) > 0) {
                        flow += f;
                    }
                }
            }
        }
        return flow;
    }
};
