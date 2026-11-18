#include <iostream>
#include <random>
#define endl '\n'

using namespace std;

constexpr long long p10(int k) {
    if (k < 0) return 1;
    long long res = 1;
    while (k--) res *= 10;
    return res; 
}

signed main(int argc, char** argv) {
    int tag = stoi(argv[1]);
    long long seed = stoll(argv[2]);


    mt19937_64 rng(seed);
    int t = 0, p1, p2;

    switch (tag) {
        case 1:
            t = uniform_real_distribution<double>(1, 100)(rng);
            p1 = uniform_real_distribution<double>(1, 5)(rng);
            p2 = uniform_real_distribution<double>(1, 5)(rng);
            break;
        case 2:
            t = uniform_real_distribution<double>(101, 1e3)(rng);
            p1 = uniform_real_distribution<double>(1, 10)(rng);
            p2 = uniform_real_distribution<double>(1, 10)(rng);
            break;
        case 3:
            t = uniform_real_distribution<double>(1e3 + 1, 1e5)(rng);
            p1 = uniform_real_distribution<double>(1, 18)(rng);
            p2 = uniform_real_distribution<double>(1, 18)(rng);
            break;
    }

    cout << t << endl;

    while (t--) {
        long long n = uniform_int_distribution<long long>(p10(p1 - 1), p10(p1))(rng);
        long long a, b;

        long long sum = uniform_int_distribution<long long>(p10(p2 - 1), p10(p2))(rng);

        a = uniform_int_distribution<long long>(0, sum)(rng);
        b = sum - a;
        // 切记不要使用 a + b <= sum 去循环，概率分布函数及其不均匀

        cout << n << ' ' << a << ' ' << b << endl;
    }
}