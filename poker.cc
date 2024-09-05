#include <algorithm>
#include <bitset>
#include <complex>
#include <deque>
#include <exception>
#include <fstream>
#include <functional>
#include <iomanip>
#include <ios>
#include <iosfwd>
#include <iostream>
#include <iterator>
#include <limits>
#include <list>
#include <map>
#include <memory>
#include <new>
#include <numeric>
#include <ostream>
#include <queue>
#include <random>
#include <set>
#include <sstream>
#include <streambuf>
#include <string>
#include <utility>
#include <valarray>
#include <vector>
#include <chrono>
#include <phevaluator/phevaluator.h>
typedef std::chrono::high_resolution_clock Clock;

using namespace std;

#define NUM_BUCKETS 10000
#define STARTING_STACK 100

const vector<int> defaultDeckCards = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51};

int total = 0;

struct Deck
{
    vector<int> cards;

    Deck() {
        cards = defaultDeckCards;
        auto rng = default_random_engine {};
        shuffle(cards.begin(), cards.end(), rng);
    }
};


struct InfoSetNode
{
    vector<double> strategySum;
    vector<double> regretSum;
    vector<InfoSetNode*> children;


    InfoSetNode() : strategySum(vector<double>(NUM_BUCKETS, 0)), regretSum(vector<double>(NUM_BUCKETS, 0)), children(vector<InfoSetNode*>(0, NULL)){}
    
    ~InfoSetNode() { for (auto &child : children) delete child;}

    vector<double> get_strat(){
        vector<double> strategy;
        double sum = 0;

        for (int i = 0; i<NUM_BUCKETS; i++){
            strategy.push_back(max(0.0, regretSum[i]));
            sum += strategy[i];
        }

        for (int i = 0; i<NUM_BUCKETS; i++){
            strategy[i] = (sum > 0) ? strategy[i]/sum : 1/NUM_BUCKETS;
        }
        
        return strategy;
    }

    vector<double> getAverageStrategy(){
        vector<double> avgStrategy;
        double sum = accumulate(avgStrategy.begin(), avgStrategy.end(), 0);

        for (int i = 0; i<NUM_BUCKETS; i++){
            avgStrategy[i] = (sum > 0) ? avgStrategy[i]/sum : 1/NUM_BUCKETS;
        }
        
        return avgStrategy;
    }
};


struct Game{
    InfoSetNode* r0;
    InfoSetNode* r1;
    int iter;

    Game() : r0(new InfoSetNode()), r1(new InfoSetNode()), iter(1) {}
    
    ~Game() { delete r0; delete r1;}
};


int getRank(const int holeCard1, const int holeCard2, const vector<int> &communityCards){
    total += 1;
    int len = communityCards.size();
    phevaluator::Rank rank;
    if (len == 3)
    {
        rank = phevaluator::EvaluateCards(holeCard1, holeCard2, communityCards[0], communityCards[1], communityCards[2]);
    }
    else if (len == 4)
    {
        rank = phevaluator::EvaluateCards(holeCard1, holeCard2, communityCards[0], communityCards[1], communityCards[2], communityCards[3]);
    }
    else if (len == 5)
    {
        rank = phevaluator::EvaluateCards(holeCard1, holeCard2, communityCards[0], communityCards[1], communityCards[2], communityCards[3], communityCards[4]);
    }
    else
    {
        throw invalid_argument( "received invalid number of elements");
    }

    return rank.value();
}

double evalrunout(const int ourcard1, const int ourcard2, const vector<int> &finalCommunityCards, const vector<int> &unseenbyriver)
{
    vector<int> outcome(3);
    int ourrank = getRank(ourcard1, ourcard2, finalCommunityCards);

    for (int m = 0; m< unseenbyriver.size(); m++) {
        for (int n = m + 1; n<unseenbyriver.size(); n++)
        {
            int opprank = getRank(unseenbyriver[m], unseenbyriver[n], finalCommunityCards);
            if (ourrank < opprank) outcome[0] += 1;
            else if (ourrank == opprank) outcome[1] += 1;
            else outcome[2] +=1;
        }
    }
    return (outcome[0] + outcome[1]/2.0)/(outcome[0] + outcome[1] + outcome[2]);
}

double ehs2(const vector<int> &ourcards, const vector<int> &seenCommunityCards)
{
    double res = 0, sum = 0;
    vector<int> unseen = defaultDeckCards;
    for (int card : ourcards) unseen.erase(remove(unseen.begin(), unseen.end(), card), unseen.end());
    for (int card : seenCommunityCards) unseen.erase(remove(unseen.begin(), unseen.end(), card), unseen.end());
    
    if (seenCommunityCards.size() == 3)
    {
        for (int i = 0; i< unseen.size(); i++) {
            for (int j = i + 1; j<unseen.size(); j++)
            {
                int turn = unseen[i], river = unseen[j];
                vector<int> finalCommunityCards = seenCommunityCards;
                finalCommunityCards.push_back(turn);
                finalCommunityCards.push_back(river);
                vector<int> unseenbyriver = unseen;
                unseenbyriver.erase(remove(unseenbyriver.begin(), unseenbyriver.end(), turn), unseenbyriver.end());
                unseenbyriver.erase(remove(unseenbyriver.begin(), unseenbyriver.end(), river), unseenbyriver.end());

                double hs = evalrunout(ourcards[0], ourcards[1], finalCommunityCards, unseenbyriver);
                res += hs*hs;
                sum +=1;
            }
        }
    }
    if (seenCommunityCards.size() == 4)
    {
        for (int i = 0; i< unseen.size(); i++) {
            int river = unseen[i];
            vector<int> finalCommunityCards = seenCommunityCards;
            finalCommunityCards.push_back(river);
            vector<int> unseenbyriver = unseen;
            unseenbyriver.erase(remove(unseenbyriver.begin(), unseenbyriver.end(), river), unseenbyriver.end());

            double hs = evalrunout(ourcards[0], ourcards[1], finalCommunityCards, unseenbyriver);
            res += hs*hs;
            sum +=1;
        }
    }
    if (seenCommunityCards.size() == 5)
    {
        vector<int> finalCommunityCards = seenCommunityCards;
        vector<int> unseenbyriver = unseen;
        double hs = evalrunout(ourcards[0], ourcards[1], finalCommunityCards, unseenbyriver);
        res += hs*hs;
        sum +=1;
    }
    cout << total << endl;
    cout << res << " " << sum << endl;
    return res/sum;
}


int main()
{

   std::printf("Start testing seven cards\n");
   auto t1 = Clock::now();
    vector<string> oc = {"Ad", "2d"};
    vector<string> scc = {"4d", "8d", "Tc"};

    const static std::unordered_map<char, int> rankMap = {
    {'2', 0}, {'3', 1}, {'4', 2}, {'5', 3},  {'6', 4},  {'7', 5},  {'8', 6},
    {'9', 7}, {'T', 8}, {'J', 9}, {'Q', 10}, {'K', 11}, {'A', 12},
    };
    const static std::unordered_map<char, int> suitMap = {
        {'C', 0}, {'D', 1}, {'H', 2}, {'S', 3},
        {'c', 0}, {'d', 1}, {'h', 2}, {'s', 3},
    };

    vector<int> ourcards;

    for (string name : oc)
    {
        ourcards.push_back(rankMap.at(name[0]) * 4 + suitMap.at(name[1]));
    }
    vector<int> seenCommCards; 
    for (string name : scc)
    {
        seenCommCards.push_back(rankMap.at(name[0]) * 4 + suitMap.at(name[1]));
    }
    
    cout << ehs2(ourcards, seenCommCards) << endl;

      auto t2 = Clock::now();
    std::cout << "Delta t2-t1: " 
              << std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count()
              << " nanoseconds" << std::endl;
}