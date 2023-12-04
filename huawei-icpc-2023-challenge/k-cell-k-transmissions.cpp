#include <iostream>
#include <vector>
#include <iomanip>
#include <valarray>
#include <set>

#define DEBUG true
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

        frame[j].tbs /= W;
        frame[j].tbs = 1.0 / pow(2.0, frame[j].tbs);
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


set<int> done;

void k_cell_k_transmissions() {
    vector<vector<int>> related_queries_by_time = get_related_queries_by_time();
    for (int t = 0; t < T; t++) {

        vector<int> related_queries;
        for (auto j : related_queries_by_time[t]) {
            if (done.find(j) == done.end()) {
                related_queries.push_back(j);
            }
        }

        if (related_queries.empty()) continue;

        vector<bool> used_r(R, false);
        vector<bool> used_k(N, false);

        for (auto j : related_queries) {
            int n = frame[j].n;
            int max_k = -1;
            int max_r = -1;
            long double max_s = -1;
            for (int k = 0; k < K; k++) {
                if (used_k[k]) continue;
                for (int r = 0; r < R; r++) {
                    if (used_r[r]) continue;
                    if (s_0[t][k][r][n] > max_s) {
                        max_s = s_0[t][k][r][n];
                        max_k = k;
                        max_r = r;
                    }
                }
            }
            if (max_k != -1) {
                p[t][max_k][max_r][n] = min({3.99l, R - THRESHOLD, (1.0 / frame[j].tbs - 1) / s_0[t][max_k][max_r][n] + THRESHOLD});
                frame[j].tbs *= (1 + s_0[t][max_k][max_r][n] * p[t][max_k][max_r][n]);
                used_r[max_r] = true;
                used_k[max_k] = true;
            }

            if (frame[j].tbs >= 1) {
                done.insert(j);
            }
        }
    }
    cerr << endl;
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);

    if (DEBUG){
        freopen("tests/06", "r", stdin);
        freopen("output.txt", "w", stdout);
    }

    read_input();

    if (DEBUG) {
        cerr << "input read\n";
    }

    sort(frame.begin(), frame.end(), [&](Query& a, Query& b) {
        return (a.tbs == b.tbs && a.t_0 + a.l < b.t_0 + b.l) || (a.tbs > b.tbs);
//        return (a.t_0 + a.l == b.t_0 + b.l && a.t_0 < b.t_0) || (a.t_0 + a.l < b.t_0 + b.l);
    });

    k_cell_k_transmissions();

    print_output();

    if (DEBUG) {
        cerr << "is_valid: " << is_valid() << endl;
        cerr << done.size() << "\n";
    }
}