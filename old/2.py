def solve(n, k):
    result = [n] * (n-1) + [n-1]
    return result

t = int(input())
for _ in range(t):
    n, k = map(int, input().split())
    print(*solve(n, k))