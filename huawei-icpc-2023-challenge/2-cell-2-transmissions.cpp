/*
 * score: 5710.904
 */

#include <iostream>
#include <vector>
#include <iomanip>
#include <valarray>
#include <set>
#define DEBUG false
using namespace std;

const int MAX_N = 100;
const int MAX_K = 10;
const int MAX_T = 1000;
const int MAX_R = 10;
const int MAX_J = 5000;
const long double W = 192;
const long double THRESHOLD = 0.000001;

int N, K, T, R, J;
long double s_0[MAX_T][MAX_K][MAX_R][MAX_N];
long double d[MAX_K][MAX_R][MAX_N][MAX_N];
long double p[MAX_T][MAX_K][MAX_R][MAX_N];

struct Query {
    int id;
    long double tbs;
    int n, t_0, l;
};

vector<Query> frame;

inline int b(int t, int k, int r, int n) {
    return p[t][k][r][n] < THRESHOLD ? 0 : 1;
}
inline long double power(int t, int k, int r, int n) {
    return (b(t, k, r, n) ? p[t][k][r][n] : 0);
}

bool is_valid() {
    for (int t = 0; t < T; t++) {
        for (int k = 0; k < K; k++) {
            long double sum = 0;
            for (int r = 0; r < R; r++) {
                long double sum2 = 0;
                for (int n = 0; n < N; n++) {
                    sum2 += power(t,k,r,n);
                }
                sum += sum2;
                if (sum2 > 4 - THRESHOLD) {
                    return false;
                }
            }
            if (sum > R - THRESHOLD) {
                return false;
            }
        }
    }
    return true;
}

void read_input() {
    cin >> N >> K >> T >> R;
    for (int t = 0; t < T; t++) {
        for (int k = 0; k < K; k++) {
            for (int r = 0; r < R; r++) {
                for (int n = 0; n < N; n++) {
                    cin >> s_0[t][k][r][n];
                }
            }
        }
    }

    for (int k = 0; k < K; k++) {
        for (int r = 0; r < R; r++) {
            for (int i = 0; i < N; i++) {
                for (int j = 0; j < N; j++) {
                    cin >> d[k][r][i][j];
                }
            }
        }
    }

    cin >> J;
    frame.resize(J);

    for (int j = 0; j < J; j++) {
        cin >> frame[j].id;
        cin >> frame[j].tbs;
        cin >> frame[j].n;
        cin >> frame[j].t_0;
        cin >> frame[j].l;
    }

}

void print_output() {
    cout << fixed << setprecision(6);
    for (int t = 0; t < T; t++) {
        for (int k = 0; k < K; k++) {
            for (int r = 0; r < R; r++) {
                for (int n = 0; n < N; n++) {
                    cout << power(t, k, r, n);
                    if (n != N - 1)
                        cout << " ";
                }
                if (r != R - 1 || k != K - 1 || t != T - 1)
                    cout << endl;
            }
        }
    }
}

vector<vector<int>> get_related_queries_by_time() {
    vector<vector<int>> related_queries_by_time(T);
    for (int j = 0; j < J; j++) {
        int t_0 = frame[j].t_0;
        int l = frame[j].l;
        for (int t = t_0; t < t_0 + l; t++) {
            related_queries_by_time[t].push_back(j);
        }
    }
    return related_queries_by_time;
}

vector<vector<vector<int>>> min_r(MAX_T, vector<vector<int>>(MAX_K, vector<int>(MAX_N, -1)));
vector<vector<vector<int>>> min2_r(MAX_T, vector<vector<int>>(MAX_K, vector<int>(MAX_N, -1)));
int select_min_r(int t, int k, int n) {
    if (min_r[t][k][n] != -1) {
        return min_r[t][k][n];
    }
    int r = 0;
    for (int i = 0; i < R; i++) {
        if (s_0[t][k][i][n] > s_0[t][k][r][n]) {
            r = i;
        }
    }
    return min_r[t][k][n] = r;
}

int select_min2_r(int t, int k, int n) {
    if (min2_r[t][k][n] != -1) {
        return min2_r[t][k][n];
    }
    int r1 = select_min_r(t, k, n);
    int r = (r1 + 1) % R;
    for (int i = 0; i < R; i++) {
        if (i == r1) continue;
        if (s_0[t][k][i][n] > s_0[t][k][r][n]) {
            r = i;
        }
    }
    return min2_r[t][k][n] = r;
}

set<int> done;

void one_cell_one_transmission(int t, int j) {

    int n = frame[j].n;
    int max_k = -1;
    long double max_g = -1;
    for (int k = 0; k < K; k++) {
        int r = select_min_r(t, k, n);
        long double g = W * log2(1 + s_0[t][k][r][n] * min(3.99l, R - THRESHOLD));
        if (g > max_g) {
            max_g = g;
            max_k = k;
        }
    }
    p[t][max_k][select_min_r(t, max_k, n)][n] = min(3.99l, R - THRESHOLD);

    frame[j].tbs -= W * log2(1 + s_0[t][max_k][select_min_r(t, max_k, n)][n] * min(3.99l, R - THRESHOLD));
    if (frame[j].tbs <= THRESHOLD) {
        done.insert(j);
    }

}

void two_cell_two_transmissions() {
    vector<vector<int>> related_queries_by_time = get_related_queries_by_time();
    for (int t = 0; t < T; t++) {

        vector<int> related_queries;
        for (auto j : related_queries_by_time[t]) {
            if (done.find(j) == done.end()) {
                related_queries.push_back(j);
            }
        }

        if (related_queries.empty()) continue;
        int j1 = related_queries[0];

        if (related_queries.size() < 2) {
            one_cell_one_transmission(t, j1);
            continue;
        }
        int j2 = related_queries[1];

        int n1 = frame[j1].n;
        int n2 = frame[j2].n;

        int max_k1 = -1;
        int max_k2 = -1;
        long double max_g = -1;

        for (int k1 = 0; k1 < K; k1++) {
            int r1 = select_min_r(t, k1, n1);
            for (int k2 = 0; k2 < K; k2++) {
                if (k1 == k2) continue;
                int r2 = select_min_r(t, k2, n2);
                if (r1 == r2) r2 = select_min2_r(t, k2, n2);

                long double g = W * log2(1 + s_0[t][k1][r1][n1] * min(3.99l, R - THRESHOLD)) +
                                W * log2(1 + s_0[t][k2][r2][n2] * min(3.99l, R - THRESHOLD));

                if (g > max_g) {
                    max_g = g;
                    max_k1 = k1;
                    max_k2 = k2;
                }
            }
        }

        p[t][max_k1][select_min_r(t, max_k1, n1)][n1] = min(3.99l, R - THRESHOLD);
        p[t][max_k2][select_min_r(t, max_k2, n2)][n2] = min(3.99l, R - THRESHOLD);

        frame[j1].tbs -= W * log2(1 + s_0[t][max_k1][select_min_r(t, max_k1, n1)][n1] * min(3.99l, R - THRESHOLD));
        frame[j2].tbs -= W * log2(1 + s_0[t][max_k2][select_min_r(t, max_k2, n2)][n2] * min(3.99l, R - THRESHOLD));

        if (frame[j1].tbs <= THRESHOLD) {
            done.insert(j1);
        }
        if (frame[j2].tbs <= THRESHOLD) {
            done.insert(j2);
        }

    }
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);

    if (DEBUG){
        freopen("input.txt", "r", stdin);
        freopen("output.txt", "w", stdout);
    }

    read_input();

    if (DEBUG) {
        cerr << "input read\n";
    }

    sort(frame.begin(), frame.end(), [&](Query& a, Query& b) {
        return (a.t_0 + a.l == b.t_0 + b.l && a.t_0 < b.t_0) || (a.t_0 + a.l < b.t_0 + b.l);
    });

    two_cell_two_transmissions();

    print_output();

    if (DEBUG) {
        cerr << "is_valid: " << is_valid() << endl;
        cerr << done.size() << "\n";
    }
}