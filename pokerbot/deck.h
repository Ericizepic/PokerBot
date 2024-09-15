#ifndef DECK_H
#define DECK_H

#include <algorithm>
#include <random>
#include <vector>

struct Deck
{
    std::vector<int> cards;
    std::vector<std::vector<int>> playerCards;

    Deck();
    std::vector<int> getPlayerCards(int player);
    std::vector<int> getCommunityCards(int round_num);
};

#endif
