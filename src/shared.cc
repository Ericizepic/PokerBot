#include "shared.h"
#include <omp.h>

int get_rank(const int holeCard1, const int holeCard2, const std::vector<int> &communityCards)
{
    int len = communityCards.size();
    if (len == 3)
    {
        return phevaluator::EvaluateCards(holeCard1, holeCard2, communityCards[0], communityCards[1], communityCards[2]).value();
    }
    else if (len == 4)
    {
        return phevaluator::EvaluateCards(holeCard1, holeCard2, communityCards[0], communityCards[1], communityCards[2], communityCards[3]).value();
    }
    else if (len == 5)
    {
        return phevaluator::EvaluateCards(holeCard1, holeCard2, communityCards[0], communityCards[1], communityCards[2], communityCards[3], communityCards[4]).value();
    }
    else
    {
        throw std::invalid_argument( "received invalid number of elements");
    }
}

double eval_runout(const int ourcard1, const int ourcard2, const std::vector<int> &finalCommunityCards, const std::vector<int> &unseenbyriver)
{
    const int ourrank = get_rank(ourcard1, ourcard2, finalCommunityCards);
    double num = 0;
    double denom = (unseenbyriver.size() - 1)*(unseenbyriver.size())/2.0;
    int size = unseenbyriver.size();

    omp_set_num_threads(14);

    #pragma omp parallel for reduction(+: num)
    for (int i = 0; i < size * (size - 1) / 2; i++) {
        int m = i / (size - 1);
        int n = i % (size - 1) + 1 + m;
        if (n >= size) {
            m++;
            n = m + 1;
        }
        int opprank = get_rank(unseenbyriver[m], unseenbyriver[n], finalCommunityCards);
        int val = 0;
        if (ourrank < opprank) val = 1;
        else if (ourrank == opprank) val = 0.5;

        num += val;
    }
    return num/denom;
}

double ehs2(const std::vector<int> &ourcards, const std::vector<int> &seenCommunityCards)
{
    double res = 0, sum = 0;
    std::vector<int> unseen = defaultDeckCards;
    for (int card : ourcards) unseen.erase(remove(unseen.begin(), unseen.end(), card), unseen.end());
    for (int card : seenCommunityCards) unseen.erase(remove(unseen.begin(), unseen.end(), card), unseen.end());
    
    if (seenCommunityCards.size() == 3)
    {
        std::vector<int> finalCommunityCards = seenCommunityCards; // Create local copies of vectors and reserve space to avoid reallocations
        finalCommunityCards.reserve(seenCommunityCards.size() + 2); // Reserve space for 2 additional cards

        std::vector<int> unseenbyriver = unseen;
    
        // We no longer need to modify `unseenbyriver` dynamically
        int unseenSize = unseen.size();
        for (int i = 0; i< 47; i++) {
            for (int j = i + 1; j<47; j++)
            {
                int turn = unseen[i], river = unseen[j];
                finalCommunityCards.push_back(turn);
                finalCommunityCards.push_back(river);
                unseenbyriver.erase(remove(unseenbyriver.begin(), unseenbyriver.end(), turn), unseenbyriver.end());
                unseenbyriver.erase(remove(unseenbyriver.begin(), unseenbyriver.end(), river), unseenbyriver.end());

                double hs = eval_runout(ourcards[0], ourcards[1], finalCommunityCards, unseenbyriver);
                res += hs*hs;

                finalCommunityCards.pop_back();
                finalCommunityCards.pop_back();
                unseenbyriver.push_back(turn);
                unseenbyriver.push_back(river);
            }
        }
        sum = 1081;//(47)*(46)/2
    }
    if (seenCommunityCards.size() == 4)
    {
        for (int i = 0; i< 46; i++) {
            int river = unseen[i];
            std::vector<int> finalCommunityCards = seenCommunityCards;
            finalCommunityCards.push_back(river);
            std::vector<int> unseenbyriver = unseen;
            unseenbyriver.erase(remove(unseenbyriver.begin(), unseenbyriver.end(), river), unseenbyriver.end());

            double hs = eval_runout(ourcards[0], ourcards[1], finalCommunityCards, unseenbyriver);
            res += hs*hs;
        }
        sum = 46;
    }
    if (seenCommunityCards.size() == 5)
    {
        std::vector<int> finalCommunityCards = seenCommunityCards;
        std::vector<int> unseenbyriver = unseen;
        double hs = eval_runout(ourcards[0], ourcards[1], finalCommunityCards, unseenbyriver);
        res += hs*hs;
        sum = 1;
        
    }
    return res/sum;
}


int get_winning_player(const std::vector<int> &player0Cards, const std::vector<int> &player1Cards, const std::vector<int> &communityCards){
    int rank0 = get_rank(player0Cards[0], player0Cards[1], communityCards);
    int rank1 = get_rank(player1Cards[0], player1Cards[1], communityCards);

    // smaller rank better
    return rank0 < rank1 ? 0 : 1;
}