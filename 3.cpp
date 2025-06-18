#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>

// Function to solve a single test case
void solve() {
    int n; // Number of admirers/towers
    std::cin >> n;
    
    // Use long long for tower heights because individual heights can be up to 10^9.
    // The sum S can potentially exceed the capacity of a 32-bit integer, so long long is necessary for S as well.
    std::vector<long long> a(n); 
    
    long long S = 0; // Total sum of heights, initialized to 0. Must be long long.
    int odd_count = 0; // Count of towers with odd height. An int is sufficient as n <= 2*10^5.
    long long initial_max = 0; // Maximum initial height. We use long long just to be safe, as the final maximum beauty might exceed 10^9 although initial max might not.
    
    // Read tower heights from input.
    // While reading, calculate the total sum S, count the number of towers with odd heights,
    // and keep track of the maximum initial height.
    for (int i = 0; i < n; ++i) {
        std::cin >> a[i];
        S += a[i]; // Add current height to the total sum.
        
        // Check if the current height a[i] is odd. 
        // The bitwise AND operator '& 1' is an efficient way to check parity. 
        // It returns 1 if the number is odd, 0 if even.
        if (a[i] & 1) { 
            odd_count++; // Increment the count of odd towers.
        }
        
        // Update the maximum initial height found so far.
        initial_max = std::max(initial_max, a[i]);
    }
    
    // Now determine the maximum possible beauty based on the properties derived.
    // The core idea relies on the invariant that the number of odd towers (odd_count) remains constant throughout operations.
    
    // Case 1: All towers have the same parity initially.
    // This happens if odd_count is 0 (all even) or odd_count is n (all odd).
    if (odd_count == 0 || odd_count == n) {
        // In this case, for any pair of distinct towers i and j, a[i] + a[j] will always be even.
        // The condition for the operation (a[i] + a[j] is odd) is never met.
        // Therefore, no operations can be performed.
        // The maximum beauty remains the same as the initial maximum height.
        std::cout << initial_max << "\n";
    } 
    // Case 2: There is a mix of odd and even towers initially.
    // This happens if 0 < odd_count < n.
    else {
        // When there's a mix of parities, operations are possible.
        // The analysis showed that it's possible to rearrange the heights through operations
        // such that we can potentially achieve a maximum height configuration.
        // Specifically, we can reach a state where one tower has height S - odd_count + 1, 
        // and this configuration respects the invariant number of odd towers.
        // This value represents the maximum possible height achievable for any single tower.
        // S - odd_count is the total "even part" of the sum, which is always even.
        // S - odd_count + 1 is thus always odd.
        // This state corresponds to one tower having height S - odd_count + 1 (odd),
        // and odd_count - 1 other towers having height 1 (odd), ensuring k=odd_count towers are odd.
        // The remaining towers would have height 0 (even).
        long long max_beauty = S - odd_count + 1;
        std::cout << max_beauty << "\n";
    }
}

int main() {
    // Standard C++ optimization technique for faster input/output.
    // std::ios_base::sync_with_stdio(false) disables synchronization with C standard input/output streams.
    // std::cin.tie(NULL) unties std::cin from std::cout, allowing input and output operations to not wait for each other.
    std::ios_base::sync_with_stdio(false); 
    std::cin.tie(NULL); 
    
    int t; // Number of test cases
    std::cin >> t;
    // Loop through each test case and solve it.
    while (t--) {
        solve(); 
    }
    return 0; // Indicate successful execution.
}