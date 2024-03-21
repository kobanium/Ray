/**
 * @file src/mcts/ucb/UCBEvaluation.cpp
 * @author Yuki Kobayashi
 * @~english
 * @brief Evaluation of upper confidence bound.
 * @~japanese
 * @brief UCBによる評価
 */
#include <cmath>

#include "mcts/ucb/UCBEvaluation.hpp"


/**
 * @~english
 * @brief Move evaluation bonus weight.
 * @~japanese
 * @brief 着手評価ボーナスの重み
 */
static double bonus_weight = BONUS_WEIGHT;

/**
 * @~english
 * @brief Equivalence parameter for move evaluation bonus.
 * @~japanese
 * @brief 着手評価ボーナスの等価パラメータ
 */
static double bonus_equivalence = BONUS_EQUIVALENCE;

/**
 * @~english
 * @brief Constant value for UCB1.
 * @~japanese
 * @brief UCB1値の定数
 */
static double ucb_c = UCB_COEFFICIENT;


/**
 * @~english
 * @brief Calculate move score bonus.
 * @param[in] child Next move candidate.
 * @param[in] move_score_bonus_weight Bonus weight.
 * @return Move score bonus.
 * @~japanese
 * @brief 着手評価値によるボーナスを返す
 * @param[in] child 次の着手のノード
 * @param[in] move_score_bonus_weight ボーナスの重み
 * @return 着手評価のボーナス
 */
double
CalculateMoveScoreBonus( const child_node_t &child, const double move_score_bonus_weight )
{
  return move_score_bonus_weight * child.rate;
}


/**
 * @~english
 * @brief Calculate UCB1 value.
 * @param[in] child Next move candidate.
 * @param[in] total_visits Total visits count of current node.
 * @return UCB1 value.
 * @~japanese
 * @brief UCB1値を返す
 * @param[in] child 次の着手のノード
 * @param[in] total_visits 現在のノードの探索回数合計値
 * @return UCB1値
 */
double
CalculateUCB1Value( const child_node_t &child, const int total_visits )
{
  const int move_count = child.move_count + child.virtual_loss.load();
  const double exploitation_term = static_cast<double>(child.win) / move_count;
  const double exploration_term = std::sqrt(2.0 * std::log(total_visits) / move_count);

  return exploitation_term + ucb_c * exploration_term;
}


/**
 * @~english
 * @brief Calculate UCB1-Tuned value.
 * @param[in] child Next move candidate.
 * @param[in] total_visits Total visits count of a current node.
 * @return UCB1-Tuned value.
 * @~japanese
 * @brief UCB1-Tuned値を返す
 * @param[in] child 次の着手のノード
 * @param[in] total_visits 現在のノードの探索回数合計値
 * @return UCB1-Tuned値
 */
double
CalculateUCB1TunedValue( const child_node_t &child, const int total_visits )
{
  const int move_count = child.move_count + child.virtual_loss.load();
  const double p = static_cast<double>(child.win) / move_count;
  const double div = std::log(total_visits) / move_count;
  const double v = p - p * p + std::sqrt(2.0 * div);

  return p + std::sqrt(div * ((0.25 < v) ? 0.25 : v));
}


/**
 * @~english
 * @brief Select child node by UCB1 value .
 * @param[in] node Current node.
 * @param[in] mt Random number generator.
 * @return Next move node index.
 * @~japanese
 * @brief UCB1値最大の子ノードを返す
 * @param[in] node 現在のノード
 * @param[in] mt 乱数生成器
 * @return 次の着手のノードのインデックス
 */
int
SelectBestChildIndexByUCB1( const uct_node_t &node, std::mt19937_64 &mt )
{
  const int child_num = node.child_num;
  const int sum = node.move_count + node.virtual_loss.load();
  const double move_score_bonus_weight = bonus_weight * sqrt(bonus_equivalence / (sum + bonus_equivalence));
  const child_node_t *child = node.child;
  int max_child = 0;
  double max_value = -10000.0;

  for (int i = 0; i < child_num; i++) {
    if (child[i].pw || child[i].open) {
      const int move_count = child[i].move_count + child[i].virtual_loss.load();
      double ucb_value = 0.0;
      if (move_count == 0) {
        ucb_value = FPU + 0.0001 * (mt() % 10000);
      } else {
        ucb_value = CalculateUCB1TunedValue(child[i], sum) + CalculateMoveScoreBonus(child[i], move_score_bonus_weight);
      }
      if (ucb_value > max_value) {
        max_value = ucb_value;
        max_child = i;
      }
    }
  }
  
  return max_child;
}
