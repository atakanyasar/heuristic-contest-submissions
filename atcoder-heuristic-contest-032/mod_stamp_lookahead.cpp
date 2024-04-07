#include <iostream>
#include <vector>
#include <array>
#include <cstdint>
#include <chrono>
//#define DEBUG

using namespace std;

class System {

public:
    int grid_size{};
    int num_stamps{};
    int num_operations{};
    int mod = 998244353;
    chrono::time_point<chrono::system_clock> time_start;
    vector<vector<int>> grid;
    vector<vector<vector<int>>> stamps;
    vector<array<int, 3>> operations;

    System() {
        time_start = chrono::high_resolution_clock::now();
    }

    void assert_time() {
        auto time_now = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::milliseconds>(time_now - time_start).count();
        if (duration > 1500) {
#ifdef DEBUG
            cerr << "Time limit exceeded" << endl;
#endif
            cout << *this;
            exit(0);
        }
    }

    int64_t apply_operation(int stamp_index, int x, int y) {
        if (operations.size() == num_operations) {
            return 0;
        }
        int64_t delta = 0;
        operations.push_back({stamp_index, x, y});
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                delta -= grid[x + i][y + j];
                grid[x + i][y + j] += stamps[stamp_index][i][j];
                grid[x + i][y + j] %= mod;
                delta += grid[x + i][y + j];
            }
        }
        return delta;
    }

    int64_t undo_operation(int stamp_index, int x, int y) {
        int64_t delta = 0;

        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                delta -= grid[x + i][y + j];
                grid[x + i][y + j] -= stamps[stamp_index][i][j];
                grid[x + i][y + j] += mod;
                grid[x + i][y + j] %= mod;
                delta += grid[x + i][y + j];
            }
        }

        return delta;
    }

    int64_t undo_last_operation() {
        auto last_operation = operations.back();
        operations.pop_back();
        return undo_operation(last_operation[0], last_operation[1], last_operation[2]);
    }

    friend istream& operator>>(istream& is, System& system) {
        is >> system.grid_size >> system.num_stamps >> system.num_operations;
        system.grid = vector<vector<int>> (system.grid_size, vector<int>(system.grid_size));
        system.stamps = vector<vector<vector<int>>> (system.num_stamps, vector<vector<int>>(3, vector<int>(3)));

        for (int i = 0; i < system.grid_size; i++) {
            for (int j = 0; j < system.grid_size; j++) {
                is >> system.grid[i][j];
            }
        }

        for (int stamp_id = 0; stamp_id < system.num_stamps; stamp_id++) {
            for (int j = 0; j < 3; j++) {
                for (int k = 0; k < 3; k++) {
                    is >> system.stamps[stamp_id][j][k];
                }
            }
        }

        return is;
    }

    friend ostream& operator<<(ostream& os, const System& system) {
        os << system.operations.size() << endl;
        for (auto operation : system.operations) {
            os << operation[0] << " " << operation[1] << " " << operation[2] << endl;
        }
        return os;
    }

};

class StampPlacement {
    System& system;

    array<int64_t, 4> lookahead(int operations) {
        if (operations == 0) {
            return {0, -1, -1, -1};
        }
        array<int64_t, 4> best = {INT32_MIN, -1, -1, -1};

        for (int stamp_index = 0; stamp_index < system.num_stamps; stamp_index++) {
            for (int x = 0; x < system.grid_size - 2; x++) {
                for (int y = 0; y < system.grid_size - 2; y++) {
                    int64_t score = system.apply_operation(stamp_index, x, y);
                    auto next = lookahead(operations - 1);
                    score += next[0];
                    if (score > best[0]) {
                        best = {score, stamp_index, x, y};
                    }
                    system.undo_last_operation();
                }
            }
        }

        return best;
    }

public:
    explicit StampPlacement(System& system) : system(system) {}

    void place_stamps() {
        for (int i = 0; i < system.num_operations; i++) {
            int look_ahead = 1;
            for (look_ahead = 1; look_ahead <= 2; look_ahead++) {
                auto best = lookahead(look_ahead);
                if (best[1] < 0) {
                    break;
                }
                int64_t delta = system.apply_operation(best[1], best[2], best[3]);
#ifdef DEBUG
                cerr << "Operation #" << i << " score: " << delta << endl;
#endif
                system.assert_time();
                if (best[0] > 0) break;
            }
            if (look_ahead + 1 == 3) {
                break;
            }
        }

    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    cout.tie(nullptr);
    cerr.tie(nullptr);

#ifdef DEBUG
    freopen("tools/in/0001.txt", "r", stdin);
    freopen("tools/out.txt", "w", stdout);
#endif
    System system;
    cin >> system;
    StampPlacement placement(system);
    placement.place_stamps();
    cout << system;
    return 0;
}
