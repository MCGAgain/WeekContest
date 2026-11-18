#include <iostream>
#include <random>

using namespace std;

signed main(int argc, char** argv) {
    int tag = stoi(argv[1]);
    long long seed = stoll(argv[2]);

    mt19937 rng(seed);

    int a, b;

    switch (tag) {
        case 1:
        do {
            a = uniform_int_distribution<int>((int)-1e9, (int)1e9)(rng);
            b = uniform_int_distribution<int>((int)-1e9, (int)1e9)(rng);
        } while ((a + b) < (int)-1e9 || (a + b) > (int)1e9);
        break;
    }

    cout << a << ' ' << b << '\n';
}