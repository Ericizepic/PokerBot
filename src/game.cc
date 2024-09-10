#include "game.h"

Game::Game() : root0(new InfoSetNode()), root1(new InfoSetNode()), deck(nullptr), iter(1) {}
    
Game::~Game() { delete root0; delete root1;}

std::pair<double, double> Game::cfr(InfoSetNode* r0, InfoSetNode* r1, std::vector<std::string> history, int round_num, double p0, double p1)
{
    if (is_terminal(history, round_num))
        return get_utility(history);
    if (is_non_terminal_round_end(history))
    {
        if (r0->children.size() == 0)
        {
            r0->init_children();
            r1->init_children();
        }

        int next_round_num = (round_num==0) ? round_num + 3 : round_num + 1;
        std::vector<std::string> next_hist = history;
        next_hist.push_back("|");
        int bucket0 = get_bucket(next_round_num, 0), bucket1 = get_bucket(next_round_num, 1);
        return cfr(r0->children[bucket0], r1->children[bucket1], next_hist, next_round_num, p0, p1);
    }
        
    int player = get_player(history);
    std::vector<double> state_action_util_0(NUM_BUCKETS, 0);
    std::vector<double> state_action_util_1(NUM_BUCKETS, 0);
    std::vector<std::string> ACTIONS = get_actions(history);
    double u0 = 0, u1 = 0;

    if (player == 0)
    {
        std::vector<double> strat = r0->get_strat();
        for (int i = 0; i<ACTIONS.size(); i++)
        {
            std::vector<std::string> next_hist = history;
            next_hist.push_back(ACTIONS[i]);
            std::pair<double, double> ua = cfr(r0->children[i], r1->children[i], next_hist, round_num, strat[i]*p0, p1);
            state_action_util_0[i] = ua.first;
            state_action_util_1[i] = ua.second;
            u0 += state_action_util_0[i]*strat[i];
            u1 += state_action_util_1[i]*strat[i];
        }

        for (int i = 0; i<r0->regretSum.size(); i++)
            r0->regretSum[i] = (iter*r0->regretSum[i] + p1*(state_action_util_0[i] - u0))/(iter + 1);
    }
    else
    {
        std::vector<double> strat = r1->get_strat();
        for (int i = 0; i<ACTIONS.size(); i++)
        {
            std::vector<std::string> next_hist = history;
            next_hist.push_back(ACTIONS[i]);
            std::pair<double, double> ua = cfr(r0->children[i], r1->children[i], next_hist, round_num, p0, strat[i]*p1);
            state_action_util_0[i] = ua.first;
            state_action_util_1[i] = ua.second;
            u0 += state_action_util_0[i]*strat[i];
            u1 += state_action_util_1[i]*strat[i];
        }

        for (int i = 0; i<r0->regretSum.size(); i++)
            r1->regretSum[i] = (iter*r1->regretSum[i] + p0*(state_action_util_1[i] - u1))/(iter + 1);
    }


   return std::make_pair(u0,u1);
}


bool Game::is_non_terminal_round_end(std::vector<std::string> history){
    int len = history.size();
    if (len < 2)
        return false;
    
    bool both_player_has_acted = history[len - 2] != "|" && history[len - 1] != "|";
    bool is_action_closed = history[len - 1] == "c";

    return both_player_has_acted && is_action_closed;
}


bool Game::is_terminal(std::vector<std::string> history, int round_num){
    int len = history.size();
    if (len < 2)
        return false;

    bool is_river_terminal = is_non_terminal_round_end(history) && round_num == 5;
    bool is_all_in = history[len-2] == "a" && history[len-2] == "c";
    bool is_fold = history[len-1] == "f";

    return is_river_terminal || is_all_in || is_fold;
}


int Game::get_player(std::vector<std::string> history){
    int player = 0;
    for (int i = 0; i<history.size(); i++)
    {
        if (history[i] == "|") player = 0;
        player = 1 - player;
    }
    return player;
}


std::pair<std::vector<double>, std::vector<std::string>> Game::get_bet_sizes_and_actions(std::vector<std::string> history){
    double pot_size = 0;
    std::vector<double> remaining_stacks(2, STARTING_STACK);
    std::vector<std::string> allowed_actions = ALL_ACTIONS;
    
    int player = 0, opp = 1, curr_round = 0;
    std::vector<double> bet_sizes(2, 1);

    for (std::string action : history)
    {
        if (action == "|") // new round
        {
            bet_sizes = {0,0};
            remaining_stacks[0] -= bet_sizes[0];
            remaining_stacks[1] -= bet_sizes[1];

            pot_size += bet_sizes[0] + bet_sizes[1];
            player = 0;
            opp = 1;
            curr_round = (curr_round == 0) ? 3 : curr_round + 1;
        }
        else
        {
            if (action == "c")
                bet_sizes[player] = bet_sizes[opp];
            if (action == "r50")
                bet_sizes[player] = 2*bet_sizes[opp] + 0.5*pot_size;
            if (action == "r100")
                bet_sizes[player] = 3*bet_sizes[opp] + pot_size;
            if (action == "a")
                bet_sizes[player] = remaining_stacks[player];

            player = 1 - player;
            opp = 1 - opp;
        }
    }

    if (3*bet_sizes[player] + pot_size > remaining_stacks[opp])
        allowed_actions.erase(std::remove(allowed_actions.begin(), allowed_actions.end(), "r100"), allowed_actions.end());
    if (2*bet_sizes[player] + 0.5*pot_size > remaining_stacks[opp])
        allowed_actions.erase(std::remove(allowed_actions.begin(), allowed_actions.end(), "r50"), allowed_actions.end());
    if (history.size() > 0 && history.back() == "a")
        allowed_actions.erase(std::remove(allowed_actions.begin(), allowed_actions.end(), "a"), allowed_actions.end());
    
    return make_pair(bet_sizes, allowed_actions);
}


std::vector<std::string> Game::get_actions(std::vector<std::string> history){
    return get_bet_sizes_and_actions(history).second;
}


std::pair<double, double> Game::get_utility(std::vector<std::string> history){
    std::vector<double> bet_sizes = get_bet_sizes_and_actions(history).first;
    int winner = get_winning_player(deck->getPlayerCards(0), deck->getPlayerCards(1), deck->getCommunityCards(5));
    bet_sizes[1 - winner] = -1*bet_sizes[1 - winner];
    return std::make_pair(bet_sizes[0], bet_sizes[1]);
}


int Game::get_bucket(int round_num, int player){
    double hs_metric = ehs2(deck->getPlayerCards(player), deck->getCommunityCards(round_num));
    return floor(NUM_BUCKETS*hs_metric);
}


void Game::train(int epochs){
    std::pair<double, double> utils = {0,0};
    int count = 0;
    int progress = 0;

    std::printf("Starting regret minimization\n");

    for (int epoch = 1; epoch <= epochs; epoch++) {
        deck = new Deck();
        iter +=1;
        std::vector<std::string> emptyhist;
        utils = utils + cfr(root0, root1, emptyhist, 0, 1, 1);

        if (count % 1000 == 0) {
            std::printf("Epoch $%d%%: %d\n", epoch, utils.first, utils.second);
        }

        count++;
    }
    std::printf("Completed regret minimization.\n");
    std::printf("Final strategy profile:");
}