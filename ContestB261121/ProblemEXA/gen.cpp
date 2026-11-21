#include <cassert>
#include <algorithm>
#include <iostream>
#include <numeric>
#include <random>
#include <unordered_set>
#include <vector>
#include "jngen.h"

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

class Bitset {
public:
    int cntones = 0;
    Bitset(int n) : deg((n + 63) / 64, 0) {}

    void rev(int i) {
        int idx = i >> 6, bit = i & 63;
        cntones += (deg[idx] & (1ull << bit)) ? -1 : 1;
        deg[idx] ^= (1ull << bit);
    }

private:
    vector<uint64_t> deg;
};

long long make_hack_x(AVERandom<>& gen, long long seed) {
    int k = gen.next_int(54, 59);          // 2^59 < 1e18 < 2^60
    long long p2 = 1LL << k;
    long long lim = 1LL << (k - 54);       // double 在 2^k 下方的半间距
    int type = seed % 4;
    if (type == 0) return p2 - 1;                         // 卡 floor 版
    if (type == 1) return p2;                             // 卡 ceil(log2(x+1))
    if (type == 2) return p2 + 1;                         // 卡 ceil(log2(x))
    return p2 - gen.next_int(1, max(1LL, lim));           // 随机小偏移，仍会被舍入成 2^k
}

int main(int argc, char** argv) {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int tag = stoi(argv[1]);
    long long seed = stoll(argv[2]);

    AVERandom<> gen(seed);
    jngen::rnd.seed(seed); // 同步种子给 jngen 内部随机引擎

    int n = 0, m = 0;
    long long x = 0;

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
            x = gen.next_int(gen.p_10(16), gen.p_10(18));
            break;
        case 5:
            n = gen.next_log_int(100, 2000);
            m = gen.next_log_int(n - 1, 2 * n);
            x = make_hack_x(gen, seed);
    }

    // 统一存放最终生成的所有无向边 (0-indexed)
    vector<pair<int, int>> edge_list;
    edge_list.reserve(m + 4);

    // 掷骰子控制有解比例：1~80 走构造有解 (80%)，81~100 走纯随机 (20%)
    int dice = gen.next_int(1, 100);

    if (dice <= 80) {
        bool want_path = (dice > 40); // 1~40: 欧拉回路(0奇度)；41~80: 欧拉路径(2奇度)

        // 1. 判重哈希表与原子加边工具函数
        unordered_set<uint64_t> edge_set;
        edge_set.reserve(m + 16); // 提前预留桶空间，防止 tag=4 大数据扩容卡顿

        auto encode = [](int u, int v) -> uint64_t {
            if (u > v) swap(u, v);
            return (static_cast<uint64_t>(u) << 32) | static_cast<uint32_t>(v);
        };

        auto try_add_edge = [&](int u, int v) -> bool {
            if (u == v) return false;
            uint64_t key = encode(u, v);
            if (edge_set.count(key)) return false;
            edge_set.insert(key);
            edge_list.emplace_back(u, v);
            return true;
        };

        // 保连通 + 全偶
        vector<int> p(n);
        iota(p.begin(), p.end(), 0);
        for (int i = n - 1; i > 0; --i) {
            int j = gen.next_int(0, i);
            swap(p[i], p[j]);
        }
        rep(i, 0, n) {
            try_add_edge(p[i], p[(i + 1) % n]);
        }

        // 3. 加 3 边，保持全偶
        int fail_cnt = 0;
        while ((int)edge_list.size() + 3 <= m && fail_cnt < 200) {
            int u = gen.next_int(0, n - 1);
            int v = gen.next_int(0, n - 1);
            int w = gen.next_int(0, n - 1);
            if (u == v || v == w || u == w) continue;

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

        // 4. 在全偶图加 1 边
        if (want_path) {
            rep(tries, 0, 500) {
                int u = gen.next_int(0, n - 1);
                int v = gen.next_int(0, n - 1);
                if (try_add_edge(u, v)) break;
            }
        }

        // 5. 打乱边的输出顺序，防止选手从前 n 条边直接看出大环结构
        for (int i = (int)edge_list.size() - 1; i > 0; --i) {
            int j = gen.next_int(0, i);
            swap(edge_list[i], edge_list[j]);
        }
        for (auto& [u, v] : edge_list) {
            if (gen.next_int(0, 1)) swap(u, v);
        }
    } else {
        // 20% 概率：用 jngen 生成纯随机连通图（大概率奇度点 > 2，无解）
        Graph g = jngen::Graph::random(n, m).connected().g().shuffle();
        for (auto [u, v] : g.edges()) {
            edge_list.emplace_back(u, v);
        }
    }

    // 统一输出与度数奇偶性核验（注意用实际生成的 edge_list.size() 作为边数）
    cout << n << ' ' << edge_list.size() << '\n';

    Bitset deg(n + 1);
    for (auto [u, v] : edge_list) {
        cout << u + 1 << ' ' << v + 1 << '\n';
        deg.rev(u);
        deg.rev(v);
    }

    if (deg.cntones == 0 || deg.cntones == 2) {
        cout << x << '\n';
    }

    return 0;
}
