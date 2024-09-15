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
        std::vector<std::string> history,
        std::vector<double> remaining_stacks, 
        std::vector<double> bet_sizes,
        double pot_size, 
        int round_num, 
        double p0, 
        double p1);

    bool is_non_terminal_round_end(std::vector<std::string> history);
    bool is_terminal(std::vector<std::string> history, int round_num);
    int get_player(std::vector<std::string> history);
    std::tuple<std::vector<double>, std::vector<double>, double> get_next_stacks_bets_pot(std::string action, std::vector<double> remaining_stacks, std::vector<double> bet_sizes, double pot_size, int player);
    std::vector<std::string> get_actions(std::vector<std::string> history, std::vector<double> remaining_stacks, std::vector<double> bet_sizes, double pot_size, int player);
    std::pair<double, double> get_utility(std::vector<double> remaining_stacks, std::vector<double> bet_sizes, double pot_size);
    int get_bucket(int round_num, int player);
    void train(int epochs);
};


#endif