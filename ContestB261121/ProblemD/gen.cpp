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
            ++max_k;
        }

        int k = uniform_int_distribution<int>(0, max_k)(rng_);
        if (k == 0) return l;

        ull low = p10[k - 1];
        ull high = (k == 20) ? span : min(span, p10[k] - 1);
        ull offset = uniform_int_distribution<ull>(low, high)(rng_);

        return static_cast<long long>(static_cast<ull>(l) + offset);
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
    return arr;
}


    static constexpr auto p10 = make_pow10();
};

signed main(int argc, char** argv) {
    ios::sync_with_stdio(false);
    cin.tie(0);

    int tag = stoi(argv[1]);
    long long seed = stoll(argv[2]);

    AVERandom<>gen(seed);


}