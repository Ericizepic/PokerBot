#include "deck.h"

Deck::Deck() {
    cards = defaultDeckCards;
    auto rng = std::default_random_engine {};
    std::shuffle(cards.begin(), cards.end(), rng);

    playerCards = std::vector<std::vector<int>>(2, std::vector<int>(0));
    playerCards[0].push_back(cards[0]);
    playerCards[0].push_back(cards[1]);
    playerCards[1].push_back(cards[2]);
    playerCards[1].push_back(cards[3]);
}

std::vector<int> Deck::getPlayerCards(int player){
    return playerCards[player];
}


std::vector<int> Deck::getCommunityCards(int round_num){
    std::vector<int> communityCards;
    for (int i = 0; i<round_num; i++)
        communityCards.push_back(cards[4+i]);

    return communityCards;
}
