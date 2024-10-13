#include "infosetnode.h"

InfoSetNode::InfoSetNode() : strategySum(std::vector<double>(NUM_BUCKETS, 0)), regretSum(std::vector<double>(NUM_BUCKETS, 0)), children(std::vector<InfoSetNode*>(0, NULL)){}

InfoSetNode::~InfoSetNode() { 
    for (auto child : children) 
        delete child;
    children.clear();
}

void InfoSetNode::init_children() {
    if (children.size() != 0)
    {
        throw std::invalid_argument("children has already been initalized");
    }

    children = std::vector<InfoSetNode*>(NUM_BUCKETS, nullptr);
    for (int i = 0; i<NUM_BUCKETS; i++)
        children[i] = new InfoSetNode();
}

std::vector<double> InfoSetNode::get_strat(double discount){
    std::vector<double> strategy;
    double sum = 0;
    visitCount+=1;

    for (int i = 0; i<NUM_BUCKETS; i++){
        strategy.push_back(std::max(0.0, regretSum[i]));
        sum += strategy[i];
    }

    for (int i = 0; i<NUM_BUCKETS; i++){
        strategy[i] = (sum > 0) ? strategy[i]/sum : 1.0/NUM_BUCKETS;
        strategySum[i] += strategy[i]*discount;
    }

    return strategy;
}

std::vector<double> InfoSetNode::get_average_strategy(){
    std::vector<double> avgStrategy;
    double sum = 0;
    for (int i = 0; i<NUM_BUCKETS; i++){
        avgStrategy.push_back(std::max(0.0, strategySum[i]));
        sum += avgStrategy[i];
    }

    for (int i = 0; i<NUM_BUCKETS; i++){
        avgStrategy[i] = (sum > 0) ? avgStrategy[i]/sum : 1.0/NUM_BUCKETS;
    }
    
    return avgStrategy;
}
