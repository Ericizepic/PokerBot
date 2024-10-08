#ifndef SHARED_H
#define SHARED_H

#include <phevaluator/phevaluator.h>
#include <algorithm>
#include <bitset>
#include <exception>
#include <iostream>
#include <vector>

#define NUM_BUCKETS 5
#define STARTING_STACK 30
const std::vector<std::string> ALL_ACTIONS = {"f", "c", "r50", "r100", "a"};

const std::vector<std::string> STARTING_HAND_RANKINGS = {
    "AA",
    "KK",
    "QQ",
    "AKs",
    "JJ",
    "AQs",
    "KQs",
    "AJs",
    "KJs",
    "TT",
    "AKo",
    "ATs",
    "QJs",
    "KTs",
    "QTs",
    "JTs",
    "99",
    "AQo",
    "A9s",
    "KQo",
    "88",
    "K9s",
    "T9s",
    "A8s",
    "Q9s",
    "J9s",
    "AJo",
    "A5s",
    "77",
    "A7s",
    "KJo",
    "A4s",
    "A6s",
    "QJo",
    "66",
    "K8o",
    "T8s",
    "A2s",
    "98s",
    "J8s",
    "ATo",
    "Q8s",
    "K7s",
    "KTo",
    "55",
    "JTo",
    "78s",
    "QTo",
    "44",
    "22",
    "33",
    "K6s",
    "97s",
    "K5s",
    "76s",
    "T7s",
    "K4s",
    "K3s",
    "K2s",
    "Q7s",
    "86s",
    "65s",
    "J7s",
    "54s",
    "Q6s",
    "75s",
    "96s",
    "Q5s",
    "64s",
    "Q3s",
    "T9o",
    "T6s",
    "Q2s",
    "A9o",
    "53s",
    "85s",
    "J6s",
    "J9o",
    "K9o",
    "J5s",
    "Q9s",
    "43s",
    "74s",
    "J4s",
    "J3s",
    "95s",
    "J2s",
    "63s",
    "A8o", 
    "52s",
    "T5s",
    "84s",
    "T4s",
    "T3s",
    "42s",
    "T2s",
    "98o",
    "T8o",
    "A5o",
    "A7o",
    "73s",
    "A4o",
    "32s",
    "94s",
    "93s",
    "J8o",
    "A3o",
    "62o",
    "92s",
    "K8o",
    "A6o",
    "87o",
    "Q8o",
    "83s",
    "A2o",
    "82s",
    "97o",
    "72s",
    "76o",
    "K7o",
    "65o",
    "T7o",
    "K6o",
    "86o",
    "54o",
    "K5o",
    "J7o",
    "75o",
    "Q7o",
    "K4o",
    "K3o",
    "K2o",
    "96o",
    "64o",
    "Q6o",
    "53o",
    "85o",
    "T6o",
    "Q5o",
    "43o",
    "Q4o",
    "Q3o",
    "Q2o",
    "74o",
    "J6o",
    "63o",
    "J5o",
    "52o",
    "J4o",
    "J3o",
    "42o",
    "J2o",
    "84o",
    "T5o",
    "T4o",
    "T3o",
    "T2o",
    "32o",
    "73o",
    "62o",
    "94o",
    "93o",
    "92o",
    "83o",
    "82o",
    "72o",
};

const static std::vector<std::string> rankStr = {"2", "3", "4", "5", "6", "7", "8", "9", "T", "J", "Q", "K", "A"};

const std::vector<int> defaultDeckCards = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51};

int get_rank(const int holeCard1, const int holeCard2, const std::vector<int> &communityCards);

double eval_runout(const int ourcard1, const int ourcard2, const std::vector<int> &finalCommunityCards, const std::vector<int> &unseenbyriver);

double ehs2(const std::vector<int> &ourcards, const std::vector<int> &seenCommunityCards);
    
int get_winning_player(const std::vector<int> &player0Cards, const std::vector<int> &player1Cards, const std::vector<int> &communityCards);



#endif
