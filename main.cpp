#include <vector>
#include <cmath>
#include <array>
#include <emscripten/emscripten.h>
#include <algorithm>

using namespace std;

struct Branch {
    vector<int> cnt;  // Capacitor counts per type
    double C;         // Total parallel capacitance
    int total;        // Total number of caps in this branch
};

// Global data
vector<double> values;
vector<Branch> branches;
vector<int> dfs_cnt;
array<Branch, 3> bestSol;
double bestDiff;
double target;
int m = 0; // number of capacitor types
int maxPerBranch = 20;

// DFS to generate valid single branch configs
void dfs(int idx, int used) {
    if (idx == m) {
        if (used == 0) return;
        double Csum = 0;
        for (int j = 0; j < m; j++)
            Csum += dfs_cnt[j] * values[j];
        if (Csum <= 0) return;
        branches.push_back({dfs_cnt, Csum, used});
        return;
    }

    for (int c = 0; c + used <= maxPerBranch; c++) {
        dfs_cnt[idx] = c;
        dfs(idx + 1, used + c);
    }
    dfs_cnt[idx] = 0;
}

// Called from JS to set up capacitor types and target
extern "C" {

EMSCRIPTEN_KEEPALIVE
void setup(double* caps, int num, double targetVal, int maxPer) {
    values = vector<double>(caps, caps + num);
    m = num;
    maxPerBranch = maxPer;
    target = targetVal;

    branches.clear();
    dfs_cnt.assign(m, 0);
    dfs(0, 0);
}

// Run the search for best configuration
EMSCRIPTEN_KEEPALIVE
void run() {
    bestDiff = 1e9;
    double invTarget = 1.0 / target;
    const double EPS = 1e-9;

    for (auto& b1 : branches) {
        for (auto& b2 : branches) {
            double inv12 = 1.0 / b1.C + 1.0 / b2.C;
            if (inv12 > invTarget + EPS) continue;
            for (auto& b3 : branches) {
                double Ceq = 1.0 / (inv12 + 1.0 / b3.C);
                double diff = fabs(Ceq - target);
                if (diff < bestDiff) {
                    bestDiff = diff;
                    bestSol = {b1, b2, b3};
                    if (bestDiff < EPS) return;
                }
            }
        }
    }
}

// Get resulting capacitance
EMSCRIPTEN_KEEPALIVE
double get_result() {
    double inv = 1.0 / bestSol[0].C + 1.0 / bestSol[1].C + 1.0 / bestSol[2].C;
    return 1.0 / inv;
}

// Get how many of a type used in a branch
EMSCRIPTEN_KEEPALIVE
int get_config(int branchIdx, int typeIdx) {
    if (branchIdx < 0 || branchIdx >= 3) return -1;
    if (typeIdx < 0 || typeIdx >= m) return -1;
    return bestSol[branchIdx].cnt[typeIdx];
}

}
