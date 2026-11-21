#include <iostream>
#define endl '\n'
#define int long long

using namespace std;

signed main() {
    ios::sync_with_stdio(false);
    cin.tie(0);

    int T; cin >> T;
    while (T--) {
        int x; cin >> x; // x 的二进制位数
        cout << (1ll << (64 - __builtin_clzll(x))) - 1 << endl;    
    }
}