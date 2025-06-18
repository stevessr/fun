def solve():
    x, y = map(int, input().split())

    if (x + y) % 2 != 0:
        print(-1)
        return

    if (x & y) == 0:
        print(0)
        return

    if (x & y) > 0:
        k = (x & y)
        print(k)
        return
        
    print(-1)

t = int(input())
for _ in range(t):
    solve()