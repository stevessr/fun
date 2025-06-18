#include <iostream>

using namespace std;

int main() {
    int t;
    cin >> t;
    while (t--) {
        long long x, y;
        cin >> x >> y;
        if (x == y) {
            cout << -1 << endl;
        } else {
            long long k = 0;
            int carry = 0;
            for (int i = 0; i < 61; ++i) {
                int x_bit = (x >> i) & 1;
                int y_bit = (y >> i) & 1;
                int u_prime = (x_bit + carry) % 2;
                int v_prime = (y_bit + carry) % 2;
                int k_bit;
                if (u_prime == 0 && v_prime == 0) {
                    k_bit = 0;
                } else if (u_prime == 1 && v_prime == 1) {
                    k_bit = 1;
                } else {
                    k_bit = 0;
                }
                carry = (x_bit + k_bit + carry) / 2;
                if (carry != (y_bit + k_bit + carry) / 2) {
                    // Should not happen based on logic
                }
                if (k_bit == 1) {
                    k |= (1LL << i);
                }
            }
            cout << k << endl;
        }
    }
    return 0;
}