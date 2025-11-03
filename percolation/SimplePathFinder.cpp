#include "SimplePathFinder.h"
#include <tuple>

SimplePathFinder::SimplePathFinder(char* input_grid, int num_rows, int num_cols)
    : grid(input_grid), rows(num_rows), cols(num_cols) {
}

std::vector<std::pair<int, int>> SimplePathFinder::find_path_dfs(
    std::vector<std::vector<bool>>& visited,
    int start_row, int end_row
) {
    std::vector<std::pair<int, int>> path;
    std::stack<std::pair<int, int>> s; // Χρησιμοποιούμε stack αντί για queue
    std::vector<std::vector<std::pair<int, int>>> parent(rows, std::vector<std::pair<int, int>>(cols, { -1, -1 }));

    // Εύρεση σημείων εκκίνησης (πάνω γραμμή)
    for (int j = 0; j < cols; ++j) {
        if (grid[start_row * cols + j] == '1' && !visited[start_row][j]) {
            s.push({ start_row, j });
            visited[start_row][j] = true;
            parent[start_row][j] = { -2, -2 }; // Ειδική τιμή για την αρχή
        }
    }

    if (s.empty()) return path;

    bool path_found = false;
    std::pair<int, int> current_node;

    int dr[] = { 0, 1, 0, -1 }; // κίνηση πάνω, κάτω, αριστερά, δεξιά
    int dc[] = { -1, 0, 1, 0 };

    while (!s.empty()) {
        current_node = s.top();
        s.pop();

        int r = current_node.first;
        int c = current_node.second;

        // Έλεγχος αν φτάσαμε στον προορισμό (τελευταία γραμμή)
        if (r == end_row) {
            path_found = true;
            break;
        }

        for (int i = 0; i < 4; ++i) {
            int new_r = r + dr[i];
            int new_c = c + dc[i];

            if (new_r >= 0 && new_r < rows && new_c >= 0 && new_c < cols &&
                grid[new_r * cols + new_c] == '1' && !visited[new_r][new_c]) {
                s.push({ new_r, new_c });
                visited[new_r][new_c] = true;
                parent[new_r][new_c] = current_node;
            }
        }
    }

    // Αν βρέθηκε μονοπάτι, το ανακατασκευάζουμε και το αποθηκεύουμε
    if (path_found) {
        std::pair<int, int> p = current_node;
        while (p.first != -2) {
            path.push_back(p);
            // Σημειώνουμε το μονοπάτι με '2'
            grid[p.first * cols + p.second] = '2'; 
            p = parent[p.first][p.second];
        }
    }

    return path;
}


std::vector<std::pair<int, int>> SimplePathFinder::find_path_bfs(
    std::vector<std::vector<bool>>& visited,
    int start_row, int end_row
) {
    std::vector<std::pair<int, int>> path;
    std::queue<std::pair<int, int>> q;
    std::vector<std::vector<std::pair<int, int>>> parent(rows, std::vector<std::pair<int, int>>(cols, { -1, -1 }));

    // Εύρεση σημείων εκκίνησης (πάνω γραμμή)
    for (int j = 0; j < cols; ++j) {
        if (grid[start_row * cols + j] == '1' && !visited[start_row][j]) {
            q.push({ start_row, j });
            visited[start_row][j] = true;
            parent[start_row][j] = { -2, -2 }; // Ειδική τιμή για την αρχή
        }
    }

    if (q.empty()) return path;

    bool path_found = false;
    std::pair<int, int> current_node;

   
    int dr[] = { 0, 1, 0, -1 }; // κίνηση πάνω, κάτω, αριστερά, δεξιά
    int dc[] = { -1, 0, 1, 0 };
    // int xNum[] = { -1, 0 ,1,  0 };
    // The y-coordinate offsets for checking neighbors.
    //int yNum[] = { 0,  1 ,0, -1 };

    

    while (!q.empty()) {
        current_node = q.front();
        q.pop();

        int r = current_node.first;
        int c = current_node.second;

        // Έλεγχος αν φτάσαμε στον προορισμό (τελευταία γραμμή)
        if (r == end_row) {
            path_found = true;
            break;
        }

        for (int i = 0; i < 4; ++i) {
            int new_r = r + dr[i];
            int new_c = c + dc[i];

            if (new_r >= 0 && new_r < rows && new_c >= 0 && new_c < cols &&
                grid[new_r * cols + new_c] == '1' && !visited[new_r][new_c]) {
                q.push({ new_r, new_c });
                visited[new_r][new_c] = true;
                parent[new_r][new_c] = current_node;
            }
        }
    }

    // Αν βρέθηκε μονοπάτι, το ανακατασκευάζουμε και το αποθηκεύουμε
    if (path_found) {
        std::pair<int, int> p = current_node;
        while (p.first != -2) {
            path.push_back(p);

            // Mark the current node *before* moving to its parent
            grid[p.first * cols + p.second] = '2';

            // Now, update p to its parent for the next iteration
            p = parent[p.first][p.second];
        }
    }

    return path;
}

int SimplePathFinder::find_all_paths() {
    int path_count = 0;
    std::vector<std::vector<bool>> visited(rows, std::vector<bool>(cols, false));
    std::vector<std::pair<int, int>> current_path;

    do {
        // Επαναφέρουμε τον πίνακα visited για κάθε νέα αναζήτηση
        std::vector<std::vector<bool>> new_visited(rows, std::vector<bool>(cols, false));
        current_path = find_path_dfs(new_visited, 0, rows - 1);

        if (!current_path.empty()) {
            path_count++;
            // Αφαίρεση του μονοπατιού από το grid
            for (const auto& node : current_path) {
                grid[node.first * cols + node.second] = '0'; // Σημειώνουμε ως εμπόδιο για την επόμενη αναζήτηση
            }
        }
    } while (!current_path.empty());

    return path_count;
}

void SimplePathFinder::print_grid() {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            std::cout << grid[i * cols + j] << " ";
        }
        std::cout << std::endl;
    }
}