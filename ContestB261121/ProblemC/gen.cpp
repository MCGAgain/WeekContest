#include <cassert>
#include <algorithm>
#include <iostream>
#include <random>
#define rep(i, a, b) for (int i = (a); i < (b); i++)
#define rrep(i, a, b) for (int i = (a); i <= (b); i++)

using namespace std;

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
    //int tag = 1, seed = 1;

    AVERandom<> gen(seed);

    int t, l, r;

    switch (tag) {
        case 1:
            t = gen.next_log_int(1, gen.p_10(3) - 1);
            l = 1, r = 4;
            break;
        case 2:
            t = gen.next_int(gen.p_10(3), gen.p_10(4) - 1);
            l = 4, r = 12;
            break;
        case 3:
            t = gen.next_log_int(gen.p_10(4), gen.p_10(5) - 1);
            l = 12, r = 18;
            break;
    }

    cout << t << endl;

    while (t--) {
        long long digit_n = gen.next_int(l, r);
        long long digit_sum = gen.next_int(l, r);

        long long sum;
        if (digit_sum <= 3) sum = gen.next_int(0, gen.p_10(digit_sum));
        else  sum = gen.next_log_int(0, gen.p_10(digit_sum) - 1);

        long long a;
        if (gen.next_int(1, 1000) == 1) {
            a = gen.next_int(0, 1) ? 0 : sum;
        } else {
            a = gen.next_log_int(0, sum);
        }
        long long b = sum - a;

        long long n = gen.next_log_int(1, gen.p_10(digit_n));

        cout << n << ' ' << a << ' ' << b << endl;
    }
}