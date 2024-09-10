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

#define NUM_BUCKETS 10000
#define STARTING_STACK 100

const std::vector<std::string> ALL_ACTIONS = {"f", "c", "r50", "r100", "a"};

struct Game{
    InfoSetNode* root0;
    InfoSetNode* root1;
    Deck* deck;
    int iter;

    Game();
    ~Game();

    std::pair<double, double> cfr(InfoSetNode* r0, InfoSetNode* r1, std::vector<std::string> history, int round_num, double p0, double p1);
    bool is_non_terminal_round_end(std::vector<std::string> history);
    bool is_terminal(std::vector<std::string> history, int round_num);
    int get_player(std::vector<std::string> history);
    std::pair<std::vector<double>, std::vector<std::string>> get_bet_sizes_and_actions(std::vector<std::string> history);
    std::vector<std::string> get_actions(std::vector<std::string> history);
    std::pair<double, double> get_utility(std::vector<std::string> history);
    int get_bucket(int round_num, int player);
    void train(int epochs);
};


#endif