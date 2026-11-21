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

    explicit AVERandom(SeedType seed) : seed_(seed), rng_(seed) {}

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
            ++max_k;
        }

        int k = uniform_int_distribution<int>(1, max_k)(rng_);

        ull low = p10[k - 1];
        ull high = (k == 20) ? span : min(span, p10[k] - 1);
        ull offset = uniform_int_distribution<ull>(low, high)(rng_);

        return static_cast<long long>(static_cast<ull>(l) + offset);
    }

    constexpr long long p_10(int x) {
        if (!x) return 1;
        return p10[x];
    }

private:
    typedef unsigned long long ull;

    SeedType seed_;
    Engine rng_;
    struct POW10 {
        constexpr ull& operator[](int i) noexcept { return data[i]; }
        constexpr ull operator[](int i) const noexcept { return data[i]; }
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

// make_hack_x：type 改成由调用者传入，同一个文件里每个用例独立随机
long long make_hack_x(AVERandom<>& gen) {
    int k = gen.next_int(54, 59);          // 2^59 < 1e18 < 2^60
    long long p2 = 1LL << k;
    long long lim = 1LL << (k - 54);       // double 在 2^k 下方的半间距
    int type = gen.next_int(0, 3);
    if (type == 0) return p2 - 1;                         // 卡 floor 版
    if (type == 1) return p2;                             // 卡 ceil(log2(x+1))
    if (type == 2) return p2 + 1;                         // 卡 ceil(log2(x))
    return p2 - gen.next_int(1, max(1LL, lim));           // 随机小偏移，仍会被舍入成 2^k
}

int main(int argc, char** argv) {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    assert(argc >= 3);
    int tag = stoi(argv[1]);
    long long seed = stoll(argv[2]);

    AVERandom<> gen(seed);

    int T = gen.next_log_int(1, 100000);
    cout << T << '\n';

    rep(i, 0, T) {
        long long x = 0;
        switch (tag) {
            case 1: x = gen.next_log_int(1, 2000); break;
            case 2: x = gen.next_log_int(2000, gen.p_10(10)); break;
            case 3: x = gen.next_int(gen.p_10(10), gen.p_10(15)); break;
            case 4: x = gen.next_int(gen.p_10(16), gen.p_10(18)); break;
            case 5: x = make_hack_x(gen); break;
            default: assert(false && "Invalid tag");
        }
        cout << x << '\n';
    }
    return 0;
}