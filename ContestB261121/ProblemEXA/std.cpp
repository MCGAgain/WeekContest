#include <cstdint>
#include <iostream>
#include <vector>
#define endl '\n'
#define rep(i, a, b) for (int i = (a); i < (b); i++)
#define rrep(i, a, b) for (int i = (a); i <= (b); i++)
#define int long long

using namespace std;

class Bitset {
    public:
    int cntones = 0;
        Bitset(int n): deg((n + 63) / 64, 0) {}

        void rev(int i) {
            int idx = i >> 6, bit = i & 63;
            cntones += (deg[idx] & (1ull << bit)) ? -1 : 1;
            deg[idx] ^= (1ull << bit);
        }
    private:
    vector<uint64_t> deg;

};

signed main() {
    ios::sync_with_stdio(false);
    cin.tie(0);

    int n, m; cin >> n >> m;
    Bitset deg(n + 1);

    while (m--) {
        int a, b; cin >> a >> b;
        deg.rev(a);
        deg.rev(b);
    }

    if (deg.cntones == 0 || deg.cntones == 2) {
        int x; cin >> x;
        cout << (1ull << 64 - __builtin_clzll(x)) - 1 << endl;
    }
    else cout << -1 << endl;

}