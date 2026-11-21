#include <iostream>
#define endl '\n'
#define rep(i, a, b) for (int i = (a); i < (b); i++)
#define rrep(i, a, b) for (int i = (a); i <= (b); i++)
#define int long long

using namespace std;

const int N = 1e5 + 10;

int a[N];

signed main() {
    ios::sync_with_stdio(false);
    cin.tie(0);

    int n; cin >> n;
    rrep(i, 1, n) cin >> a[i];

    long long res = 1e18;

    rrep(p, 1, 100) {
        long long sum = 0;
        rrep(j, 1, n) {
            sum += (long long)(a[j] - p) * (a[j] - p);
        }
        res = min(res, sum);
    }

    cout << res << endl;

}