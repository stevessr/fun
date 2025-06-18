import sys
import math

def main():
    input = sys.stdin.read().split()
    idx = 0
    t = int(input[idx])
    idx += 1
    
    # Precompute the number of divisors for each number up to 1e6
    max_B = 10**6
    divisors = [0] * (max_B + 1)
    for i in range(1, max_B + 1):
        for j in range(i, max_B + 1, i):
            divisors[j] += 1
    
    for _ in range(t):
        n = int(input[idx])
        idx += 1
        s = input[idx]
        idx += 1
        
        beauty = [0] * (n + 1)
        for i in range(2, n + 1):
            beauty[i] = beauty[i - 1] + (s[i - 2] != s[i - 1])
        
        res = []
        for i in range(1, n + 1):
            B = beauty[i]
            if B == 0:
                res.append(i)
            else:
                res.append(divisors[B])
        
        print(' '.join(map(str, res)))
    
if __name__ == '__main__':
    main()