#include <iostream>
#include <random>

using namespace std;

class AVERandom {
private:
    long long seed;
    mt19937 rng;

public:
    AVERandom (long long seed) {
        this->seed = seed;
    }
};

signed main(int argc, char** argv) {
    ios::sync_with_stdio(false);
    cin.tie(0);



}