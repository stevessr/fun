def solve():
    n = int(input())
    b = list(map(int, input().split()))
    b.sort()

    for i in range(2 * n):
        a = []
        temp_b = b[:]
        deleted = temp_b.pop(i)
        a.extend(temp_b)

        sum_even = 0
        sum_odd = 0
        for j in range(len(a)):
            if (j + 1) % 2 == 0:
                sum_even += a[j]
            else:
                sum_odd += a[j]

        
        possible_a1 = sum_even - sum_odd
        
        
        
        
        a_copy = a[:]
        
        if possible_a1 > 0 :
          a_copy.insert(0,possible_a1)
          
          test_sum_even = 0
          test_sum_odd = 0

          for j in range(len(a_copy)):
              if (j + 1) % 2 == 0:
                  test_sum_even += a_copy[j]
              else:
                  test_sum_odd += a_copy[j]
          
          if test_sum_even - test_sum_odd == a_copy[0] :
            print(*a_copy)
            return 

    
    a = b[:]
    
    sum_even = 0
    sum_odd = 0
    
    for j in range(0,len(a)-1):
        if (j + 1) % 2 == 0:
            sum_even += a[j]
        else :
            sum_odd += a[j]

    possible_a1 = sum_even - sum_odd
    
    if possible_a1 > 0 : 

        
        
        a_copy = a[:]
        

        a_copy.insert(0,possible_a1)


        test_sum_even = 0
        test_sum_odd = 0

        for j in range(len(a_copy)):
            if (j + 1) % 2 == 0:
                test_sum_even += a_copy[j]
            else:
                test_sum_odd += a_copy[j]

        if test_sum_even - test_sum_odd == a_copy[0] :
            print(*a_copy)
            return


t = int(input())
for _ in range(t):
    solve()