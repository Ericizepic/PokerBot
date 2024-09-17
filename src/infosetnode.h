#ifndef INFOSETNODE_H
#define INFOSETNODE_H

#include <algorithm>
#include <vector>
#include <numeric>
#include "shared.h"


struct InfoSetNode
{
    std::vector<double> strategySum;
    std::vector<double> regretSum;
    std::vector<InfoSetNode*> children;
    bool isChanceNode = false;

    InfoSetNode();
    ~InfoSetNode();
    void init_children();
    std::vector<double> get_strat(double discount);
    std::vector<double> get_average_strategy();
    void save_info_node(std::string history);
};


#endif
