#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>

using namespace std;

bool check_capacity(int days, int capacity, long long target_k, int n, const vector<int>& a) {
    if (days > n) {
        return false;
    }
    long long current_revenue = 0;
    for (int i = 0; i < days; ++i) {
        current_revenue += min((long long)a[i], (long long)capacity);
    }
    return current_revenue >= target_k;
}

int main() {
    int t;
    cin >> t;
    while (t--) {
        int n;
        long long m0, k;
        cin >> n >> m0 >> k;
        vector<int> a(n);
        for (int i = 0; i < n; ++i) {
            cin >> a[i];
        }

        int best_days = -1;
        long long best_capacity = -1;
        long long best_revenue = -1;

        int min_days_possible = -1;
        long long min_capacity_for_min_days = -1;
        long long revenue_for_min_days_capacity = -1;

        int low_days = 1, high_days = n;
        int min_days_found = -1;

        while (low_days <= high_days) {
            int current_days = low_days + (high_days - low_days) / 2;
            int low_capacity = 1, high_capacity = min((long long)m0, 2000000000LL); // set a reasonable upper bound for capacity in binary search
            int min_capacity_found_for_days = -1;

            while (low_capacity <= high_capacity) {
                int current_capacity = low_capacity + (high_capacity - low_capacity) / 2;
                if (check_capacity(current_days, current_capacity, k, n, a)) {
                    min_capacity_found_for_days = current_capacity;
                    high_capacity = current_capacity - 1;
                } else {
                    low_capacity = current_capacity + 1;
                }
            }

            if (min_capacity_found_for_days != -1) {
                min_days_found = current_days;
                min_capacity_for_min_days = min_capacity_found_for_days;
                revenue_for_min_days_capacity = 0;
                for (int i = 0; i < current_days; ++i) {
                    revenue_for_min_days_capacity += min((long long)a[i], (long long)min_capacity_for_min_days);
                }
                high_days = current_days - 1;
            } else {
                low_days = current_days + 1;
            }
        }

        if (min_days_found != -1) {
            cout << min_days_found << " " << min_capacity_for_min_days << " " << revenue_for_min_days_capacity << endl;
        } else {
            cout << "tai ruo xiao le, mei you li liang" << endl;
        }
    }
    return 0;
}