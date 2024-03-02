/**
 * @file include/mcts/ucb/UCBEvaluation.hpp
 * @author Yuki Kobayashi
 * @~english
 * @brief Evaluation of upper confidence bound.
 * @~japanese
 * @brief UCBによる評価
 */
#ifndef _UCB_EVALUATION_HPP_
#define _UCB_EVALUATION_HPP_

#include <random>

#include "mcts/MCTSNode.hpp"


/**
 * @~english
 * @brief First play urgency.
 * @~japanese
 * @brief FPUの定数
 */
constexpr double FPU = 5.0;

/**
 * @~english
 * @brief Move evaluation bonus weight.
 * @~japanese
 * @brief 着手評価ボーナスの重み
 */
constexpr double BONUS_WEIGHT = 0.20;

/**
 * @~english
 * @brief Equivalence parameter for move evaluation bonus.
 * @~japanese
 * @brief 着手評価ボーナスの等価パラメータ
 */
constexpr double BONUS_EQUIVALENCE = 1000;

/**
 * @~english
 * @brief Constant value for UCB1.
 * @~japanese
 * @brief UCB1の定数
 */
constexpr double UCB_COEFFICIENT = 0.60;


// UCB1値の計算
double CalculateUCB1Value( const child_node_t &child, const int total_visits );

// UCB1-Tuned値の計算
double CalculateUCB1TunedValue( const child_node_t &child, const int total_visits );

// UCB1値が最大の手を取得
int SelectBestChildIndexByUCB1( const uct_node_t &node, std::mt19937_64 &mt );

#endif
