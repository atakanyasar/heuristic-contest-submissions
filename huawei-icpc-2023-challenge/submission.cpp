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

//        frame[j].tbs *= 1.5;
        frame[j].tbs /= W;
        frame[j].tbs = 1.0 / pow(2.0, frame[j].tbs);
        if (DEBUG) {
//            cerr << "frame[" << j << "].tbs: " << frame[j].tbs << endl;
        }
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

        if (DEBUG) {
            cerr << "time: " << t << endl;
            cerr << "related queries: ";
        }

        vector<int> related_queries;
        for (auto j : related_queries_by_time[t]) {
            if (done.find(j) == done.end()) {
                related_queries.push_back(j);
                if (DEBUG) {
                    cerr  << j << " ";
                }
            }
        }
        if (DEBUG) {
            cerr << endl;
        }

        if (related_queries.empty()) continue;

        vector<int> used_r(R, -1);
        vector<vector<bool>> used_kn(K, vector<bool>(N, false));
        vector<vector<long double>> n_limit(K, vector<long double>(R, 3.999));
        vector<long double> nr_limit(K, R-THRESHOLD*2);

        for (auto j : related_queries) {
            vector<pair<pair<long double, int>, pair<int, int>>> sorted;
            int n = frame[j].n;
            for (int k = 0; k < K; k++) {
                for (int r = 0; r < R; r++) {
                    sorted.push_back({{s_0[t][k][r][n], j},
                                      {k,               r}});
                }
            }
            sort(sorted.begin(), sorted.end());
            reverse(sorted.begin(), sorted.end());


            for (auto P: sorted) {
                int k = P.second.first;
                int r = P.second.second;

                if (done.count(j)) continue;
                if (used_r[r] != -1 && used_r[r] != n) continue;
                if (used_kn[k][n]) continue;
                if (n_limit[k][r] < THRESHOLD) continue;
                if (nr_limit[k] < THRESHOLD) continue;

                p[t][k][r][n] = min({n_limit[k][r], nr_limit[k], (1.0 / frame[j].tbs - 1) / s_0[t][k][r][n] + THRESHOLD});
                frame[j].tbs *= (1 + s_0[t][k][r][n] * p[t][k][r][n]);
                used_r[r] = n;
                used_kn[k][n] = true;
                n_limit[k][r] -= p[t][k][r][n];
                nr_limit[k] -= p[t][k][r][n];

                if (DEBUG) {
//                    cerr << "used (j=" << j << ", r=" << r << ", k=" << k << "): " << p[t][k][r][n] << endl;
                }

                if (frame[j].tbs >= 1) {
                    done.insert(j);

                    if (DEBUG) {
                        cerr << "done: " << j << endl;
                    }
                }
            }
        }

        if (false && DEBUG) {
            for (int k = 0; k < K; k++) {
                cerr << "nr_limit[" << k << "]: " << nr_limit[k] << endl;
            }
            for (int k = 0; k < K; k++) {
                for (int r = 0; r < R; r++) {
                    cerr << "n_limit[" << k << "][" << r << "]: " << n_limit[k][r] << endl;
                }
            }

        }
    }
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);

    if (DEBUG){
        freopen("tests/21", "r", stdin);
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