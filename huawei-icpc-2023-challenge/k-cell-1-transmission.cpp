#include <iostream>
#include <vector>
#include <iomanip>
#include <valarray>
#include <cassert>

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
vector<vector<int>> frame;
long double p[MAX_T][MAX_K][MAX_R][MAX_N];

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
    frame.resize(J, vector<int>(5));

    for (int j = 0; j < J; j++) {
        for (int i = 0; i < 5; i++) {
            cin >> frame[j][i];
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

    vector<int> used(T, 0);

    int score = 0;

    sort(frame.begin(), frame.end(), [&](vector<int>& a, vector<int>& b) {
        return a[4] < b[4];
    });

    for (int j = 0; j < J; j++) {
        int id = frame[j][0];
        int tbs = frame[j][1];
        int n = frame[j][2];
        int t_0 = frame[j][3];
        int l = frame[j][4];
        long double g = 0;
        vector<pair<long double, pair<int, int>>> candidates;
        for (int t = t_0; t < t_0 + l; t++) {
            if (used[t]) continue;
            for (int r = 0; r < R; r++) {
                long double sum = 0;
                for (int k = 0; k < K; k++) {
                    sum += log2(1 + s_0[t][k][r][n] * min(3.99l, R - THRESHOLD));
                }
                candidates.push_back({sum, {t, r}});
            }
        }
        sort(candidates.begin(), candidates.end());
        reverse(candidates.begin(), candidates.end());

        for (int i = 0; i < candidates.size() && tbs > g; i++) {
            int t = candidates[i].second.first;
            int r = candidates[i].second.second;
            if (used[t]) continue;
            used[t] = 1;
            for (int k = 0; k < K; k++) {
                g += W * log2(1 + s_0[t][k][r][n] * min(3.99l, R - THRESHOLD));
                p[t][k][r][n] = min(3.99l, R - THRESHOLD);
            }
        }
        for (int i = 0; i < candidates.size() && tbs > g; i++) {
            int t = candidates[i].second.first;
            int r = candidates[i].second.second;
            if (used[t]) continue;
            used[t] = 0;
            for (int k = 0; k < K; k++) {
                p[t][k][r][n] = 0;
            }
        }
        if (tbs <= g) {
            score++;
        }

    }
//    assert(is_valid());
    print_output();

    if (DEBUG) {
        cerr << "is_valid: " << is_valid() << endl;
        cerr << score << "\n";
    }
}