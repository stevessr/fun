#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>

using namespace std;

int main(){
    int T;
    cin>>T;
    while(T--){
        int n;
        cin>>n;
        vector<int> a(n);
        for(int i=0;i<n;i++){
            cin>>a[i];
        }
        if(a[0]!=0){
            cout<<2<<endl;
            cout<<1<<' '<<n-1<<endl<<'0 1\n';
        }else{
            cout<<3<<endl;
            cout<<'0 1\n1 '<<n-2<<'\n0 1\n';
        }
    }
    return 0;
}