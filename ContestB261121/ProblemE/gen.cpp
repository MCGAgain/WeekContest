#include <cassert>
#include <algorithm>
#include <iostream>
#include <random>
#define rep(i, a, b) for (int i = (a); i < (b); i++)
#define rrep(i, a, b) for (int i = (a); i <= (b); i++)

using namespace std;

const int N = 1e4 + 10;
long long a[N], b[N];

template<typename Engine = mt19937_64>
class AVERandom {

public:
    using SeedType = typename Engine::result_type;

    explicit AVERandom (SeedType seed): seed_(seed), rng_(seed) {}

    long long next_int(long long l, long long r) {
        assert(l <= r);
        return uniform_int_distribution<long long>(l, r)(rng_);
    }

    long long next_log_int(long long l, long long r) {
        assert(l <= r);
        if (l == r) return l;

        ull span = static_cast<ull>(r) - static_cast<ull>(l);

        int max_k = 0;
        while (max_k < 20 && p10[max_k] <= span) {
            ++max_k;    //取 span 数位，r 与 l 之间相差的数量级
        }

        // 分桶
        int k = uniform_int_distribution<int>(1, max_k)(rng_);

        ull low = p10[k - 1]; // p10[0] 是 0，这里被我修正过，桶内更均匀一点
        ull high = (k == 20) ? span : min(span, p10[k] - 1);
        ull offset = uniform_int_distribution<ull>(low, high)(rng_);

        return static_cast<long long>(static_cast<ull>(l) + offset);
    }

    // x 上限是 19.不要做傻事
    constexpr long long p_10(int x) {
        if (!x) return 1;
        return p10[x];
    }

private:
    typedef unsigned long long ull;

    SeedType seed_;
    Engine rng_;
    struct POW10 {

        constexpr ull& operator[](int i) noexcept {
            return data[i];
        }

        constexpr ull operator[](int i) const noexcept {
            return data[i];
        }

        private:
            ull data[20];
    };

    static constexpr auto make_pow10() {
        POW10 arr{};
        arr[0] = 1;
        rep(i, 1, 20) arr[i] = arr[i - 1] * 10ull;
        arr[0] = 0;
        return arr;
    }

    static constexpr auto p10 = make_pow10();
};

signed main(int argc, char** argv) {
    ios::sync_with_stdio(false);
    cin.tie(0);

    int tag = stoi(argv[1]);
    long long seed = stoll(argv[2]);

    AVERandom<> gen(seed);

    long long A, B, M;

    switch (tag) {
        case 1:
            A = gen.next_int(1, 100);
            B = gen.next_int(1, 100);
            M = gen.next_int(1, 50);
            break;
        case 2:
            A = gen.next_int(100, 10000);
            B = gen.next_int(100, 10000);
            M = gen.next_int(100, 10000 / 2);
            break;
    }

    cout << A << ' ' << B << ' ' << M << '\n';

    rrep(i, 1, A) cout << (a[i] = gen.next_log_int(1, 10000)) << " \n"[i == A];
    rrep(i, 1, B) cout << (b[i] = gen.next_int(1, 10000)) << " \n"[i == B];
    rrep(i, 1, M) {
        
        long long x = gen.next_int(1, A);
        long long y = gen.next_int(1, B);
        long long c = gen.next_log_int(1, max((a[x] + b[y]) / 2, a[x] + b[y] - gen.next_int(0, 100)));

        cout << x << ' ' << y << ' ' << c << '\n';
    }
}