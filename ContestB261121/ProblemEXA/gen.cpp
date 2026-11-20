#include <cassert>
#include <algorithm>
#include <iostream>
#include <random>
#include "jngen.h"
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

class Bitset {
    public:
    int cntones = 0;
        Bitset(int n): deg((n + 63) / 64, 0) {}

        void rev(int i) {
            int idx = i >> 6, bit = i & 63;
            cntones += (deg[idx] & (1ull << bit)) ? -1 : 1;
            deg[idx] ^= (1ull << bit);
        }
    private:
    vector<uint64_t> deg;

};



signed main(int argc, char** argv) {
    ios::sync_with_stdio(false);
    cin.tie(0);

    int tag = stoi(argv[1]);
    long long seed = stoll(argv[2]);

    AVERandom<> gen(seed);

    #define int long long
    int n, m, x;

    switch (tag) {
        case 1:
            n = gen.next_log_int(10, 200);
            m = gen.next_log_int(n - 1, 2 * n);
            x = gen.next_log_int(1, 2000);
            break;
        case 2:
            n = gen.next_log_int(100, 2000);
            m = gen.next_log_int(n - 1, 2 * n);
            x = gen.next_log_int(2000, gen.p_10(10));
            break;
        case 3:
            n = gen.next_log_int(1000, 200000);
            m = gen.next_log_int(n - 1, 2 * n);
            x = gen.next_int(gen.p_10(10), gen.p_10(15));
            break;
        case 4:
            n = gen.next_log_int(gen.p_10(5), 2 * gen.p_10(6));
            m = gen.next_log_int(n - 1, 2 * n);
            x = gen.next_int(gen.p_10(15), gen.p_10(18));
            break;
    }


    vector<int> edges(n);
    rep(i, 0, n) edges[i] = i;
    rep(i, 0, n) {
        int j = gen.next_int(0, n - 1);
        swap(edges[i], edges[j]);
    }

    // 1：把无向边 (u, v) 压缩成一个 64 位整数，用于 O(1) 判重边
    unordered_set<uint64_t> edge_set;
    auto encode = [](int u, int v) -> uint64_t {
        if (u > v) swap(u, v);  //判重
        return (static_cast<uint64_t>(u) << 32) | static_cast<uint32_t>(v);
    };

    rep(i, 0, n) {
        edge_set.insert(encode(edges[i], (edges[i + 1]) % edges.size()));
    }

    // 2：循环控制与熔断器
    int fail_cnt = 0; 
    while ((int)edges.size() + 3 <= m && fail_cnt < 200) {
        // 3：随机挑 3 个互不相同的点
        int u = gen.next_int(0, n - 1);
        int v = gen.next_int(0, n - 1);
        int w = gen.next_int(0, n - 1);
        if (u == v || v == w || u == w) continue;

        // 4：原子性检查——必须 3 条边全都不存在，才一次性全加进去
        if (!edge_set.count(encode(u, v)) &&
            !edge_set.count(encode(v, w)) &&
            !edge_set.count(encode(w, u))) {
            try_add_edge(u, v);
            try_add_edge(v, w);
            try_add_edge(w, u);
            fail_cnt = 0;
        } else {
            ++fail_cnt;
        }
    }





    Graph g = jngen::Graph::random(n, m).connected().g().shuffle();
    Bitset deg(n + 1);
    
    cout << g.printN().printM().add1() << '\n';




    for (auto [u, v] : g.edges()) {
        deg.rev(u);
        deg.rev(v);
    }

    if (deg.cntones == 0 || deg.cntones == 2) {
        cout << x << '\n';
    }

}