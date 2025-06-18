#include <iostream>
#include <vector>
#include <utility> // for std::pair
#include <iomanip> // For std::fixed and std::setprecision
#include <set>     // For std::set to check uniqueness
#include <vector>  // std::vector is already included but good practice

using namespace std;

const int GRID_SIZE = 6;
const int TARGET_COUNT = 18; // Updated target count based on 6 rows/cols * 3 selections each

// 辅助函数：检查在 (r, c) 放置棋子后是否形成三连
// 只需要检查新放置点相关的行和列即可
bool check(int r, int c, const vector<vector<int>>& board) {
    // 检查行
    for (int j = 0; j <= GRID_SIZE - 3; ++j) {
        if (board[r][j] == 0 && board[r][j+1] == 0 && board[r][j+2] == 0) {
            return false; // 发现行三连
        }
    }
    // 检查列
    for (int i = 0; i <= GRID_SIZE - 3; ++i) {
        if (board[i][c] == 0 && board[i+1][c] == 0 && board[i+2][c] == 0) {
            return false; // 发现列三连
        }
    }
    return true; // 没有发现三连
}
// 新增函数：检查行和列的选中模式是否唯一
bool checkRowColUniqueness(const vector<vector<int>>& board) {
    set<vector<int>> row_patterns;
    for (int r = 0; r < GRID_SIZE; ++r) {
        // 直接使用行向量作为模式
        row_patterns.insert(board[r]);
    }
    if (row_patterns.size() != GRID_SIZE) {
        return false; // 存在重复的行模式
    }

    set<vector<int>> col_patterns;
    for (int c = 0; c < GRID_SIZE; ++c) {
        vector<int> current_col_pattern;
        current_col_pattern.reserve(GRID_SIZE); // 预分配空间
        for (int r = 0; r < GRID_SIZE; ++r) {
            current_col_pattern.push_back(board[r][c]);
        }
        col_patterns.insert(current_col_pattern);
    }
    if (col_patterns.size() != GRID_SIZE) {
        return false; // 存在重复的列模式
    }

    return true; // 行和列模式都唯一
}


// 递归回溯函数
// k: 还需要选择的格子数量
// start_index: 从一维索引的哪个位置开始搜索 (0 to 35)
// board: 当前棋盘状态 (-1: empty, 0: selected)
// current_selection: 当前已选择的格子坐标
void findSolutions(int k, int start_index,
                   vector<vector<int>>& board,
                   vector<pair<int, int>>& current_selection,
                   vector<int>& row_counts, vector<int>& col_counts) {
    if (k == 0) {
        // Base case: Found 18 selections. Now perform final checks.
        // 1. Verify row/col counts are exactly 3 (sanity check)
        bool counts_ok = true;
        for(int count : row_counts) if(count != 3) counts_ok = false;
        for(int count : col_counts) if(count != 3) counts_ok = false;

        if (!counts_ok) {
             cerr << "Error: Reached base case with incorrect row/col counts!" << endl;
             // This should ideally not happen due to pruning
             return;
        }

        // 2. Check for unique row/column patterns
        if (checkRowColUniqueness(board)) {
            // 3. Check for three-in-a-row/col (already done during recursion, but can double check if needed)
            // bool no_three_in_a_row = true; // Add full board check if paranoid
            // if (no_three_in_a_row) {
                cout << "Found a valid solution (3/row, 3/col, no 3-in-a-row, unique patterns):\n";
                for (const auto& p : current_selection) {
                    cout << "(" << p.first << ", " << p.second << ") ";
                }
                cout << "\n--------------------\n";
            // }
        }
        return; // Return after checking this potential solution
    }

    // 如果剩余的格子数量加上已选的格子数量不足 TARGET_COUNT，则无法构成解
    // Pruning: If remaining cells are not enough to reach the target count
    if (start_index + k > GRID_SIZE * GRID_SIZE) {
        return;
    }
    for (int i = start_index; i < GRID_SIZE * GRID_SIZE; ++i) {
        int r = i / GRID_SIZE;
        int c = i % GRID_SIZE;

        // Pruning 1: Check row/column counts before trying
        if (row_counts[r] >= 3 || col_counts[c] >= 3) {
            continue; // Cannot select more in this row or column
        }

        // Try selecting cell (r, c)
        board[r][c] = 0;

        // Pruning 2: Check for three-in-a-row immediately
        if (!check(r, c, board)) {
            // Invalid move, backtrack immediately without recursing
            board[r][c] = -1; // Restore board state
            continue;         // Try next cell
        }

        // If valid so far, update counts and selection
        row_counts[r]++;
        col_counts[c]++;
        current_selection.push_back({r, c});

        // Recurse for the next cell
        findSolutions(k - 1, i + 1, board, current_selection, row_counts, col_counts);

        // Backtrack: Restore state
        current_selection.pop_back();
        col_counts[c]--;
        row_counts[r]--;
        board[r][c] = -1;
    }
}

int main() {
    vector<vector<int>> board(GRID_SIZE, vector<int>(GRID_SIZE, -1));
    vector<pair<int, int>> selection;
    vector<int> row_counts(GRID_SIZE, 0);
    vector<int> col_counts(GRID_SIZE, 0);

    cout << "Starting search for solutions (18 cells, 3/row, 3/col, no 3-in-a-row, unique patterns)...\n";

    // Start the recursive search
    findSolutions(TARGET_COUNT, 0, board, selection, row_counts, col_counts);

    cout << "Search finished.\n";

    return 0;
}