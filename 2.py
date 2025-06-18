import sys

# Function to calculate ceil(n/2) using integer division
# For any integer n >= 0, (n + 1) // 2 correctly computes ceil(n / 2).
def ceil_div2(n):
    return (n + 1) // 2

# Function to calculate floor(n/2) using integer division
# For any integer n >= 0, n // 2 correctly computes floor(n / 2).
def floor_div2(n):
    return n // 2

def solve():
    # Read the length of the bit strings
    n = int(sys.stdin.readline())
    # Read the bit string a
    a = sys.stdin.readline().strip()
    # Read the bit string b
    b = sys.stdin.readline().strip()

    # The operations allow swapping values between positions. We can analyze the connectivity.
    # The positions {a_1, ..., a_n, b_1, ..., b_n} can be partitioned into two connected components based on the allowed swaps.
    # We use 0-based indexing for strings a and b (indices 0 to n-1).
    
    # Component 1 consists of positions: {a[j] | j is even} union {b[j] | j is odd}.
    # Component 2 consists of positions: {a[j] | j is odd} union {b[j] | j is even}.
    # Within each component, any permutation of the initial values is possible.

    # We want to reach a state where string 'a' consists only of zeros: a[j] = '0' for all j from 0 to n-1.
    
    # Initialize counters for the number of zeros in each component
    zeros_c1 = 0 # Counts zeros initially in Component 1
    zeros_c2 = 0 # Counts zeros initially in Component 2

    # Iterate through the strings to count initial zeros in each component
    for j in range(n):
        if j % 2 == 0: # Index j is even
            # Check a[j]. This position belongs to Component 1.
            if a[j] == '0':
                zeros_c1 += 1
            # Check b[j]. This position belongs to Component 2.
            if b[j] == '0':
                zeros_c2 += 1
        else: # Index j is odd
            # Check a[j]. This position belongs to Component 2.
            if a[j] == '0':
                zeros_c2 += 1
            # Check b[j]. This position belongs to Component 1.
            if b[j] == '0':
                zeros_c1 += 1

    # Determine the number of zeros required for string 'a' from each component.
    # The target state requires a[j] = '0' for all j.
    
    # Positions a[j] where j is even belong to Component 1.
    # The number of such positions is ceil(n/2).
    # To make these positions zero, we need at least ceil(n/2) zeros available in Component 1.
    required_zeros_for_a_even = ceil_div2(n) 
    
    # Positions a[j] where j is odd belong to Component 2.
    # The number of such positions is floor(n/2).
    # To make these positions zero, we need at least floor(n/2) zeros available in Component 2.
    required_zeros_for_a_odd = floor_div2(n)       

    # Check if both conditions are met:
    # Does Component 1 have enough zeros for the even positions of 'a'?
    # Does Component 2 have enough zeros for the odd positions of 'a'?
    if zeros_c1 >= required_zeros_for_a_even and zeros_c2 >= required_zeros_for_a_odd:
        # If both conditions are satisfied, it is possible to make string 'a' all zeros.
        print("YES")
    else:
        # Otherwise, it is not possible.
        print("NO")

# Read the number of test cases
T = int(sys.stdin.readline())
# Process each test case
for _ in range(T):
    solve()