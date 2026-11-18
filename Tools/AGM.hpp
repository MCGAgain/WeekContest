#pragma once

namespace Math {
    #define int long long

    inline int qpow(int a, int k, int p) {
        int res = 1;
        a %= p;

        while (k) {
            if (k & 1) res = res * a % p;
            k >>= 1;
            a = a * a % p;
        }

        return res;
    }

    inline int qpow(int a, int k) {
        int res = 1;
        while (k) {
            if (k & 1) res = res * a;
            k >>= 1;
            a = a * a;
        }

        return res;
    }

    inline int log2(int x) {
        if (x == 0) return 0xffffffffll;
        return 63 - __builtin_clz(x);
    }
}