#include <iostream>
using namespace std;

const int MAX_N = 100;
const int MAX_K = 10;
const int MAX_T = 1000;
const int MAX_R = 10;
const int MAX_J = 5000;

int N, K, T, R, J;
long double s_0[MAX_T][MAX_K][MAX_R][MAX_N];
long double d[MAX_K][MAX_R][MAX_N][MAX_N];
int frame[MAX_J][5];

void print_input() {
    cout << N << " " << K << " " << T << " " << R << "\n";
    for (int t = 0; t < T; t++) {
        for (int k = 0; k < K; k++) {
            for (int r = 0; r < R; r++) {
                for (int n = 0; n < N; n++) {
                    cout << s_0[t][k][r][n] << " ";
                }
                cout << "\n";
            }
        }
    }

    for (int k = 0; k < K; k++) {
        for (int r = 0; r < R; r++) {
            for (int i = 0; i < N; i++) {
                for (int j = 0; j < N; j++) {
                    cout << d[k][r][i][j] << " ";
                }
                cout << "\n";
            }
        }
    }

    cout << J << "\n";
    for (int j = 0; j < J; j++) {
        for (int i = 0; i < 5; i++) {
            cout << frame[j][i] << " ";
        }
        cout << "\n";
    }

}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);

    freopen("input.txt", "w", stdout);

    N = MAX_N/2;
    K = MAX_K/2;
    T = MAX_T/2;
    R = MAX_R/2;
    J = MAX_J/2;

    for (int t = 0; t < T; t++) {
        for (int k = 0; k < K; k++) {
            for (int r = 0; r < R; r++) {
                for (int n = 0; n < N; n++) {
                    s_0[t][k][r][n] = 100.0 * rand() / RAND_MAX;
                }
            }
        }
    }

    for (int k = 0; k < K; k++) {
        for (int r = 0; r < R; r++) {
            for (int i = 0; i < N; i++) {
                d[k][r][i][i] = 0;
                for (int j = i + 1; j < N; j++) {
                    d[k][r][i][j] = -1.0 * rand() / RAND_MAX;
                    d[k][r][j][i] = d[k][r][i][j];
                }
            }
        }
    }

    for (int j = 0; j < J; j++) {
        frame[j][0] = j;
        frame[j][1] = 10000.0 * rand() / RAND_MAX;
        frame[j][2] = rand() % N;
        frame[j][3] = rand() % (T - 100) + 1;
        frame[j][4] = rand() % 100 + 1;
    }

    print_input();
}