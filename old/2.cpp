#include <iostream>
#include <vector>
using namespace std;
using ll = long long;
int main()
{
    int T;
    cin >> T;
    while (T--)
    {
        ll n, k;
        cin >> n >> k;
        if (k % 2 == 1)
        {
            for (int i = 1; i < n; i++)
            {
                cout << n << " ";
            }
            cout << n - 1 << endl;
        }
        else
        {
            for (int i = 2; i < n; i++)
            {
                cout << n - 1 << " ";
            }
            cout << n << " " << n - 1 << endl;
        }
    }
}