#ifndef GAME_H
#define GAME_H

#include <algorithm>
#include <vector>
#include <numeric>
#include <map>
#include <string>

#include "deck.h"
#include "infosetnode.h"
#include "shared.h"
#include <nlohmann/json.hpp>

struct Game{
    InfoSetNode* root0;
    InfoSetNode* root1;
    Deck* deck;
    int iter;
    int counter = 0;

    Game();
    ~Game();

    std::pair<double, double> cfr(
        InfoSetNode* r0, 
        InfoSetNode* r1, 
        std::vector<Action> history,
        std::vector<double> remaining_stacks, 
        std::vector<double> bet_sizes,
        double pot_size, 
        int round_num, 
        int player,
        long double p0, 
        long double p1);

    inline bool is_non_terminal_round_end(std::vector<Action> history);
    inline bool is_terminal(std::vector<Action> history, int round_num);
    std::tuple<std::vector<double>, std::vector<double>, double> get_next_stacks_bets_pot(Action action, std::vector<double> remaining_stacks, std::vector<double> bet_sizes, double pot_size, int player);
    std::vector<Action> get_actions(std::vector<Action> history, std::vector<double> remaining_stacks, std::vector<double> bet_sizes, double pot_size, int player);
    std::pair<double, double> get_utility(std::vector<double> remaining_stacks, std::vector<double> bet_sizes, double pot_size);
    inline int get_bucket(int round_num, int player);
    int get_init_bucket(int player);
    void train(int epochs);
    nlohmann::json save(InfoSetNode* r0, InfoSetNode* r1, std::string history, int player);
};


#endif