#ifndef SHARED_H
#define SHARED_H

#include <phevaluator/phevaluator.h>
#include <algorithm>
#include <bitset>
#include <iostream>
#include <vector>

const std::vector<int> defaultDeckCards = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51};

int get_rank(const int holeCard1, const int holeCard2, const std::vector<int> &communityCards);

double eval_runout(const int ourcard1, const int ourcard2, const std::vector<int> &finalCommunityCards, const std::vector<int> &unseenbyriver);

double ehs2(const std::vector<int> &ourcards, const std::vector<int> &seenCommunityCards);
    
int get_winning_player(const std::vector<int> &player0Cards, const std::vector<int> &player1Cards, const std::vector<int> &communityCards);

std::pair<double, double> operator +(const std::pair<double, double>& x, const std::pair<double, double>& y) {
    return std::make_pair(x.first + y.first, x.second + y.second);
}

std::pair<double, double> operator -(const std::pair<double, double>& x, const std::pair<double, double>& y) {
    return std::make_pair(x.first - y.first, x.second - y.second);
}

#endif
