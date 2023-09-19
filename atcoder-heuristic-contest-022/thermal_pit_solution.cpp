#include <iostream>
#include <utility>
#include <vector>

using namespace std;

struct Position {
    int x, y;
};

struct Judge {
    static void set_temperature(const vector<vector<int>>& temperature) {
        for (const vector<int>& row : temperature) {
            for (int i = 0; i < row.size(); i++) {
                cout << row[i] << (i == row.size() - 1 ? "\n" : " ");
            }
        }
        cout.flush();
    }

    static int measure(int i, int x, int y) {
        cout << i << " " << x << " " << y << endl;
        int v;
        cin >> v;
        if (v == -1) {
            cerr << "something went wrong. i=" << i << " x=" << x << " y=" << y << endl;
            exit(1);
        }
        return v;
    }

    static void answer(const vector<int>& estimate) {
        cout << "-1 -1 -1" << endl;
        for (int e : estimate) {
            cout << e << endl;
        }
    }
};

struct Environment {
    const int L;
    const int N;
    const int S;
    const vector<Position> wormholes;
    int window_size = 3;
    int remeasurements;
    int multiplier;

    Environment(int L, int N, int S, vector<Position>& wormholes) : L(L), N(N), S(S), wormholes(wormholes) {
        if (S > 16) {
            multiplier = 3;
            remeasurements = 10;
        }
        else if(S > 9) {
            multiplier = 3;
            remeasurements = 8;
        }
        else if(S > 4) {
            multiplier = 3;
            remeasurements = 2;
        }
        else if(S > 1) {
            multiplier = 3;
            remeasurements = 1;
        }
        else {
            multiplier = 1;
            remeasurements = 1;
        }
    }

    Position move(Position pos, int dx, int dy) {
        pos.x += dx;
        pos.y += dy;
        if (pos.x < 0) {
            pos.x = L - 1;
        }
        if (pos.x >= L) {
            pos.x = 0;
        }
        if (pos.y < 0) {
            pos.y = L - 1;
        }
        if (pos.y >= L) {
            pos.y = 0;
        }
        return pos;
    }
};

struct Solver {
    Environment env;

    int L;
    int N;
    int S;

    explicit Solver(Environment env) : env(std::move(env)), L(env.L), N(env.N), S(env.S) {

    }

    int measure(int i, int x, int y) const {
        int sum = 0;
        for (int measurements = 0; measurements < env.remeasurements; measurements++) {
            sum += Judge::measure(i, x, y);
        }
        return sum / env.remeasurements;
    }

    struct Logic {
        static int difference(vector<vector<int>>& A, vector<vector<int>>& B) {
            int diff = 0;
            for(int i = 0; i < A.size(); i++) {
                for(int j = 0; j < A[i].size(); j++){
                    diff += (A[i][j] - B[i][j]) * (A[i][j] - B[i][j]);
                }
            }
            return diff;
        }
        static void print(vector<vector<int>>& A, vector<vector<int>>& B) {
            for(int i = 0; i < A.size(); i++) {
                for(int j = 0; j < A[i].size(); j++){
                    cerr << A[i][j] << " ";
                }
                cerr << "   ";
                for(int j = 0; j < B[i].size(); j++){
                    cerr << B[i][j] << " ";
                }
                cerr << endl;
            }
        }
    };

    void solve() {
        const vector<vector<int>> temperature = create_temperature();
        Judge::set_temperature(temperature);
        const vector<int> estimate = predict(temperature);
        Judge::answer(estimate);
    }

    vector<vector<int>> create_temperature() const {
        vector<vector<int>> temperature(L, vector<int>(L, 0));
        for (int i = 0; i < L; i++) {
            for(int j = 0; j < L; j++){
                temperature[i][j] = 500 + env.multiplier * (abs(i-L/2) * abs(j-L/2) / 2 - 500/env.multiplier);
            }
        }
        return temperature;
    }

    vector<int> predict(const vector<vector<int>>& temperature) {
        int window_size = env.window_size;
        vector<int> estimate(N);
        for (int i_in = 0; i_in < N; i_in++) {

            vector<vector<int>> measurement_matrix(window_size, vector<int>(window_size, 0));
            for(int i = 0; i < window_size; i++) {
                for(int j = 0; j < window_size; j++) {
                    measurement_matrix[i][j] = measure(i_in, i-1, j-1);
                }
            }

            int min_diff = 9999;
            for (int i_out = 0; i_out < N; i_out++) {
                const Position& pos = env.wormholes[i_out];
                vector<vector<int>> hole_matrix(window_size, vector<int>(window_size, 0));

                for(int i = 0; i < window_size; i++) {
                    for(int j = 0; j < window_size; j++){
                        Position p = env.move(pos, i-1, j-1);
                        hole_matrix[i][j] = temperature[p.x][p.y];
                    }
                }
                int diff = Logic::difference(measurement_matrix, hole_matrix);
                if (diff < min_diff) {
                    min_diff = diff;
                    estimate[i_in] = i_out;
                }
            }
        }
        return estimate;
    }
};

int main() {
    int L, N, S;
    cin >> L >> N >> S;
    vector<Position> wormholes(N);
    for (int i = 0; i < N; i++) {
        cin >> wormholes[i].x >> wormholes[i].y;
    }
    Environment env(L, N, S, wormholes);
    Solver solver(env);
    solver.solve();

}
