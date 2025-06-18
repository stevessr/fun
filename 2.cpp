#include <bits/stdc++.h>
using namespace std;

int main()
{
    int T;
    cin >> T;
    while (T--)
    {
        int n;
        cin >> n;
        string a, b;
        cin >> a >> b;
        bool success = true;
        for (int i = 0; i < n; i++)
        {
            if (a[i] == '1')
            {
                if ((i != 1 ? b[i - 1] == '1' : true)&&(i != n - 1 ? b[i + 1] == '1' : true))
                {
                    success = false;
                    break;
                }
            }
        }
        if(success)
        {
            cout << "YES" << endl;
        }
        else
        {
            cout << "NO" << endl;
        }
    }
    return 0;
}