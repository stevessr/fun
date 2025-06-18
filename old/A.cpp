#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

bool is_good(const vector<vector<int>>& matrix) {
    int n = matrix.size();
    int m = matrix[0].size();

    for (int i = 0; i < n; ++i) {
        int row_xor = 0;
        for (int j = 0; j < m; ++j) {
            row_xor ^= matrix[i][j];
        }
        if (row_xor != 0) {
            return false;
        }
    }

    for (int j = 0; j < m; ++j) {
        int col_xor = 0;
        for (int i = 0; i < n; ++i) {
            col_xor ^= matrix[i][j];
        }
        if (col_xor != 0) {
            return false;
        }
    }

    return true;
}

int solve() {
    int n, m;
    cin >> n >> m;
    vector<vector<int>> matrix(n, vector<int>(m));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            char c;
            cin >> c;
            matrix[i][j] = c - '0';
        }
    }

    int min_changes = n * m;
    for (int i = 0; i < (1 << (n * m)); ++i) {
        vector<vector<int>> temp_matrix = matrix;
        int changes = 0;
        int k = i;
        for (int row = 0; row < n; ++row) {
            for (int col = 0; col < m; ++col) {
                int bit = k % 2;
                k /= 2;
                if (bit != temp_matrix[row][col]) {
                    changes++;
                    temp_matrix[row][col] = bit;
                }
            }
        }

        if (is_good(temp_matrix)) {
            min_changes = min(min_changes, changes);
        }
    }

    return min_changes;
}

int main() {
    int t;
    cin >> t;
    while (t--) {
        cout << solve() << endl;
    }
    return 0;
}