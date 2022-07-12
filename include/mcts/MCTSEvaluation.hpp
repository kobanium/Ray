#ifndef _MCTS_EVALUATION_HPP_
#define _MCTS_EVALUATION_HPP_

#include <random>

#include "mcts/MCTSNode.hpp"


int SelectMaxUcbChild( uct_node_t &node, const int moves, const int color, std::mt19937_64 &mt );




#endif
