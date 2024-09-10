#ifndef INFOSETNODE_H
#define INFOSETNODE_H

#include <algorithm>
#include <vector>
#include <numeric>


#define NUM_BUCKETS 10000
#define STARTING_STACK 100

struct InfoSetNode
{
    std::vector<double> strategySum;
    std::vector<double> regretSum;
    std::vector<InfoSetNode*> children;

    InfoSetNode();
    ~InfoSetNode();
    void init_children();
    std::vector<double> get_strat();
    std::vector<double> get_average_strategy();
};


#endif
