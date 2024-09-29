#include "game.h"
#include <chrono>
typedef std::chrono::high_resolution_clock Clock;


Game::Game() : root0(new InfoSetNode()), root1(new InfoSetNode()), deck(nullptr), iter(0) {}
    
Game::~Game() { delete root0; delete root1; delete deck;}

std::pair<double, double> Game::cfr(
    InfoSetNode* r0, 
    InfoSetNode* r1, 
    std::vector<Action> history,
    std::vector<double> remaining_stacks, 
    std::vector<double> bet_sizes,
    double pot_size, 
    int round_num, 
    int player,
    double p0, 
    double p1)
{
    if (is_terminal(history, round_num))
        return get_utility(remaining_stacks, bet_sizes, pot_size);

    if (r0->children.size() == 0)
    {
        r0->init_children();
    }
    if (r1->children.size() == 0)
    {
        r1->init_children();
    }

    if (is_non_terminal_round_end(history))
    {
        r0->isChanceNode = true;
        r1->isChanceNode = true;

        int next_round_num = (round_num==0) ? round_num + 3 : round_num + 1;
        std::vector<Action> next_hist = history;
        next_hist.push_back(NEXT_ROUND);

        auto t = get_next_stacks_bets_pot(NEXT_ROUND, remaining_stacks, bet_sizes, pot_size, 2);

        int bucket0 = get_bucket(next_round_num, 0), bucket1 = get_bucket(next_round_num, 1);

        return cfr(r0->children[bucket0], r1->children[bucket1], next_hist, std::get<0>(t), std::get<1>(t), std::get<2>(t), next_round_num, 0, p0, p1);
    }
    

    // int player = get_player(history);
    std::vector<double> state_action_util_0(NUM_BUCKETS, 0);
    std::vector<double> state_action_util_1(NUM_BUCKETS, 0);
    std::vector<Action> ACTIONS = get_actions(history, remaining_stacks, bet_sizes, pot_size, player);
    double u0 = 0, u1 = 0;

    std::vector<double> strat = (player == 0) ? r0->get_strat(p0) : r1->get_strat(p1);

    for (auto i : ACTIONS)
    {
        auto t = get_next_stacks_bets_pot(i, remaining_stacks, bet_sizes, pot_size, player);
        double nextp0 = (player == 0) ? strat[i]*p0 : p0;
        double nextp1 = (player == 1) ? strat[i]*p1 : p1;

        int len1 = history.size();
        history.push_back(i);
        std::pair<double, double> ua = cfr(r0->children[i], r1->children[i], history, std::get<0>(t), std::get<1>(t) , std::get<2>(t), round_num, 1 - player, nextp0, nextp1);
        history.pop_back();

        state_action_util_0[i] = ua.first;
        state_action_util_1[i] = ua.second;
        u0 += state_action_util_0[i]*strat[i];
        u1 += state_action_util_1[i]*strat[i];
    }

    if (player == 0)
    {
        for (int i = 0; i<NUM_BUCKETS; i++)
            r0->regretSum[i] = (iter*r0->regretSum[i] + p1*(state_action_util_0[i] - u0))/(iter + 1);
    }
    else
    {
        for (int i = 0; i<NUM_BUCKETS; i++)
            r1->regretSum[i] = (iter*r1->regretSum[i] + p0*(state_action_util_1[i] - u1))/(iter + 1);
    }

   return std::make_pair(u0,u1);
}


inline bool Game::is_non_terminal_round_end(std::vector<Action> history){
    int len = history.size();
    if (len < 2)
        return false;
    
    bool both_player_has_acted = history[len - 2] != NEXT_ROUND && history[len - 1] != NEXT_ROUND;
    bool is_action_closed = history[len - 1] == CALL;

    return both_player_has_acted && is_action_closed;
}


inline bool Game::is_terminal(std::vector<Action> history, int round_num){
    int len = history.size();

    bool is_river_terminal = is_non_terminal_round_end(history) && round_num == 5;
    bool is_all_in_call = len >= 2 && history[len-2] == ALL_IN && history[len-2] == CALL;
    bool is_fold = len >= 1 && history[len-1] == FOLD;

    return is_river_terminal || is_all_in_call || is_fold;
}


std::tuple<std::vector<double>, std::vector<double>, double> Game::get_next_stacks_bets_pot(Action action, std::vector<double> remaining_stacks, std::vector<double> bet_sizes, double pot_size, int player){
    double next_pot_size = pot_size;
    std::vector<double> next_remaining_stacks = remaining_stacks;
    std::vector<double> next_bet_sizes = bet_sizes;

    if (action == NEXT_ROUND) // new round
    {
        next_remaining_stacks[0] -= bet_sizes[0];
        next_remaining_stacks[1] -= bet_sizes[1];

        next_pot_size += bet_sizes[0] + bet_sizes[1];
        next_bet_sizes = {0,0};
    }
    else
    {
        if (action == CALL)
            next_bet_sizes[player] = bet_sizes[1  - player];
        if (action == RAISE50)
            next_bet_sizes[player] = 2*bet_sizes[1  - player] + 0.5*pot_size;
        if (action == RAISE100)
            next_bet_sizes[player] = 3*bet_sizes[1  - player] + pot_size;
        if (action == ALL_IN)
            next_bet_sizes[player] = remaining_stacks[player];
    }
    
    return {next_remaining_stacks, next_bet_sizes, next_pot_size};
}


std::vector<Action> Game::get_actions(std::vector<Action> history, std::vector<double> remaining_stacks, std::vector<double> bet_sizes, double pot_size, int player){
    std::vector<Action> allowed_actions = {CALL};// c
    if (3*bet_sizes[1 - player] + pot_size <= remaining_stacks[player])
        allowed_actions.push_back(RAISE100); // r100
    if (2*bet_sizes[1 - player] + 0.5*pot_size <= remaining_stacks[player])
        allowed_actions.push_back(RAISE50); // r50
    if (history.size() == 0 || history.back() != ALL_IN)
        allowed_actions.push_back(ALL_IN); // a
    if (bet_sizes[1 - player] != 0)
        allowed_actions.push_back(FOLD); // f not disallowed but there is no point in foldinig

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


inline int Game::get_bucket(int round_num, int player){
    double hs_metric = ehs2(deck->getPlayerCards(player), deck->getCommunityCards(round_num));
    return std::min((int)floor(NUM_BUCKETS*hs_metric), NUM_BUCKETS - 1); // bigger bucket better
}

int Game::get_init_bucket(int player)
{
    std::vector<int> cards = deck->getPlayerCards(player);
    int card1 = std::max(cards[0]/4, cards[1]/4), card2 = std::min(cards[0]/4, cards[1]/4);

    std::string handstring = rankStr[card1] + rankStr[card2];
    if (cards[0]/4 != cards[1]/4)
        handstring += (cards[0]%4 == cards[1]%4) ? "s" : "o";

    int index = 168 - (std::find(STARTING_HAND_RANKINGS.begin(), STARTING_HAND_RANKINGS.end(), handstring) - STARTING_HAND_RANKINGS.begin());
    return floor(index*NUM_BUCKETS/169);
}


void Game::train(int epochs){
    std::printf("Starting regret minimization\n");
    auto t1 = Clock::now();
    double avgutil0 = 0, avgutil1 = 0;
    for (int epoch = 1; epoch <= epochs; epoch++) {
        deck = new Deck();
        iter +=1;

        std::vector<Action> emptyhist;
        std::vector<double> starting_stacks = {STARTING_STACK, STARTING_STACK};
        std::vector<double> blinds = {1, 1};
        if (root0->children.size() == 0)
            root0->init_children();
        if (root1->children.size() == 0)
            root1->init_children();
        root0->isChanceNode = true;
        root1->isChanceNode = true;

        int bucket0 = get_init_bucket(0), bucket1 = get_init_bucket(1);
        std::pair<double, double> util = cfr(root0->children[bucket0], root1->children[bucket1], emptyhist, starting_stacks, blinds, 0, 0, 0, 1, 1);
        avgutil0 += util.first;
        avgutil1 += util.second;

        if (epoch % 5 == 0) {
            auto t2 = Clock::now();
            std::printf("Epoch %d: %f %f in %fs\n", epoch, avgutil0, avgutil1, std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count()/1e9);
            t1 = t2;
            avgutil0 = 0, avgutil1 = 0;
        }
    }
    std::printf("Completed regret minimization.\n");
    std::printf("Saving final strategy profile");
    root0->save_info_node("");
}