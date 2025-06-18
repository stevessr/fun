#include <iostream>
#include <string>
#include <vector>
using namespace std;
struct floor
{
    int b;
    vector<string> stus;
};
struct building
{
    int a;
    vector<floor> f;
};
int main()
{
    int n, m, k;
    vector<building> bs(n);
    for(auto &i:bs){
        i.f.resize(m);
    }
    int a, b, c;
    for(int i=0;i<n;i++){
        cin>>a>>b>>c;
        bs[a-1].a=a;
        bs[a-1].f[b-1].b=b;
        bs[a-1].f[b-1].stus.resize(c);
        for(auto &s:bs[a-1].f[b-1].stus){
            cin>>s;
        }
    }
    int T;
    cin >> T;
    while (T--)
    {
        string todo;
        cin >> todo;
        bool found = false;
        for (size_t i = 0; i < bs.size(); i++)
        {
            for (size_t j = 0; j < bs[i].f.size(); j++)
            {
                for (size_t k = 0; k < bs[i].f[j].stus.size(); k++)
                {
                        if (bs[i].f[j].stus[k] == todo)
                        {
                            found = true;
                            cout << i + 1 << " " << j + 1 << " " << k + 1 << endl;
                        }
                }
            }
        }
        if (!found)
        {
            cout << "not found" << endl;
        }
    }
    return 0;
}