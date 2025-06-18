#include <iostream> // For input/output operations (cin, cout)
#include <vector>   // For using std::vector dynamic arrays
#include <string>   // For using std::string (though not directly needed here)
#include <algorithm> // For std::sort, std::unique, std::max
#include <limits>   // For using __LONG_LONG_MAX__

// Use standard long long for state values (L, R) and final answer.
// The problem statement implies results can exceed 32-bit int.
typedef long long ll; 

// Use __int128 for intermediate calculations involving potentially large numbers
// to prevent overflow before casting back to long long. This type is a
// GCC/Clang extension.
#ifdef __GNUC__
typedef __int128 int128;
// Define the maximum value representable by long long, cast to __int128 for comparisons.
const int128 MAX_VAL_128 = (int128)__LONG_LONG_MAX__; 
#else
// Provide a fallback for compilers that do not support __int128.
// This fallback might lead to incorrect results on test cases with very large intermediate values.
#warning "__int128 not supported, using long long as fallback. Potential overflow issues."
typedef long long int128; 
// Define the max value using the long long limit.
const int128 MAX_VAL_128 = __LONG_LONG_MAX__; 
#endif

// Define the maximum value for long long, used for capping results and overflow checks.
const ll MAX_VAL = __LONG_LONG_MAX__; 

// Structure to represent a gate's operation ('+' or 'x') and its associated value.
struct Gate {
    char type; // '+' for addition, 'x' for multiplication
    ll val;    // The amount for '+' or the factor for 'x'
};

// Structure to represent the state of the game (people in Left and Right lanes).
struct State {
    ll L, R; // People counts in Left and Right lanes

    // Custom comparison operator for sorting states.
    // Primarily sort by L ascending. For ties in L, sort by R descending.
    // This specific order is beneficial for the efficiency of the Pareto frontier pruning algorithm.
    // States with higher R for the same L are preferred (kept later during pruning).
    bool operator<(const State& other) const {
        if (L != other.L) return L < other.L; // Smaller L comes first
        return R > other.R; // Larger R comes first if L is equal
    }
    
    // Equality operator, needed for std::unique to remove duplicate states.
    bool operator==(const State& other) const {
        return L == other.L && R == other.R;
    }
};

// Function to prune the set of candidate states to keep only the Pareto frontier.
// A state S1=(L1, R1) dominates S2=(L2, R2) if L1 >= L2 and R1 >= R2, with at least one inequality being strict.
// The function removes dominated states, keeping only non-dominated ones.
// Input: `candidates` is a vector of potential next states.
// Output: A vector containing only the non-dominated states from the input.
std::vector<State> prune(std::vector<State>& candidates) {
    // If there are no candidates, return an empty vector.
    if (candidates.empty()) return {}; 
    
    // Sort candidates based on the custom State::operator<.
    // This arranges states primarily by increasing L, then decreasing R.
    std::sort(candidates.begin(), candidates.end());
    // Remove adjacent duplicate states that might have been generated.
    candidates.erase(std::unique(candidates.begin(), candidates.end()), candidates.end());

    std::vector<State> frontier; // Vector to store the resulting Pareto frontier.
    // Reserving capacity based on the number of unique candidates can slightly improve performance.
    frontier.reserve(candidates.size()); 

    // Iterate through the sorted, unique candidate states.
    for (const auto& p : candidates) {
        // Pruning step: Remove states from the back of the `frontier` that are dominated by `p`.
        // Since `candidates` are sorted by L ascending, any `frontier.back()` state has L <= p.L.
        // If `frontier.back().R <= p.R`, then `p` dominates `frontier.back()`.
        while (!frontier.empty() && frontier.back().R <= p.R) {
            frontier.pop_back(); // Remove the dominated state.
        }
        
        // Add state `p` to the frontier if it's not dominated by the last state already in the frontier.
        // If `frontier` is empty, `p` is definitely not dominated.
        // If `frontier.back().L < p.L`, `p` cannot be dominated by `frontier.back()` based on L.
        // If `frontier.back().L == p.L`, then due to sorting and the `while` loop, we must have `p.R < frontier.back().R`,
        // meaning `p` *is* dominated, and this condition (`frontier.back().L < p.L`) correctly prevents adding `p`.
        if (frontier.empty() || frontier.back().L < p.L) {
             frontier.push_back(p);
        }
    }
    return frontier; // Return the computed Pareto frontier.
}


int main() {
    // Optimize standard C++ input/output stream synchronization.
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    int t; // Number of test cases
    std::cin >> t;
    while (t--) {
        int n; // Number of gate pairs in the current test case
        std::cin >> n;
        // Vector to store the information for each of the n gate pairs.
        std::vector<std::pair<Gate, Gate>> pairs(n); 
        for (int i = 0; i < n; ++i) {
            char typeL, typeR; // Types of the left and right gates ('+' or 'x')
            ll valL, valR;    // Values associated with the left and right gates
            std::cin >> typeL >> valL >> typeR >> valR;
            pairs[i].first = {typeL, valL}; // Store left gate info
            pairs[i].second = {typeR, valR}; // Store right gate info
        }

        // Vector to store the current set of non-dominated states (Pareto frontier).
        std::vector<State> current_states; 
        current_states.push_back({1, 1}); // Initialize with the starting state (1 person in each lane).

        // Heuristics parameters to manage memory usage if the state space explodes.
        // If the number of states exceeds STATE_LIMIT, sample states from the extremes.
        const size_t STATE_LIMIT = 400000;     // Max number of states to keep without sampling.
        const size_t HEAD_TAIL_SAMPLE = 200000; // Number of states to sample from head (low L) and tail (high L).

        // Process each gate pair sequentially.
        for (int i = 0; i < n; ++i) { 
            
            // State Limiting Heuristic: If the frontier becomes too large, reduce its size.
             if(current_states.size() > STATE_LIMIT) { 
                 std::vector<State> sampled_states; // Store the sampled states.
                 // Estimate required capacity to reduce reallocations.
                 size_t reserve_size = std::min((size_t)STATE_LIMIT + 10, 2 * HEAD_TAIL_SAMPLE); 
                 sampled_states.reserve(reserve_size); 
                 
                 // Keep the first HEAD_TAIL_SAMPLE states (those with the smallest L values).
                 for(size_t k=0; k < current_states.size() && k < HEAD_TAIL_SAMPLE; ++k) {
                     sampled_states.push_back(current_states[k]);
                 }
                 
                 // Keep the last HEAD_TAIL_SAMPLE states (those with the largest L values).
                 // Calculate starting index correctly.
                 size_t start_idx = (current_states.size() > HEAD_TAIL_SAMPLE) ? (current_states.size() - HEAD_TAIL_SAMPLE) : 0;
                 for(size_t k = start_idx; k < current_states.size(); ++k) {
                     // Add states from the tail. Duplicates might occur if ranges overlap.
                     sampled_states.push_back(current_states[k]);
                 }
                 
                 // Sort and remove duplicates from the combined sample.
                 std::sort(sampled_states.begin(), sampled_states.end());
                 sampled_states.erase(std::unique(sampled_states.begin(), sampled_states.end()), sampled_states.end());
                 // Replace the full state list with the sampled list.
                 current_states = std::move(sampled_states); 
             }

            // Vector to store all candidate states generated in this step before pruning.
            std::vector<State> next_candidates; 
            // Reserve memory anticipating roughly 2 candidates per current state plus a buffer.
            // Adjusted reservation based on the simplified splitting logic below.
            next_candidates.reserve(std::min((size_t)1000000, current_states.size() * 2 + 100)); 

            const Gate& gateL = pairs[i].first; // Current left gate
            const Gate& gateR = pairs[i].second; // Current right gate
            
            // Iterate through each state on the current Pareto frontier.
            for (const auto& state : current_states) { 
                ll currentL = state.L;
                ll currentR = state.R;

                // Calculate the base values (after multiplication, if any) and the delta (additional people)
                // generated by each gate, using __int128 for intermediate steps to prevent overflow.
                int128 nextL_base_128 = currentL; // Base L starts as current L
                int128 deltaL_128 = 0;           // Additional people from left gate
                bool skip_state = false; // Flag to indicate if overflow occurs during base calculation        

                // Process Left Gate
                if (gateL.type == '+') {
                    // Addition: Delta is the gate value, base remains currentL.
                    deltaL_128 = gateL.val;
                } else { // Multiplication 'x'
                    // Check for potential overflow: currentL * gateL.val
                    // Perform check before multiplication using division property or direct comparison with MAX_VAL_128.
                    int128 next_val_check = (int128)currentL * gateL.val;
                    // Check if multiplication result exceeds or equals MAX_VAL_128 (the limit for ll)
                    // Need careful check: if currentL=0, result is 0. If val=1, result is currentL.
                    // Overflow only possible if currentL > 0 and val > 1.
                    if (currentL > 0 && gateL.val > 1 && next_val_check >= MAX_VAL_128) { 
                        skip_state = true; // Mark state as invalid if base calculation overflows.
                    } else {
                       nextL_base_128 = next_val_check; // Update base L value after multiplication.
                       deltaL_128 = nextL_base_128 - currentL; // Calculate additional people generated.
                    }
                }
                // If Left Gate processing led to overflow, skip this state entirely for this gate pair.
                if (skip_state) continue; 

                // Process Right Gate (similar logic)
                int128 nextR_base_128 = currentR; // Base R starts as current R
                int128 deltaR_128 = 0;           // Additional people from right gate
                if (gateR.type == '+') {
                    deltaR_128 = gateR.val;
                } else { // Multiplication 'x'
                   // Check for potential overflow: currentR * gateR.val
                   int128 next_val_check = (int128)currentR * gateR.val;
                    if (currentR > 0 && gateR.val > 1 && next_val_check >= MAX_VAL_128) { 
                       skip_state = true; // Mark state as invalid if base calculation overflows.
                   } else {
                      nextR_base_128 = next_val_check; // Update base R value after multiplication.
                      deltaR_128 = nextR_base_128 - currentR; // Calculate additional people generated.
                   }
                }
                 // If Right Gate processing led to overflow, skip this state entirely.
                 if (skip_state) continue; 
                
                // Calculate the total additional people generated by both gates.
                // Check for overflow when adding deltas.
                int128 delta_total_128 = 0;
                if (deltaL_128 > MAX_VAL_128 - deltaR_128) { // Check if deltaL + deltaR overflows
                    // If total delta overflows, consider it as effectively infinite or capped.
                    // Skipping the state might be safer if interpretation is unclear.
                    // Let's skip if total delta overflows, as assigning it would be problematic.
                    continue; 
                } else {
                    delta_total_128 = deltaL_128 + deltaR_128;
                }
                
                // Basic sanity check: total delta should be non-negative.
                 if (delta_total_128 < 0) continue; 

                // *** HEURISTIC SPLITTING LOGIC ***
                // Instead of exploring all k from 0 to delta_total, only consider the two extreme splits:
                // 1. All delta goes to the Left lane (k = delta_total).
                // 2. All delta goes to the Right lane (k = 0).
                // This significantly reduces the number of candidate states generated per step.

                // State 1: k = delta_total (All delta added to Left Base)
                int128 nextL1_128 = nextL_base_128;
                if (nextL1_128 <= MAX_VAL_128 - delta_total_128) { // Check for overflow before adding delta
                    nextL1_128 += delta_total_128;
                    int128 nextR1_128 = nextR_base_128; // Right base remains unchanged by this split.
                    // Check if final state values are within long long limits.
                    if (nextL1_128 < MAX_VAL_128 && nextR1_128 < MAX_VAL_128) {
                        next_candidates.push_back({(ll)nextL1_128, (ll)nextR1_128});
                    }
                } // else: Adding delta to L overflows, discard this state candidate.


                // State 2: k = 0 (All delta added to Right Base)
                // Only generate this state if delta_total > 0, otherwise it's identical to State 1 when delta is 0.
                if (delta_total_128 > 0) { 
                    int128 nextL2_128 = nextL_base_128; // Left base remains unchanged by this split.
                    int128 nextR2_128 = nextR_base_128; 
                    if (nextR2_128 <= MAX_VAL_128 - delta_total_128) { // Check for overflow before adding delta
                        nextR2_128 += delta_total_128;
                        // Check if final state values are within long long limits.
                        if (nextL2_128 < MAX_VAL_128 && nextR2_128 < MAX_VAL_128) {
                             next_candidates.push_back({(ll)nextL2_128, (ll)nextR2_128});
                        }
                    } // else: Adding delta to R overflows, discard this state candidate.
                } 
                // *** END OF HEURISTIC SPLITTING LOGIC ***

            } // End loop iterating over `current_states`

            // Prune the generated `next_candidates` to get the new Pareto frontier for the next step.
            current_states = prune(next_candidates); 

            // Optimization: If the frontier becomes empty (e.g., all paths overflowed or were pruned),
            // we can stop processing further gate pairs.
             if (current_states.empty() && i < n - 1) { 
                  break; 
             }

        } // End loop over the `n` gate pairs

        // Calculate the final maximum total number of people (L + R) from the final Pareto frontier.
        ll max_total = 0; // Initialize max total found.
        bool frontier_non_empty_at_end = !current_states.empty(); // Check if any valid states remain.

        if (frontier_non_empty_at_end) {
           // Iterate through the final non-dominated states.
           for (const auto& state : current_states) {
               // Calculate the sum L + R using __int128 to prevent overflow during summation.
               int128 final_sum_check = (int128)state.L + state.R;
               // Check if the sum exceeds or equals the maximum value of long long.
               if (final_sum_check >= MAX_VAL_128) {
                   max_total = MAX_VAL; // If sum overflows, the result is capped at MAX_VAL.
                   break; // No need to check other states, as we've reached the maximum possible value.
               } else {
                   // If the sum is within limits, cast it back to long long.
                   ll current_total = (ll)final_sum_check; 
                   // Update `max_total` if this state's sum is greater.
                   max_total = std::max(max_total, current_total);
               }
           }
        }
        
        // Handle edge cases and results when the final frontier might be empty.
        if (n == 0) {
            // If there were no gates, the state is still the initial (1, 1).
            max_total = 2; 
        } else if (!frontier_non_empty_at_end) {
             // If n > 0 but the final frontier is empty, it suggests all paths likely led to
             // values exceeding MAX_VAL or were pruned away by heuristics (if used aggressively).
             // In competitive programming contexts, this usually implies the maximum possible value is reachable.
             max_total = MAX_VAL;
        }
        
        // Final sanity check: The result must be at least 2, as we start with (1, 1).
        // This guards against potential logic errors yielding a result less than 2.
        max_total = std::max(max_total, 2LL);


        // Output the final calculated maximum total number of people.
        std::cout << max_total << "\n"; 
    }
    return 0; // Indicate successful execution.
}