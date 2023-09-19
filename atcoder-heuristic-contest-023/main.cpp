#include <iostream>
#include <utility>
#include <vector>
#include <queue>
#include <algorithm>
#include <ctime>
#include <set>
#include <map>

using namespace std;

struct Farm;
struct Graph;
struct BfsTree;
struct Plant;
struct Solution;
struct Environment *env;

int start_time = clock();

bool time_termination() {
    return (clock() - start_time > 1.3 * CLOCKS_PER_SEC);
}

struct Farm {
    int T{}, K{}, H{}, W{}, i0{};
    vector<int> S, D;
    vector<vector<bool>> h, v;

    void read_input() {
        cin >> T >> H >> W >> i0;
        h.resize(H - 1, vector<bool>(W, false));
        for (int i = 0; i < H - 1; ++i) {
            string s; cin >> s;
            for (int j = 0; j < W; ++j) if (s[j] == '1') h[i][j] = true;
        }
        v.resize(H, vector<bool>(W - 1, false));
        for (int i = 0; i < H; ++i) {
            string s; cin >> s;
            for (int j = 0; j < W - 1; ++j) if (s[j] == '1') v[i][j] = true;
        }
        cin >> K;
        S.resize(K);
        D.resize(K);
        for (int i = 0; i < K; ++i) cin >> S[i] >> D[i];
    }

    vector<vector<int>> construct_graph() {
        vector<vector<int>> adj(H * W);
        for (int i = 0; i < H; ++i) {
            for (int j = 0; j < W; ++j) {
                if (i + 1 < H && !h[i][j]) {
                    // no waterway between (i, j) and (i + 1, j)
                    adj[i*W + j].emplace_back((i + 1)*W + j);
                    adj[(i + 1)*W + j].emplace_back(i*W + j);
                }
                if (j + 1 < W && !v[i][j]) {
                    // no waterway between (i, j) and (i, j + 1)
                    adj[i*W + j].emplace_back(i*W + j + 1);
                    adj[i*W + j + 1].emplace_back(i*W + j);
                }
            }
        }
        return adj;
    }

};

struct Plant {
    int id, start, end, location, time;

    int get_potential_score() const {
        return (end - start + 1);
    }
};

struct Graph : vector<vector<int>> {

    int root = 0;
    vector<int> degree;
    vector<bool> marked;

    Graph() = default;
    explicit Graph(vector<vector<int>> adj, int root) : vector<vector<int>>(std::move(adj)), root(root) {
        init_graph_properties();
    }

    void init_graph_properties() {
        marked = vector<bool>(size(), false);
        degree = vector<int>(size(), 0);
        for (int i = 0; i < size(); ++i) {
            degree[i] = (int)(*this)[i].size();
        }
    }

    void mark(int node) {
        marked[node] = true;
        for (auto neighbor : (*this)[node]) {
            degree[neighbor]--;
        }
    }

    void unmark(int node) {
        marked[node] = false;
        for (auto neighbor : (*this)[node]) {
            degree[neighbor]++;
        }
    }

};

struct BfsTree : Graph {

    vector<Plant*> plants;

    BfsTree() = default;

    explicit BfsTree(Graph& graph) : Graph() {
        from_graph(graph);
        init_graph_properties();
        init_tree_properties();
    }

    void from_graph(Graph& graph) {
        resize(graph.size());
        root = graph.root;

        queue<int> bfs;
        bfs.push(graph.root);
        vector<bool> visited(graph.size(), false);
        visited[graph.root] = true;
        while (!bfs.empty()) {
            int u = bfs.front(); bfs.pop();
            for (auto v : graph[u]) {
                if (!visited[v]) {
                    (*this)[u].emplace_back(v);
                    bfs.push(v);
                    visited[v] = true;
                }
            }
        }
    }

    void init_tree_properties() {
        plants = vector<Plant*>(size(), nullptr);
    }

    vector<int> get_leaves() {
        vector<int> leaves;
        for (int i = 0; i < size(); ++i) {
            if (plants[i] != nullptr) continue;
            int cnt = 0;
            for (auto neighbor : (*this)[i]) {
                if (plants[neighbor] == nullptr) {
                    cnt++;
                    break;
                }
            }
            if (cnt == 0) leaves.emplace_back(i);
        }
        return leaves;
    }

    void allocate(Plant& plant, int location) {
        plants[location] = &plant;
        plant.location = location;
        plant.time = plant.start;
        maintain_heap_property(root);
    }

    void deallocate(int location) {
        plants[location] = nullptr;
    }

    void deallocate_all() {
        for (int i = 0; i < size(); ++i) {
            plants[i] = nullptr;
        }
    }

    void maintain_heap_property(int node_u, int time = INT32_MAX) {
        if (plants[node_u] != nullptr) {
            time = plants[node_u]->time = min(plants[node_u]->start, time);
        }
        for (auto node_v : (*this)[node_u]) {
            maintain_heap_property(node_v, time);
        }
    }

    int get_insertion_cost(int node_u, int time) {
        int cost = 0;
        if (plants[node_u] != nullptr) {
            cost = max(0, plants[node_u]->time - time);
        }
        for (auto node_v : (*this)[node_u]) {
            cost += get_insertion_cost(node_v, time);
        }
        return cost;
    }

    int get_insertion_cost(Plant& plant, int location) {
        return get_insertion_cost(location, plant.start);
    }

    void maintain(int time) {
        for (int i = 0; i < size(); ++i) {
            if (plants[i] != nullptr && plants[i]->time >= time) {
                deallocate(i);
            }
        }
    }

};

struct Environment {
    int height, width, time;
    int num_plants;
    int root;
    Graph graph;
    BfsTree tree;
    vector<Plant> plants;

    explicit Environment(Farm farm): height(farm.H), width(farm.W), time(farm.T), num_plants(farm.K) {
        root = farm.i0 * width;
        graph = Graph(farm.construct_graph(), root);
        tree = BfsTree(graph);

        for (int i = 0; i < num_plants; ++i) {
            plants.emplace_back(Plant{i+1, farm.S[i], farm.D[i], -1, -1});
        }

        sort(plants.begin(), plants.end(), [](Plant& a, Plant& b) {
            return a.end > b.end || (a.end == b.end && a.start < b.start);
        });
    }

};

struct Solution {
    int score = 0;
    set<int> plant_indices;
    static Solution best_solution;
    static vector<Plant> plants;

    Solution() {
        if (env == nullptr) return;
        *this = best_solution;
    }

    explicit Solution(set<int> indices) : plant_indices(indices) {
        if (env == nullptr) return;
        for (auto index : plant_indices) {
            score += env->plants[index].get_potential_score();
        }
    }

    vector<int> get_free_plants() {
        vector<int> free_plants;
        for (int i = 0; i < env->plants.size(); ++i) {
            if (!plant_indices.count(i)) free_plants.emplace_back(i);
        }
        return free_plants;
    }

    void include(int index) {
        plant_indices.insert(index);
        score += env->plants[index].get_potential_score();
    }

    void exclude(int index) {
        plant_indices.erase(index);
        score -= env->plants[index].get_potential_score();
    }

    void update() const {
        best_solution = *this;
        plants = env->plants;
    }

    static void print_plan() {
        cout << best_solution.plant_indices.size() << endl;
        for (auto x : best_solution.plant_indices) {
            cout << plants[x].id << " " << plants[x].location / env->width << " " << plants[x].location % env->width << " " << plants[x].time << endl;
        }
    }

    double get_utility() const {
        return score / (double) (env->time * env->height * env->width);
    }
};


struct PlacementManager {

    static int scaling;

    static vector<int> get_costs_from_leaves(BfsTree& tree, Plant& plant) {
        vector<int> leaves = tree.get_leaves();
        vector<int> costs(leaves.size(), 0);
        for (int i = 0; i < leaves.size(); ++i) {
            costs[i] = tree.get_insertion_cost(plant, leaves[i]) / scaling * scaling;
        }
        return costs;
    }

    static int select_place(BfsTree& tree, Plant& plant) {
        vector<int> leaves = tree.get_leaves();
        if (leaves.empty()) return -1;
        vector<int> costs = get_costs_from_leaves(tree, plant);
        int min_cost_index = min_element(costs.begin(), costs.end()) - costs.begin();
        if (costs[min_cost_index] * 10 > plant.get_potential_score()) return -1;
        return leaves[min_cost_index];
    }

    static int force_place(BfsTree& tree, Plant& plant) {
        vector<int> leaves = tree.get_leaves();
        if (leaves.empty()) return -1;
        vector<int> costs = get_costs_from_leaves(tree, plant);
        int min_cost_index = min_element(costs.begin(), costs.end()) - costs.begin();
        return leaves[min_cost_index];
    }

    static bool simulate(Solution& solution) {
        BfsTree& tree = env->tree;
        tree.deallocate_all();

        for (auto index : solution.plant_indices) {
            Plant& plant = env->plants[index];
            tree.maintain(plant.end + 1);
            int location = force_place(tree, plant);
            if (location == -1) {
                return false;
            }
            tree.allocate(plant, location);
        }
        tree.maintain(1);
        return true;
    }

};

struct PlantingPlanner {

    explicit PlantingPlanner() = default;

    static Solution initial_plan(int scaling) {
        PlacementManager::scaling = scaling;

        Solution solution((set<int>()));
        vector<Plant>& plants = env->plants;
        BfsTree& tree = env->tree;
        tree.deallocate_all();

        for (int index = 0; index < plants.size(); ++index) {
            Plant& plant = plants[index];
            tree.maintain(plant.end + 1);
            int location = PlacementManager::select_place(tree, plant);
            if (location != -1) {
                tree.allocate(plant, location);
                solution.include(index);
            }
        }
        tree.maintain(1);

        if (solution.score > Solution::best_solution.score) {
            solution.update();
        }
        return solution;
    }

    static Solution post_process(Solution solution) {
        map<int, set<pair<int, int>>> free_plants;
        for (auto index : solution.get_free_plants()) {
            Plant& plant = env->plants[index];
            free_plants[plant.end].insert({plant.start, index});
        }

        vector<map<int, int>> location_history(env->tree.size());
        for (auto index : solution.plant_indices) {
            Plant& plant = env->plants[index];
            location_history[plant.location].insert({plant.end, index});
        }

        for (auto index : solution.plant_indices) {
            Plant& plant = env->plants[index];
            int min_end = env->time;
            auto itr = location_history[plant.location].upper_bound(plant.end);
            if (itr != location_history[plant.location].end()) min_end = env->plants[itr->second].time - 1;
            for (auto neighbor : env->tree[plant.location]) {
                min_end = min(min_end, location_history[neighbor].lower_bound(plant.end)->first);
            }
            int min_start_index = index;
            for (int i = plant.end; i <= min_end; i++) {
                auto it = free_plants[i].lower_bound({plant.time, 0});
                if (it != free_plants[i].end() && it->first < env->plants[min_start_index].start) {
                    min_start_index = it->second;
                }
            }
            if (min_start_index != index) {
                solution.exclude(index);
                solution.include(min_start_index);
                env->plants[min_start_index].time = plant.time;
                env->plants[min_start_index].location = plant.location;
                free_plants[plant.end].insert({plant.start, index});
                free_plants[plant.end].erase({env->plants[min_start_index].start, min_start_index});
                location_history[plant.location].erase(plant.end);
                location_history[plant.location].insert({env->plants[min_start_index].end, min_start_index});
            }
            if (time_termination()) {
                break;
            }
        }
        if (solution.score > Solution::best_solution.score) {
            solution.update();
        }
        return solution;
    }

    static Solution heuristic(Solution& solution) {
        vector<int> free_plants = solution.get_free_plants();
        sort(free_plants.begin(), free_plants.end(), [](int a, int b) {
            return env->plants[a].get_potential_score() < env->plants[b].get_potential_score();
        });
        for(auto plant_index : free_plants) {
            if (env->plants[plant_index].get_potential_score() > 3) continue;
            Solution new_solution = solution;
            new_solution.include(plant_index);
            if (PlacementManager::simulate(new_solution)) {
                if (new_solution.score > Solution::best_solution.score) {
                    new_solution.update();
                } else{
                    new_solution.exclude(plant_index);
                }
            } else {
                new_solution.exclude(plant_index);
            }
        }
        return Solution::best_solution;
    }

    static void stats() {
        cerr << "Score: " << Solution::best_solution.score << endl;
        map<pair<int, int>, int> count;

        for (Plant& plant : env->plants) {
            count[{plant.start, plant.end}]++;
        }

        for(int i = 1; i <= env->time; i++) {
            for (int j = 1; j <= env->time; j++) {
                cout << count[{i, j}] << " ";
            }
            cout << endl;
        }


    }

};

Solution Solution::best_solution = Solution();
vector<Plant> Solution::plants = vector<Plant>();
int PlacementManager::scaling = 1;

int main(int argc, char** argv) {


    if (argc > 1 && argv[1][0] == '<') freopen(argv[2], "r", stdin);
    if (argc > 3 && argv[3][0] == '>') freopen(argv[4], "w", stdout);

    Farm farm;
    farm.read_input();
    env = new Environment(farm);

    for (int i = 1; !time_termination(); i += 1) {
        Solution solution = PlantingPlanner::post_process(PlantingPlanner::initial_plan(i));
        // cerr << "#" << i << " (" << Solution::best_solution.get_utility() << ") <= " << solution.get_utility() << endl;
    }
//    Solution::best_solution = PlantingPlanner::heuristic(Solution::best_solution);
    Solution::print_plan();
//    PlantingPlanner::stats();

    return 0;
}