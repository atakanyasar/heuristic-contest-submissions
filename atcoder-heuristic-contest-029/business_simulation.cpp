#include <cassert>
#include <cmath>
#include <iostream>
#include <sstream>
#include <vector>
#include <cstdint>
#include <algorithm>

using namespace std;

constexpr int MAX_INVEST_LEVEL = 20;

struct Project {
    int64_t h;
    int64_t v;

    double expected_value() const {
        return (double)v / (double)h;
    }

};

enum class CardType {
    WORK_SINGLE = 0,
    WORK_ALL = 1,
    CANCEL_SINGLE = 2,
    CANCEL_ALL = 3,
    INVEST = 4,
};

ostream& operator<<(ostream& os, CardType ct) {
    switch(ct) {
        case CardType::WORK_SINGLE:
            os << "WORK_SINGLE";
            break;
        case CardType::WORK_ALL:
            os << "WORK_ALL";
            break;
        case CardType::CANCEL_SINGLE:
            os << "CANCEL_SINGLE";
            break;
        case CardType::CANCEL_ALL:
            os << "CANCEL_ALL";
            break;
        case CardType::INVEST:
            os << "INVEST";
            break;
    }
    return os;
}

struct Card {
    CardType t;
    int64_t w;
    int64_t p;
};

struct Judge {
    const int n;
    const int m;
    const int k;

    Judge(int n, int m, int k): n(n), m(m), k(k) {}

    vector<Card> read_initial_cards() {
        vector<Card> cards;
        for (int i = 0; i < n; i++) {
            int64_t t, w;
            cin >> t >> w;
            cards.push_back(Card{(CardType)t, w, 0ll});
        }
        return cards;
    }

    vector<Project> read_projects() {
        vector<Project> projects;
        for (int i = 0; i < m; i++) {
            int64_t h, v;
            cin >> h >> v;
            projects.push_back(Project{h, v});
        }
        return projects;
    }

    void use_card(int c, int m) {
        cout << c << " " << m << endl;
    }

    int64_t read_money() {
        int64_t money;
        cin >> money;
        return money;
    }

    vector<Card> read_next_cards() {
        vector<Card> cards;
        for (int i = 0; i < k; i++) {
            int64_t t, w, p;
            cin >> t >> w >> p;
            cards.push_back(Card{(CardType)t, w, p});
        }
        return cards;
    }

    void select_card(int r) {
        cout << r << endl;
    }

    void comment(const string& message) {
        cout << "# " << message << endl;
    }
};

struct State {
    int64_t money;
    vector<Card> cards;
    vector<Project> projects;
    vector<Card> next_cards;
    int invest_level;
    int turn;

    State() : money(0), invest_level(0), turn(0) {}

    double target_efficiency = 1.2;

    vector<int> get_efficient_projects() {
        vector<int> efficient_projects(projects.size());
        for (int i = 0; i < projects.size(); ++i) {
            efficient_projects[i] = i;
        }
        sort(efficient_projects.begin(), efficient_projects.end(), [&](int a, int b) {
            return projects[a].v * projects[b].h > projects[b].v * projects[a].h;
        });
        return efficient_projects;
    }

    double work_single_efficiency(Card card, Project project) {
        return min((double)project.v / (double)project.h * (double)card.w, (double)project.v) - (double)card.p;
    }

    double work_all_efficiency(Card card) {
        double efficiency = 0;
        for (auto project : projects) {
            efficiency += work_single_efficiency(card, project);
        }
        return efficiency;
    }

    pair<int, int> select_action() {

        vector<int> efficient_projects = get_efficient_projects();

        for (int i = 0; i < cards.size(); ++i) {
            if (cards[i].t == CardType::INVEST) {
                return {i, 0};
            }
        }

        for (int i = 0; i < cards.size(); ++i) {
            if (cards[i].t == CardType::WORK_ALL) {
                return {i, 0};
            }
        }

        if (projects[efficient_projects[0]].expected_value() < target_efficiency) {
            for (int i = 0; i < cards.size(); ++i) {
                if (cards[i].t == CardType::CANCEL_ALL) {
                    return {i, 0};
                }
            }
        }
        if (projects[efficient_projects.back()].expected_value() < target_efficiency) {
            for (int i = 0; i < cards.size(); ++i) {
                if (cards[i].t == CardType::CANCEL_SINGLE) {
                    return {i, efficient_projects.back()};
                }
            }
        }

        pair<int, int> best = {-1, -1};
        for (int i = 0; i < cards.size(); ++i) {
            if (cards[i].t == CardType::WORK_SINGLE) {
                if (best.first == -1 || (cards[i].w > cards[best.first].w && projects[efficient_projects[0]].h * 7 >= cards[i].w * 5)) {
                    best = {i, 0};
                }
            }
        }
        if (best.first != -1) {
            best.second = efficient_projects[0];
            return best;
        }
        return {0, 0};
    }

    int select_next_card() {
        vector<int> efficient_projects = get_efficient_projects();

        for (int i = 0; i < next_cards.size() && invest_level < MAX_INVEST_LEVEL; ++i) {
            if (next_cards[i].t == CardType::INVEST && money >= next_cards[i].p * 2 && turn > 100) {
                return i;
            }
        }
        for (int i = 0; i < next_cards.size(); ++i) {
            if (next_cards[i].t == CardType::WORK_ALL && money >= next_cards[i].p * 2 && turn > 200 &&
                work_all_efficiency(next_cards[i]) > 0) {
                return i;
            }
        }
        for (int i = 0; i < next_cards.size(); ++i) {
            if (projects[efficient_projects[0]].expected_value() < target_efficiency) {
                if (next_cards[i].t == CardType::CANCEL_ALL && money >= next_cards[i].p * 100 && turn > 400) {
                    return i;
                }
            }
        }

        if (projects[efficient_projects.back()].expected_value() < target_efficiency) {
            for (int i = 0; i < next_cards.size(); ++i) {
                if (next_cards[i].t == CardType::CANCEL_SINGLE && money >= next_cards[i].p * 200 && turn > 50) {
                    return i;
                }
            }
        }

        vector<int> efficient_next_cards;
        for (int i = 0; i < next_cards.size(); ++i) {
            if (next_cards[i].t == CardType::WORK_SINGLE && money >= next_cards[i].p) {
                if ((double)next_cards[i].w / (double)next_cards[i].p > 1.3)
                    efficient_next_cards.push_back(i);
            }
        }
        sort(efficient_next_cards.begin(), efficient_next_cards.end(), [&](int a, int b) {
            return next_cards[a].w > next_cards[b].w;
        });

        if (!efficient_next_cards.empty()) {
            return efficient_next_cards[0];
        }

        return 0;
    }
};

struct Solver {
    const int n, m, k, t;
    Judge judge;

    Solver(int n, int m, int k, int t) : 
        n(n), m(m), k(k), t(t), judge(n, m, k) {
    }

    int64_t solve() {
        State state;
        state.cards = judge.read_initial_cards();
        state.projects = judge.read_projects();
        state.turn = t;
        while (state.turn--) {
            auto [use_card_i, use_target] = state.select_action();
            const Card& use_card = state.cards[use_card_i];
            if (use_card.t == CardType::INVEST) {
                state.invest_level++;
            }
            // example for comments
            stringstream msg;
            msg << "used Card(t=" << use_card.t << " w=" << use_card.w << " p=" << use_card.p << ") to target " << use_target;
            judge.comment(msg.str());
            judge.use_card(use_card_i, use_target);
            assert(state.invest_level <= MAX_INVEST_LEVEL);

            state.projects = judge.read_projects();
            state.money = judge.read_money();

            state.next_cards = judge.read_next_cards();
            int select_card_i = state.select_next_card();
            state.cards[use_card_i] = state.next_cards[select_card_i];
            judge.select_card(select_card_i);
            state.money -= state.next_cards[select_card_i].p;
            assert(state.money >= 0);
        }
        return state.money;
    }

};

int main() {
    int n, m, k, t;
    cin >> n >> m >> k >> t;
    Solver solver(n, m, k, t);
    int64_t score = solver.solve();
    cerr << "score:" << score << endl;
}
