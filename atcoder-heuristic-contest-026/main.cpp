#include<bits/stdc++.h>
using namespace std;

const int N = 200;
const int M = 10;

vector<vector<int>> stacks;
vector<pair<int, int>> box_loc;
int next_smallest_box = 1;
int energy = 0;

vector<int>& get_stack_of_box(int box) {
    return stacks[box_loc[box].first];
}

vector<int>::iterator get_box_iterator(int box) {
    return get_stack_of_box(box).begin() + box_loc[box].second;
}

vector<int> get_sorted_indexes(function<bool(int,int)> f) {
    vector<int> s;
    for (int i = 1; i <= M; i++) s.push_back(i);
    sort(s.begin(), s.end(), f);
    return s;
}

vector<int> sort_by_back() {
    return get_sorted_indexes([](int a, int b) {
        if (stacks[a].empty()) return false;
        if (stacks[b].empty()) return true;
        return stacks[a].back() > stacks[b].back();
    });
}

vector<int> sort_by_size() {
    return get_sorted_indexes([](int a, int b) {
        return stacks[a].size() < stacks[b].size();
    });
}

void operation(int box, int move_to) {
    if (box > N) {
        exit(0);
    }
    if (move_to < 0 || move_to > M) {
        assert(false);
    }
    if (move_to == 0) {
        if (box_loc[box].second + 1 != get_stack_of_box(box).size()) {
            return;
        }
        get_stack_of_box(box).pop_back();
        box_loc[box] = make_pair(0, 0);
        next_smallest_box++;
    }
    else if (move_to != box_loc[box].first) {
        energy += get_stack_of_box(box).end() - get_box_iterator(box) + 1;
        stacks[move_to].insert(stacks[move_to].end(), get_box_iterator(box), get_stack_of_box(box).end());
        get_stack_of_box(box).erase(get_box_iterator(box), get_stack_of_box(box).end());
        for (int i = 0; i < stacks[move_to].size(); i++) {
            box_loc[stacks[move_to][i]] = make_pair(move_to, i);
        }
    }
    else {
        energy += get_stack_of_box(box).end() - get_box_iterator(box) + 1;
    }
    cout << box << " " << move_to << endl;

    if (next_smallest_box <= N && box_loc[next_smallest_box].second + 1 == get_stack_of_box(next_smallest_box).size()) {
        operation(next_smallest_box, 0);
    }
}

int select_stack(int box) {
    vector<int> s = sort_by_size();
    if (box_loc[box].first == s[0]) {
        return s[1];
    }
    else {
        return s[0];
    }
}

void remove_smallest_box() {
    auto to_remove = next(get_box_iterator(next_smallest_box));
    if (to_remove != get_stack_of_box(next_smallest_box).end()) {
        operation(*to_remove, select_stack(*to_remove));
    }
    operation(next_smallest_box, 0);
}

void swap(int stack) {
    if (stacks[stack].size() < 2) return;
    int next = stack % M + 1;
    int x = stacks[stack].back();
    int y = *(stacks[stack].end() - 2);
    if (x > y) return;
    operation(x, next);
    operation(y, next);
    operation(x, stack);
}

void collect(int q) {
    while(true) {
        int max_box = 0;
        for (int j = 1; j <= M; j++) {
            if (stacks[j].empty() || j == q) continue;
            if (stacks[q].empty() || stacks[q].back() > stacks[j].back()) {
                max_box = max(max_box, stacks[j].back());
            }
        }
        if (max_box == 0) break;
        operation(max_box, q);
    }
}

void spread(int stack) {
    while (!stacks[stack].empty()) {
        int j = 0;
        for (int k = 1; k <= M; k++) {
            if (k == stack) continue;
            if (stacks[k].empty() || stacks[k].back() < stacks[stack].back()) {
                if (j == 0 || (stacks[k].empty() ? 0 : stacks[k].back()) > (stacks[j].empty() ? 0 : stacks[j].back())) j = k;
            }
        }
        if (j == 0) {
            j = stack % M + 1;
//            for (int k = 0; k < M; k++) {
//                if (k == stack) continue;
//                if (j == 0 || (stacks[k].empty() ? 0 : stacks[k].back()) < (stacks[j].empty() ? 0 : stacks[j].back())) j = k;
//            }
        }
        operation(stacks[stack].back(), j);
//        swap(j);
    }
}


int main(){

    freopen("tools/in/0000.txt", "r", stdin);
    freopen("tools/out/0000.txt", "w", stdout);

    int n, m;
    cin >> n >> m;
    assert (n == N && m == M);

    stacks = vector<vector<int>>(m + 1, vector<int>(n/m, 0));
    box_loc = vector<pair<int, int>>(n + 1, make_pair(0, 0));

    for (int i = 1; i <= m; i++) {
        for (int j = 0; j < n/m; j++) {
            cin >> stacks[i][j];
            box_loc[stacks[i][j]] = make_pair(i, j);
        }
    }

    for (int i = 1; i <= M && next_smallest_box <= N; i++) {
        int j = box_loc[next_smallest_box].first;
        spread(j);
        collect(j);
    }

    while (next_smallest_box <= N) {
        remove_smallest_box();
    }
    cerr << "Energy: " << energy << endl;


}