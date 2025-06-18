#include <iostream> // For input/output operations (cin, cout)
#include <vector>   // For using std::vector
#include <string>   // For using std::string
#include <numeric>  // Potentially useful for algorithms like accumulate (though not used here)
#include <map>      // For using std::map (associative array for dp state)
#include <set>      // For using std::set (to count distinct k values efficiently)
#include <algorithm> // For std::sort, std::unique, std::merge, std::lower_bound etc.
#include <utility>  // For std::pair

// ---- Global Variables ----
// Using global vectors helps reuse allocated memory across test cases, reducing overhead.

// dp[i] stores a map for prefix s[1...i].
// The map keys are beauty values B > 0.
// The map values are sorted lists (vectors) of possible partition counts 'k'
// that result in a partition of s[1...i] with common beauty B.
std::vector<std::map<long long, std::vector<int>>> dp;

// IdxV[v] stores a list of prefix lengths 'k' such that the beauty of the prefix s[1...k]
// (V_prefix_beauty[k]) is exactly 'v'. This helps quickly find previous states.
std::vector<std::vector<int>> IdxV;

// V_prefix_beauty[i] stores the beauty of the prefix s[1...i].
// Beauty is defined as the number of adjacent character changes.
std::vector<long long> V_prefix_beauty;

// Temporary vector used within each iteration 'i' of the main loop.
// It stores pairs {Beauty B, partition count k} representing newly found valid partitions
// ending at index 'i'. These are collected, sorted, and then used to build dp[i].
std::vector<std::pair<long long, int>> updates_for_i;

// Temporary set used during the result calculation phase for each prefix 'i'.
// It efficiently tracks the distinct partition counts 'k' found across all possible positive beauty values (B > 0).
std::set<int> distinct_k_seen_in_result_calc;
// ---- End of Global Variables ----

// Function to solve a single test case
void solve() {
    int n; // Length of the binary string
    std::cin >> n;
    std::string s; // The binary string (0-indexed)
    std::cin >> s;

    // ---- Preprocessing ----

    // 1. Calculate prefix beauties V_prefix_beauty[i] for i = 1 to n.
    // V_prefix_beauty[0] is conceptually 0.
    // V_prefix_beauty[i] = beauty of s[0...i-1] (prefix of length i).
    V_prefix_beauty.assign(n + 1, 0); // Resize and initialize V for indices 0 to n.
    for (int i = 1; i < n; ++i) { // Iterate through adjacent pairs in the 0-indexed string s
        // V[i+1] (beauty of prefix length i+1) depends on V[i] and s[i-1] vs s[i].
        V_prefix_beauty[i + 1] = V_prefix_beauty[i] + (s[i - 1] != s[i]);
    }

    // 2. Precompute IdxV: Map beauty values 'v' to lists of prefix lengths 'k' where V[k] == v.
    // Max beauty is V[n] <= n-1. IdxV needs size up to n. n+1 is safe.
    IdxV.assign(n + 1, std::vector<int>()); // Clear previous data and resize.
    for (int k = 0; k <= n; ++k) { // Iterate through all prefix lengths k = 0 to n
        long long beauty_k = V_prefix_beauty[k];
        // Add check for safety, though beauty_k should be <= k-1 <= n-1 < n+1
        if (beauty_k < IdxV.size()) {
            IdxV[beauty_k].push_back(k); // Store prefix length k in the list for its beauty value.
        }
    }

    // ---- Dynamic Programming ----

    // Resize DP table for the current test case. dp[0] to dp[n].
    dp.assign(n + 1, std::map<long long, std::vector<int>>());

    // Vector to store the final answers c_i for each prefix length i.
    std::vector<long long> results(n); // results[i-1] holds the answer for prefix length i.

    // Iterate through each prefix length i from 1 to n.
    for (int i = 1; i <= n; ++i) {
        long long current_prefix_beauty = V_prefix_beauty[i]; // Beauty of s[1...i]

        // --- Optimized DP Calculation using temporary updates vector ---
        updates_for_i.clear(); // Clear the temporary vector for this prefix 'i'.

        // 1. Base Case: The prefix s[1...i] itself forms a partition with k=1 part.
        // Add this as a potential state if its beauty is positive.
        if (current_prefix_beauty > 0) {
            updates_for_i.push_back({current_prefix_beauty, 1}); // {Beauty B, count k=1}
        }

        // 2. DP Transitions: Find partitions ending at 'i' based on previous partitions.
        // Consider splitting s[1...i] into s[1...p] and s[p+1...i].
        // If s[1...p] has partitions with k-1 parts of common beauty B > 0,
        // and the last segment s[p+1...i] ALSO has beauty B,
        // then we found a partition of s[1...i] into k parts with common beauty B.
        // The beauty of the last segment s[p+1...i] is V[i] - V[p+1].
        // So we need to find 'p' such that dp[p][B] exists, where B = V[i] - V[p+1].
        // This is equivalent to V[p+1] = V[i] - B.

        // Iterate through all possible positive common beauty values B.
        // B cannot exceed the total beauty of the current prefix.
        for (long long B = 1; B <= current_prefix_beauty; ++B) {
            // Calculate the required beauty V[p+1] for the prefix ending before the last segment.
            long long target_V_val = current_prefix_beauty - B;
            // target_V_val must be non-negative. B > 0 and B <= V[i] ensures target_V_val >= 0.

            // Find all prefix lengths 'idx' (= p+1) such that V[idx] == target_V_val using the precomputed IdxV.
            // Check if target_V_val is within the bounds of IdxV (it should be).
            if (target_V_val < IdxV.size()) {
                for (int idx : IdxV[target_V_val]) { // idx is the length of prefix s[1...idx]
                    // The previous partition must end at p = idx - 1.
                    int p = idx - 1;
                    // Check if 'p' corresponds to a valid previous prefix (p >= 0).
                    // The condition p < i is implicitly true because B >= 1 leads to idx <= i.
                    if (p >= 0) {
                        // Check if a DP state dp[p][B] exists (meaning s[1...p] can be partitioned with beauty B).
                        // Use find() for efficiency; it returns an iterator.
                        auto it = dp[p].find(B);
                        if (it != dp[p].end()) { // Check if the iterator is valid (key B found)
                            // Get the list of partition counts 'k_prev' for dp[p][B].
                            const std::vector<int>& k_list = it->second;
                            // For each such 'k_prev', we can form a partition for s[1...i] with k = k_prev + 1 parts.
                            for (int k_prev : k_list) {
                                updates_for_i.push_back({B, k_prev + 1}); // Add {B, k} to updates.
                            }
                        }
                    }
                }
            }
        }

        // 3. Sort Updates: Sort all collected updates {B, k} first by B, then by k.
        // This allows efficient construction of the final dp[i] state.
        std::sort(updates_for_i.begin(), updates_for_i.end());

        // 4. Build dp[i]: Consolidate sorted updates into the dp[i] map.
        // Keep only unique k values for each B, maintaining sorted order.
        dp[i].clear(); // Clear any previous state for dp[i] (relevant if globals weren't fully cleared).
        if (!updates_for_i.empty()) {
            long long current_B = -1; // Sentinel to detect the start of the first B group.
            std::vector<int> current_k_list; // Vector to build the list of k's for the current B.

            for (const auto& update : updates_for_i) {
                long long next_B = update.first;
                int next_k = update.second;

                if (next_B != current_B) { // If we encounter a new beauty value B...
                    // Store the completed list for the previous B (if it exists).
                    if (!current_k_list.empty()) {
                        dp[i][current_B] = std::move(current_k_list); // Move ownership to the map.
                        // current_k_list is now empty after the move.
                    }
                    // Start processing the new beauty value B.
                    current_B = next_B;
                    // current_k_list is empty, start it with the first k for this B.
                    current_k_list.push_back(next_k);
                } else { // If it's the same beauty value B as the previous update...
                    // Add the new k value only if it's different from the last one added.
                    // Since updates are sorted by k for the same B, duplicates are adjacent.
                    // This check ensures the k list remains unique and sorted.
                    if (current_k_list.empty() || next_k != current_k_list.back()) {
                        current_k_list.push_back(next_k);
                    }
                }
            }
            // After the loop, store the list for the very last beauty value B processed.
            if (!current_k_list.empty()) {
                dp[i][current_B] = std::move(current_k_list);
            }
        }
        // --- End of Optimized DP Calculation ---

        // --- Result Calculation for prefix 'i' ---
        // We need to count the total number of distinct k values possible,
        // considering both partitions with B > 0 and partitions with B = 0.

        // Calculate the minimum number of parts 'k' required for a partition with beauty B = 0.
        // This happens when each part is a maximal block of identical characters.
        // There are V[i] changes, meaning V[i] + 1 maximal blocks.
        long long min_k_for_B0 = 1 + V_prefix_beauty[i];

        // Count distinct 'k' values possible only with B > 0 (i.e., k < min_k_for_B0).
        long long count_small_k = 0;
        distinct_k_seen_in_result_calc.clear(); // Clear the helper set for this prefix 'i'.

        // Iterate through the computed dp[i] states (which only store B > 0).
        for (auto const& [B_val, k_list] : dp[i]) {
            for (int k : k_list) {
                // Try to insert 'k' into the set. `insert().second` is true if 'k' was new.
                if (distinct_k_seen_in_result_calc.insert(k).second) {
                    // If 'k' is a newly encountered distinct value for B > 0:
                    // Check if this 'k' is smaller than the minimum required for B=0.
                    if (k < min_k_for_B0) {
                        count_small_k++; // Increment count of k's possible only for B > 0.
                    }
                }
            }
        }

        // Calculate the number of 'k' values possible for a partition with beauty B = 0.
        long long count_B0_k = 0;
        // A partition with B=0 is possible if and only if k is in the range [min_k_for_B0, i].
        // Check if this range is valid (lower bound <= upper bound).
        if (min_k_for_B0 <= i) {
            // The number of integers 'k' in the inclusive range [min_k_for_B0, i].
            count_B0_k = (long long)i - min_k_for_B0 + 1;
        }

        // The total number of valid 'k' values for prefix 'i' is the sum of:
        // 1. Distinct 'k' values possible only with B > 0 (k < min_k_for_B0).
        // 2. 'k' values possible with B = 0 (k >= min_k_for_B0).
        // These two sets of 'k' are disjoint.
        results[i - 1] = count_small_k + count_B0_k;
        // --- End of Result Calculation ---
    }

    // ---- Output ----
    // Print the results c_1, c_2, ..., c_n.
    for (int i = 0; i < n; ++i) {
        std::cout << results[i] << (i == n - 1 ? "" : " "); // Space separated, no trailing space.
    }
    std::cout << "\n"; // Newline after each test case output.
}

// Main function
int main() {
    // Faster I/O operations. Unties cin/cout from C stdio and disables synchronization.
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    int t; // Number of test cases
    std::cin >> t;

    // Optional: Reserve memory for global vectors once, based on maximum possible N.
    // This might prevent reallocations during the first large test case.
    // Maximum N is 10^6. We need indices up to N, so size N+1.
    dp.reserve(1000001);
    IdxV.reserve(1000001);
    V_prefix_beauty.reserve(1000001);
    // updates_for_i size is unpredictable, maybe don't reserve or use a heuristic.
    // distinct_k_seen_in_result_calc size is at most N. Set handles its own memory.

    // Process each test case.
    while (t--) {
        solve();
    }

    return 0; // Indicate successful execution.
}