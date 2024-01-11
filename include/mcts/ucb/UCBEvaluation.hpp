#ifndef _UCB_EVALUATION_HPP_
#define _UCB_EVALUATION_HPP_

#include <random>

#include "mcts/MCTSNode.hpp"


constexpr double FPU = 5.0;

constexpr double BONUS_WEIGHT = 0.35;

constexpr double BONUS_EQUIVALENCE = 1000;

constexpr double UCB_COEFFICIENT = 0.60;



double CalculateUCB1Value( const child_node_t &child, const int total_visits );


double CalculateUCB1TunedValue( const child_node_t &child, const int total_visits );


int SelectBestChildIndexByUCB1( const uct_node_t &node, std::mt19937_64 &mt );


#endif
