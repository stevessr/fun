#include <iostream>
using namespace std;
using ll = long long;
int main(){
    ll T;
    cin>>T;
    while(T--){
        ll a,x,temp;
        ll sum = 0;
        cin>>a>>x;
        for(int i = 0;i<a;i++){
            cin>>temp;
            sum+=temp;
        }
        if(sum==a*x){
            cout<<"YES"<<endl;
        }
        else{
            cout<<"NO"<<endl;

        }
    }
}