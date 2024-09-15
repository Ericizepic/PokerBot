#include "game.h"
#include <chrono>
typedef std::chrono::high_resolution_clock Clock;


Game::Game() : root0(new InfoSetNode()), root1(new InfoSetNode()), deck(nullptr), iter(1) {}
    
Game::~Game() { delete root0; delete root1;}

std::pair<double, double> Game::cfr(
    InfoSetNode* r0, 
    InfoSetNode* r1, 
    std::vector<std::string> history,
    std::vector<double> remaining_stacks, 
    std::vector<double> bet_sizes,
    double pot_size, 
    int round_num, 
    double p0, 
    double p1)
{
    if (is_terminal(history, round_num))
        return get_utility(remaining_stacks, bet_sizes, pot_size);
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

        auto t = get_next_stacks_bets_pot("|", remaining_stacks, bet_sizes, pot_size, 2);
        int bucket0 = get_bucket(next_round_num, 0), bucket1 = get_bucket(next_round_num, 1);

        return cfr(r0->children[bucket0], r1->children[bucket1], next_hist, std::get<0>(t), std::get<1>(t), std::get<2>(t), next_round_num, p0, p1);
    }
        
    int player = get_player(history);
    std::vector<double> state_action_util_0(NUM_BUCKETS, 0);
    std::vector<double> state_action_util_1(NUM_BUCKETS, 0);
    std::vector<std::string> ACTIONS = get_actions(history, remaining_stacks, bet_sizes, pot_size, player);
    double u0 = 0, u1 = 0;

    std::vector<double> strat = (player == 0) ? r0->get_strat() : r1->get_strat();

    for (int i = 0; i<ACTIONS.size(); i++)
    {
        std::vector<std::string> next_hist = history;
        next_hist.push_back(ACTIONS[i]);

        auto t = get_next_stacks_bets_pot(ACTIONS[i], remaining_stacks, bet_sizes, pot_size, player);

        if (r0->children.size() == 0 || r1->children.size() == 0)
        {
            r0->init_children();
            r1->init_children();
        }

        double nextp0 = (player == 0) ? strat[i]*p0 : p0;
        double nextp1 = (player == 1) ? strat[i]*p1 : p1;

        std::pair<double, double> ua = cfr(r0->children[i], r1->children[i], next_hist, std::get<0>(t), std::get<1>(t) , std::get<2>(t), round_num, nextp0, nextp1);
        state_action_util_0[i] = ua.first;
        state_action_util_1[i] = ua.second;
        u0 += state_action_util_0[i]*strat[i];
        u1 += state_action_util_1[i]*strat[i];
    }

    if (player == 0)
    {
        for (int i = 0; i<r0->regretSum.size(); i++)
            r0->regretSum[i] = (iter*r0->regretSum[i] + p1*(state_action_util_0[i] - u0))/(iter + 1);
    }
    else
    {
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

    bool is_river_terminal = is_non_terminal_round_end(history) && round_num == 5;
    bool is_all_in_call = len >= 2 && history[len-2] == "a" && history[len-2] == "c";
    bool is_fold = len >= 1 && history[len-1] == "f";

    return is_river_terminal || is_all_in_call || is_fold;
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


std::tuple<std::vector<double>, std::vector<double>, double> Game::get_next_stacks_bets_pot(std::string action, std::vector<double> remaining_stacks, std::vector<double> bet_sizes, double pot_size, int player){
    double next_pot_size = pot_size;
    std::vector<double> next_remaining_stacks = remaining_stacks;
    std::vector<double> next_bet_sizes = bet_sizes;

    if (action == "|") // new round
    {
        next_remaining_stacks[0] -= bet_sizes[0];
        next_remaining_stacks[1] -= bet_sizes[1];

        next_pot_size += bet_sizes[0] + bet_sizes[1];
        next_bet_sizes = {0,0};
    }
    else
    {
        if (action == "c")
            next_bet_sizes[player] = bet_sizes[1  - player];
        if (action == "r50")
            next_bet_sizes[player] = 2*bet_sizes[1  - player] + 0.5*pot_size;
        if (action == "r100")
            next_bet_sizes[player] = 3*bet_sizes[1  - player] + pot_size;
        if (action == "a")
            next_bet_sizes[player] = remaining_stacks[player];
    }
    
    return {next_remaining_stacks, next_bet_sizes, next_pot_size};
}


std::vector<std::string> Game::get_actions(std::vector<std::string> history, std::vector<double> remaining_stacks, std::vector<double> bet_sizes, double pot_size, int player){
    std::vector<std::string> allowed_actions = ALL_ACTIONS;
    if (3*bet_sizes[1 - player] + pot_size > remaining_stacks[player])
        allowed_actions.erase(std::remove(allowed_actions.begin(), allowed_actions.end(), "r100"), allowed_actions.end());
    if (2*bet_sizes[1 - player] + 0.5*pot_size > remaining_stacks[player])
        allowed_actions.erase(std::remove(allowed_actions.begin(), allowed_actions.end(), "r50"), allowed_actions.end());
    if (history.size() > 0 && history.back() == "a")
        allowed_actions.erase(std::remove(allowed_actions.begin(), allowed_actions.end(), "a"), allowed_actions.end());
    if (bet_sizes[1 - player] == 0)
        allowed_actions.erase(std::remove(allowed_actions.begin(), allowed_actions.end(), "f"), allowed_actions.end()); // not disallowed but there is no point in foldinig

    return allowed_actions;
}


std::pair<double, double> Game::get_utility(std::vector<double> remaining_stacks, std::vector<double> bet_sizes, double pot_size){
    remaining_stacks[0] -= bet_sizes[0];
    remaining_stacks[1] -= bet_sizes[1];
    pot_size += bet_sizes[0] + bet_sizes[1];

    std::vector<double> utilities = {remaining_stacks[0] - STARTING_STACK, remaining_stacks[1] - STARTING_STACK};
    int winner = get_winning_player(deck->getPlayerCards(0), deck->getPlayerCards(1), deck->getCommunityCards(5));
    utilities[winner] += pot_size;

    if (utilities[0] + utilities[1] != 0)
    {
        std::cout << remaining_stacks[0] << " " << remaining_stacks[1] << " " << pot_size << std::endl;
        std::cout << bet_sizes[0] << " " << bet_sizes[1] << " " << pot_size << std::endl;
        std::cout << utilities[0] << " " << utilities[1] << " " << pot_size << std::endl;

        throw std::invalid_argument("utilies do not add up to 0");
    }

    return std::make_pair(utilities[0], utilities[1]);
}


int Game::get_bucket(int round_num, int player){
    double hs_metric = ehs2(deck->getPlayerCards(player), deck->getCommunityCards(round_num));
    return floor(NUM_BUCKETS*hs_metric);
}


void Game::train(int epochs){
    std::pair<double, double> utils = {0,0};
    int progress = 0;

    std::printf("Starting regret minimization\n");
    auto t1 = Clock::now();


    for (int epoch = 1; epoch <= epochs; epoch++) {
        deck = new Deck();
        iter +=1;
        std::vector<std::string> emptyhist;
        std::vector<double> starting_stacks = {STARTING_STACK, STARTING_STACK};
        std::vector<double> blinds = {1, 1};
        std::pair<double, double> res = cfr(root0, root1, emptyhist, starting_stacks, blinds, 0,  0, 1, 1);
        utils = std::make_pair(utils.first + res.first, utils.second + res.second);

        if (epoch % 1 == 0) {
            std::printf("Epoch %d: %f %f in %ldns\n", epoch, utils.first, utils.second, std::chrono::duration_cast<std::chrono::nanoseconds>(Clock::now() - t1).count());
        }
    }
    std::printf("Completed regret minimization.\n");
    std::printf("Final strategy profile:");
}