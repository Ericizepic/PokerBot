#ifndef INFOSETNODE_H
#define INFOSETNODE_H

#include <algorithm>
#include <vector>
#include <numeric>
#include "shared.h"
#include <nlohmann/json.hpp>


struct InfoSetNode
{
    std::vector<double> strategySum;
    std::vector<double> regretSum;
    std::vector<InfoSetNode*> children;
    bool isChanceNode = false;
    int visitCount = 0;

    InfoSetNode();
    ~InfoSetNode();
    void init_children();
    std::vector<double> get_strat(double discount);
    std::vector<double> get_average_strategy();
};


#endif
