def solve():
    x, n, m = map(int, input().split())

    def calculate_min(x, n, m):
        cur = x
        for _ in range(n):
            cur = cur // 2
        for _ in range(m):
            cur = (cur + 1) // 2
        return cur

    def calculate_max(x, n, m):
        cur = x
        for _ in range(m):
            cur = (cur + 1) // 2
        for _ in range(n):
            cur = cur // 2
        return cur

    print(calculate_min(x, n, m), calculate_max(x, n, m))

t = int(input())
for _ in range(t):
    solve()