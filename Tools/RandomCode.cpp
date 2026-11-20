#include <cassert>
#include <algorithm>
#include <iostream>
#include <chrono>
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

signed main() {
    long long seed = chrono::steady_clock::now().time_since_epoch().count();

    time_t t = std::time(nullptr);

    tm* now = std::localtime(&t);

    int year  = now->tm_year + 1900; // tm_year 记录的是从 1900 年起经过的年数
    int month = now->tm_mon + 1;     // tm_mon 范围是 [0, 11]，必须 +1
    int day   = now->tm_mday;        // tm_mday 范围是 [1, 31]，无需 +1

    AVERandom<> gen(seed);

    int code = gen.next_int(1, 9999);

    char op; cin >> op;
    if (op == 'B' || op == 'b') {
        printf("SUSEOAAB%d%02d%02d%04d\n", year, month, day, code);
    }
    else if (op == 'R' || op == 'r') {
        printf("SUSEOAAR%d%02d%02d%04d\n", year, month, day, code);
    }
    else return 0;


    
}