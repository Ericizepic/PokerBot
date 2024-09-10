#include "shared.h"

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
    std::vector<int> outcome(3);
    int ourrank = get_rank(ourcard1, ourcard2, finalCommunityCards);

    for (int m = 0; m< unseenbyriver.size(); m++) {
        for (int n = m + 1; n<unseenbyriver.size(); n++)
        {
            int opprank = get_rank(unseenbyriver[m], unseenbyriver[n], finalCommunityCards);
            if (ourrank < opprank) outcome[0] += 1;
            else if (ourrank == opprank) outcome[1] += 1;
            else outcome[2] +=1;
        }
    }
    return (outcome[0] + outcome[1]/2.0)/(outcome[0] + outcome[1] + outcome[2]);
}

double ehs2(const std::vector<int> &ourcards, const std::vector<int> &seenCommunityCards)
{
    double res = 0, sum = 0;
    std::vector<int> unseen = defaultDeckCards;
    for (int card : ourcards) unseen.erase(remove(unseen.begin(), unseen.end(), card), unseen.end());
    for (int card : seenCommunityCards) unseen.erase(remove(unseen.begin(), unseen.end(), card), unseen.end());
    
    if (seenCommunityCards.size() == 3)
    {
        for (int i = 0; i< unseen.size(); i++) {
            for (int j = i + 1; j<unseen.size(); j++)
            {
                int turn = unseen[i], river = unseen[j];
                std::vector<int> finalCommunityCards = seenCommunityCards;
                finalCommunityCards.push_back(turn);
                finalCommunityCards.push_back(river);
                std::vector<int> unseenbyriver = unseen;
                unseenbyriver.erase(remove(unseenbyriver.begin(), unseenbyriver.end(), turn), unseenbyriver.end());
                unseenbyriver.erase(remove(unseenbyriver.begin(), unseenbyriver.end(), river), unseenbyriver.end());

                double hs = eval_runout(ourcards[0], ourcards[1], finalCommunityCards, unseenbyriver);
                res += hs*hs;
                sum +=1;
            }
        }
    }
    if (seenCommunityCards.size() == 4)
    {
        for (int i = 0; i< unseen.size(); i++) {
            int river = unseen[i];
            std::vector<int> finalCommunityCards = seenCommunityCards;
            finalCommunityCards.push_back(river);
            std::vector<int> unseenbyriver = unseen;
            unseenbyriver.erase(remove(unseenbyriver.begin(), unseenbyriver.end(), river), unseenbyriver.end());

            double hs = eval_runout(ourcards[0], ourcards[1], finalCommunityCards, unseenbyriver);
            res += hs*hs;
            sum +=1;
        }
    }
    if (seenCommunityCards.size() == 5)
    {
        std::vector<int> finalCommunityCards = seenCommunityCards;
        std::vector<int> unseenbyriver = unseen;
        double hs = eval_runout(ourcards[0], ourcards[1], finalCommunityCards, unseenbyriver);
        res += hs*hs;
        sum +=1;
    }
    std::cout << res << " " << sum << std::endl;
    return res/sum;
}


int get_winning_player(const std::vector<int> &player0Cards, const std::vector<int> &player1Cards, const std::vector<int> &communityCards){
    int rank0 = get_rank(player0Cards[0], player0Cards[1], communityCards);
    int rank1 = get_rank(player1Cards[0], player1Cards[1], communityCards);

    // smaller rank better
    return rank0 < rank1 ? 0 : 1;
}