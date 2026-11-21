#include <climits>
#include <iostream>
#define endl '\n'
#define int long long

using namespace std;

signed main() {
    ios::sync_with_stdio(false);
    cin.tie(0);

    int T; cin >> T; 
    while (T--) {
        int n, a, b;
        cin >> n >> a >> b;

        int sum = a + b;

        cout << n / sum * a + (n % sum > a ? a : n % sum) << endl;
    }
}
//3 个蓝球， 4 个红球

// 蓝球数量 * 组数
// n % （蓝 + 红）
