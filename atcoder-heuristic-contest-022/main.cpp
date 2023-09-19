#include <iostream>
#include <random>
#include <utility>
#include <vector>
#include <cmath>
#include <queue>
#include <algorithm>

using namespace std;

struct Position {
    int x, y;
};

struct Judge {
    static void set_temperature(vector<vector<int>>& temperature) {
        for (vector<int>& row : temperature) {
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

    static int measure_n(int i, int x, int y, int n) {
        int sum = 0;
        for (int measurements = 0; measurements < n; measurements++) {
            sum += measure(i, x, y);
        }
        return sum / n;
    }

    static void answer(vector<int>& estimate) {
        cout << "-1 -1 -1" << endl;
        for (int e : estimate) {
            cout << e << endl;
        }
    }
};

struct Environment {
    int L;
    int N;
    int S;
    vector<Position> wormholes;
    vector<vector<int>> temperature;
    int remeasurements = 1;
    int initial_temperature = 400;
    int max_temperature = 1000;
    double elimination_threshold = 0.65;
    int bitmask_solution_threshold = 400;

    Environment(int L, int N, int S, vector<Position>& wormholes) : L(L), N(N), S(S), wormholes(wormholes) {
        if (S <= 25) {
            initial_temperature = 100;
            max_temperature = 200;
            remeasurements = 1;
            elimination_threshold = 0.75;

        } else if (S <= 81) {
            initial_temperature = 200;
            max_temperature = 500;
            remeasurements = 1;
            elimination_threshold = 0.75;
        } else if (S <= 144) {
            elimination_threshold = 0.75;
        } else if (S <= 225) {
            remeasurements = 2;
            elimination_threshold = 0.85;
        } else if (S <= 400) {
            remeasurements = 3;
            initial_temperature = 30;
            elimination_threshold = 0.6;
        } else {
            remeasurements = 3;
            initial_temperature = 30;
            elimination_threshold = 0.6;
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

    Position min_move_to(Position from, Position to) {
        int dx = to.x - from.x;
        int dy = to.y - from.y;
        if (dx > L / 2) {
            dx -= L;
        }
        if (dx < -L / 2) {
            dx += L;
        }
        if (dy > L / 2) {
            dy -= L;
        }
        if (dy < -L / 2) {
            dy += L;
        }
        return {dx, dy};
    }

} *env;

struct TemperatureCreator {

    static vector<vector<int>> create() {
        if (env->S >= env->bitmask_solution_threshold)
            return bitmask_distribution();
        return pit();
    }

    static vector<vector<int>> spiral() {
        vector<vector<int>> temperature(env->L, vector<int>(env->L, -1));
        int i = 0;
        int j = 0;
        int last = env->initial_temperature;
        int dlast = -1;
        int dx = 0;
        int dy = 1;
        while(true) {
            temperature[i][j] = last;
            last += dlast;
            last = max(0, min(last, env->initial_temperature));
            if (last == env->initial_temperature || last == 0) {
                dlast = -dlast;
            }
            if (i + dx < env->L && i + dx >= 0 && j + dy < env->L && j + dy >= 0 && temperature[i + dx][j + dy] == -1) {
                i += dx;
                j += dy;
            } else if (j + 1 < env->L && temperature[i][j + 1] == -1) {
                dx = 0;
                dy = 1;
                i += dx;
                j += dy;
            } else if (i + 1 < env->L && temperature[i + 1][j] == -1) {
                dx = 1;
                dy = 0;
                i += dx;
                j += dy;
            } else if (j - 1 >= 0 && temperature[i][j - 1] == -1) {
                dx = 0;
                dy = -1;
                i += dx;
                j += dy;
            } else if (i - 1 >= 0 && temperature[i - 1][j] == -1) {
                dx = -1;
                dy = 0;
                i += dx;
                j += dy;
            } else {
                break;
            }

        }
        temperature[env->L/2][env->L/2] = env->max_temperature;
        return temperature;
    }


    static vector<vector<int>> pit() {
        vector<vector<int>> temperature(env->L, vector<int>(env->L, 0));
        for (int i = 0; i < env->L; i++) {
            for(int j = 0; j < env->L; j++){
                temperature[i][j] = env->initial_temperature/2 + env->initial_temperature * ((abs(i-env->L/2) + abs(j-env->L/2))*1.0/env->L-0.5);
            }
        }
        temperature[env->L/2][env->L/2] = env->max_temperature;
        return temperature;
    }

    static vector<vector<int>> bitmask_distribution() {
        vector<vector<int>> temperature(env->L, vector<int>(env->L, 0));
        vector<vector<vector<int>>> patterns;
        for(int mask = 0; mask < (1 << 9); mask++) {
            vector<vector<int>> pattern(3, vector<int>(3, 0));
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    pattern[i][j] = env->max_temperature * ((mask >> (i * 3 + j)) & 1);
                }
            }
            patterns.push_back(pattern);
        }
        shuffle(patterns.begin(), patterns.end(), std::mt19937(std::random_device()()));

        vector<vector<bool>> used(env->L, vector<bool>(env->L, false));
        vector<bool> used_patterns(patterns.size(), false);

        for (int i = 0; i < env->N; i++) {
            Position p = env->wormholes[i];
            vector<vector<int>> pattern(3, vector<int>(3, -1));
            for(int j = 0; j < 9; j++) {
                Position pp = env->move(p, j / 3 - 1, j % 3 - 1);
                if (used[pp.x][pp.y]) {
                    pattern[j / 3][j % 3] = temperature[pp.x][pp.y];
                }
                for(int k = 0; k < patterns.size(); k++) {
                    if (used_patterns[k]) {
                        continue;
                    }
                    bool ok = true;
                    for(int x = 0; x < 3; x++) {
                        for(int y = 0; y < 3; y++) {
                            if (pattern[x][y] != -1 && pattern[x][y] != patterns[k][x][y]) {
                                ok = false;
                            }
                        }
                    }
                    if (ok) {
                        pattern = patterns[k];
                        used_patterns[k] = true;
                        break;
                    }
                }
                for (int x = 0; x < 3; x++) {
                    for(int y = 0; y < 3; y++) {
                        if (pattern[x][y] != -1) {
                            Position pp = env->move(p, x - 1, y - 1);
                            temperature[pp.x][pp.y] =  pattern[x][y];
                            used[pp.x][pp.y] = true;
                        }
                    }
                }
            }
        }
        return temperature;
    }
};

struct Measurements : vector<pair<int, int>> {
    double m = 0.01;

    double difference() const {
        if (empty()) return 10000000.0;
        double diff = 0;
        for(auto& measurement : (*this)) {
            double x = abs(measurement.first - measurement.second);
            diff += (exp(m*(x - env->S/2.0 + 0.693147181))
                     - exp(m*(-env->S/2.0 + 0.693147181))) / m;
        }
        return min(diff / (double)size(), 10000000.0);
    }

    bool operator<(const Measurements& other) const {
        return difference() < other.difference();
    }

};

struct CompatibilityMatrix : vector<vector<Measurements>> {

    CompatibilityMatrix() : vector<vector<Measurements>>() {}
    explicit CompatibilityMatrix(int N) : vector<vector<Measurements>>(N, vector<Measurements>(N)) {}

    struct AssignmentProblem {

        static vector<int> assignment(vector<vector<int>> a) {
            int INF = 1000 * 1000 * 1000;
            int n = a.size();
            int m = n * 2 + 2;
            vector<vector<int>> f(m, vector<int>(m));
            int s = m - 2, t = m - 1;
            int cost = 0;
            while (true) {
                vector<int> dist(m, INF);
                vector<int> p(m);
                vector<bool> inq(m, false);
                queue<int> q;
                dist[s] = 0;
                p[s] = -1;
                q.push(s);
                while (!q.empty()) {
                    int v = q.front();
                    q.pop();
                    inq[v] = false;
                    if (v == s) {
                        for (int i = 0; i < n; ++i) {
                            if (f[s][i] == 0) {
                                dist[i] = 0;
                                p[i] = s;
                                inq[i] = true;
                                q.push(i);
                            }
                        }
                    } else {
                        if (v < n) {
                            for (int j = n; j < n + n; ++j) {
                                if (f[v][j] < 1 && dist[j] > dist[v] + a[v][j - n]) {
                                    dist[j] = dist[v] + a[v][j - n];
                                    p[j] = v;
                                    if (!inq[j]) {
                                        q.push(j);
                                        inq[j] = true;
                                    }
                                }
                            }
                        } else {
                            for (int j = 0; j < n; ++j) {
                                if (f[v][j] < 0 && dist[j] > dist[v] - a[j][v - n]) {
                                    dist[j] = dist[v] - a[j][v - n];
                                    p[j] = v;
                                    if (!inq[j]) {
                                        q.push(j);
                                        inq[j] = true;
                                    }
                                }
                            }
                        }
                    }
                }

                int curcost = INF;
                for (int i = n; i < n + n; ++i) {
                    if (f[i][t] == 0 && dist[i] < curcost) {
                        curcost = dist[i];
                        p[t] = i;
                    }
                }
                if (curcost == INF)
                    break;
                cost += curcost;
                for (int cur = t; cur != -1; cur = p[cur]) {
                    int prev = p[cur];
                    if (prev != -1)
                        f[cur][prev] = -(f[prev][cur] = 1);
                }
            }

            vector<int> answer(n);
            for (int i = 0; i < n; ++i) {
                for (int j = 0; j < n; ++j) {
                    if (f[i][j + n] == 1)
                        answer[i] = j;
                }
            }
            return answer;
        }
    };

    vector<int> assignment() {
        vector<vector<int>> a(size(), vector<int>(size()));
        for(int i = 0; i < size(); i++) {
            for(int j = 0; j < size(); j++) {
                a[i][j] = (int)(*this)[i][j].difference();
            }
        }
        return AssignmentProblem::assignment(a);
    }

    int measure(int row, int x, int y, int n = 1) {
        int measurement = Judge::measure_n(row, x, y, n);
        for(int col = 0; col < env->N; col++) {
            Position p = env->move(env->wormholes[col], x, y);
            int expected = env->temperature[p.x][p.y];
            (*this)[row][col].push_back({expected, measurement});
        }
        return measurement;
    }
    void measure_random(int row) {
        int x = rand() * rand() % env->L - env->L / 2;
        int y = rand() * rand() % env->L - env->L / 2;
        measure(row, x, y);
    }

    void print() {
        for(auto& row : *this) {
            for(auto& col : row) {
                fprintf(stderr, "%4d ", (int)col.difference());
            }
            cerr << endl;
        }
    }
};

struct Solver {
    CompatibilityMatrix compatibility_matrix;

    int N;

    explicit Solver() : N(env->N) {
        compatibility_matrix = CompatibilityMatrix(N);
    }

    void solve() {
        env->temperature = TemperatureCreator::create();
        Judge::set_temperature(env->temperature);
        vector<int> estimate = predict();
        Judge::answer(estimate);
    }


    vector<int> predict() {
        vector<int> estimate(N, -1);

        if(env->S >= env->bitmask_solution_threshold) {
            for (int i_in = 0; i_in < N && env->initial_temperature; i_in++) {
                for (int i = -1; i <= 1; i++) {
                    for (int j = -1; j <= 1; j++) {
                        compatibility_matrix.measure(i_in, i, j, 11);
                    }
                }
            }
            return compatibility_matrix.assignment();
        }

        for (int i_in = 0; i_in < N && env->initial_temperature; i_in++) {
            compatibility_matrix.measure(i_in, 0, 0, env->remeasurements);
            compatibility_matrix.measure(i_in, 2,  2, env->remeasurements);
            compatibility_matrix.measure(i_in, -2,  -2, env->remeasurements);
        }
        for (int i_out = 0; i_out < N && env->initial_temperature; i_out++) {

            vector<int> bests;
            for (int i_in = 0; i_in < N; i_in++) {
                if (estimate[i_in] != -1) continue;
                bests.push_back(i_in);
            }
            sort(bests.begin(), bests.end(), [&](int a, int b) {
                return compatibility_matrix[a][i_out] < compatibility_matrix[b][i_out];
            });
            for (auto i_in : bests) {
                Position p = env->min_move_to(env->wormholes[i_out], {env->L/2, env->L/2});
                int measure = compatibility_matrix.measure(i_in, p.x, p.y, env->remeasurements);
                if (measure >= env->max_temperature*env->elimination_threshold) {
                    estimate[i_in] = i_out;
                    break;
                }
            }
        }
        // compatibility_matrix.print();
        return compatibility_matrix.assignment();
    }
};

int main() {
    int L, N, S;
    cin >> L >> N >> S;
    vector<Position> wormholes(N);
    for (int i = 0; i < N; i++) {
        cin >> wormholes[i].x >> wormholes[i].y;
    }
    Environment environment(L, N, S, wormholes);
    env = &environment;
    Solver solver;
    solver.solve();

}
