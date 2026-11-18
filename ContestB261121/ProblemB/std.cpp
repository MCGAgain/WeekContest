#include <iostream>
#define endl '\n'
using namespace std;

signed main() {
    ios::sync_with_stdio(false);
    cin.tie(0);

    int T; cin >> T;
    while (T--) {
        int p; cin >> p;
        if (p == 1) cout << "WF" << endl;
        else if (p <= 10) cout << "Gold" << endl;
        else if (p <= 30) cout << "Silver" << endl;
        else if (p <= 60) cout << "Bronze" << endl;
        else cout << "Fe" << '\n';
    }

}