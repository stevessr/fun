def mex(arr):
    seen = set(arr)
    i = 0
    while i in seen:
        i += 1
    return i

def solve():
    n = int(input())
    a = list(map(int, input().split()))
    operations = []
    current_array = list(a)
    while len(current_array) > 1:
        found_operation = False
        for l_index in range(len(current_array)):
            for r_index in range(l_index + 1, len(current_array)):
                l = l_index + 1
                r = r_index + 1
                subarray = current_array[l_index:r_index+1]
                m = mex(subarray)
                operations.append((l, r))
                next_array = current_array[:l_index] + [m] + current_array[r_index+1:]
                current_array = next_array
                found_operation = True
                break
            if found_operation:
                break
        if not found_operation:
            break
            
    print(len(operations))
    for op in operations:
        print(op[0], op[1])

t = int(input())
for _ in range(t):
    solve()