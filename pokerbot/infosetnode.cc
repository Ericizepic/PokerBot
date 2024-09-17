#include "infosetnode.h"

InfoSetNode::InfoSetNode() : strategySum(std::vector<double>(NUM_BUCKETS, 0)), regretSum(std::vector<double>(NUM_BUCKETS, 0)), children(std::vector<InfoSetNode*>(0, NULL)){}

InfoSetNode::~InfoSetNode() { for (auto &child : children) delete child;}

void InfoSetNode::init_children() {
    for (int i = 0; i<NUM_BUCKETS; i++)
    {
        InfoSetNode* child = new InfoSetNode();
        children.push_back(child);
    }
}

std::vector<double> InfoSetNode::get_strat(double discount){
    std::vector<double> strategy;
    double sum = 0;

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
    std::vector<double> avgStrategy = strategySum;
    double sum = accumulate(avgStrategy.begin(), avgStrategy.end(), 0);

    for (int i = 0; i<NUM_BUCKETS; i++){
        avgStrategy[i] = (sum > 0) ? avgStrategy[i]/sum : 1.0/NUM_BUCKETS;
    }
    
    return avgStrategy;
}

void InfoSetNode::save_info_node(std::string history)
{
    std::ofstream save_file;
    save_file.open("strat.txt", std::ofstream::app);
    if (isChanceNode)
    {
        for (int i = 0; i<NUM_BUCKETS; i++)
        {
            std::string new_history = history + std::to_string(i);
            children[i]->save_info_node(new_history);
        }
    }
    else
    {
        std::vector<double> avgstrat  = get_average_strategy();
        save_file << "For infonode: " << history << std::endl;
        for (int i = 0; i<NUM_BUCKETS; i++)
        {
            save_file << ALL_ACTIONS[i] << ": " << avgstrat[i] << std::endl;
        }

        if (children.size() > 0)
            for (int i = 0; i<NUM_BUCKETS; i++)
            {
                std::string new_history = history + ALL_ACTIONS[i];
                children[i]->save_info_node(new_history);
            }
    }
    save_file.close();
}
