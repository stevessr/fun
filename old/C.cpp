#include <iostream>
#include <vector>
#include <algorithm>

void solve() {
    int n;
    std::cin >> n;
    std::vector<long long> b(2 * n);
    for (int i = 0; i < 2 * n; ++i) {
        std::cin >> b[i];
    }
    std::sort(b.begin(), b.end());

    for (int i = 0; i < 2 * n; ++i) {
        std::vector<long long> a;
        long long sum = 0;
        for (int j = 0; j < 2 * n; ++j) {
            if (j != i) {
                a.push_back(b[j]);
            }
        }
        
        long long x = b[i];
        a.push_back(0); 

        sum = 0;
        for(int k=0; k<2*n; k+=2){
            if (k+1 < 2*n) {
                sum += a[k] - a[k+1];
            } else{
                sum += a[k];
            }
        }

        if (sum == x) {
          a[2*n-1] = x;
          for (long long val : a) {
              std::cout << val << " ";
          }
          std::cout << std::endl;
          return;
        }

        
        sum = 0;
        for(int j = 0; j<a.size(); j++){
            if((j % 2 == 0)){
                sum+= a[j];
            } else{
                sum -= a[j];
            }
        }
        if (sum == x)
        {
           a[2*n-1] = b[i];
           for(int num = 0;num<a.size();num++){
              std::cout<< a[num] << " ";
           }
           std::cout<< std::endl;
           return;
        }
    }

    
    long long sum = 0;
    for(int i=0; i< 2*n-1; i+=2){
        sum += (b[i] - b[i+1]);
    }
    
    std::vector<long long> res;
    for(int i=0; i< 2*n-1; i++){
       res.push_back(b[i]); 
    }
    res.push_back(b[2*n-1]);
    res.push_back(sum);
    
    if(sum){
       for(long long x:res){
          std::cout << x << " ";
       }
       std::cout <<std::endl;
    }
}

int main() {
    int t;
    std::cin >> t;
    while (t--) {
        solve();
    }
    return 0;
}