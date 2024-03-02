/**
 * @file include/mcts/MCTSEvaluation.hpp
 * @author Yuki Kobayashi
 * @~english
 * @brief Move evaluation and move selection for Monte-Carlo tree search.
 * @~japanese
 * @brief モンテカルロ木探索の着手評価と着手生成
 */
#ifndef _MCTS_EVALUATION_HPP_
#define _MCTS_EVALUATION_HPP_

#include <random>

#include "mcts/MCTSNode.hpp"


// UCB値最大の手を取得
int SelectMaxUcbChild( uct_node_t &node, const int moves, const int color, std::mt19937_64 &mt );

#endif
