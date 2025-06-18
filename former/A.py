import sys

def solve():
    t = int(sys.stdin.readline())
    for _ in range(t):
        line1 = sys.stdin.readline().split()
        n = int(line1[0])
        k = int(line1[1])
        s = sys.stdin.readline().strip()
        
        if n == 1:
            print("NO")
            continue
            
        all_same = True
        if n >= 2:
            first_char = s[0]
            for i in range(1, n):
                if s[i] != first_char:
                    all_same = False
                    break
                    
        if all_same:
            print("NO")
        else:
            if k == 0:
                s_rev = s[::-1]
                if s < s_rev:
                    print("YES")
                else:
                    print("NO")
            else:
                print("YES")

if __name__ == '__main__':
    solve()