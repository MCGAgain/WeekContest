// 30 0.3 0.4
// 1e3 1e4 1e5
#include <iostream>
#include <random>
#define endl '\n'

using namespace std;

signed main(int argc, char** argv) {

    int tag = stoi(argv[1]);
    int seed = stoi(argv[2]);

    ios::sync_with_stdio(false);
    cin.tie(0);

    mt19937 rng(seed);

    int t = 0;

    switch (tag) {
        case 1:
            t = uniform_int_distribution<int>(1, (int)1e3)(rng);
            break;
        case 2:
            t = uniform_int_distribution<int>((int)1e3, (int)1e4)(rng);
            break;
        case 3:
            t = uniform_int_distribution<int>((int)1e4, (int)1e5)(rng);
            break;
    }

    cout << t << '\n';
    while (t--) cout << uniform_int_distribution<int>(1, 100)(rng) << '\n';
    
}