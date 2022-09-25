#include <cmath>

#include "mcts/ucb/UCBEvaluation.hpp"

// 着手評価ボーナスの重みパラメータ
static double bonus_weight = BONUS_WEIGHT;

// 着手評価ボーナスの等価パラメータ
static double bonus_equivalence = BONUS_EQUIVALENCE;

// UCB1の係数
static double ucb_c = UCB_COEFFICIENT;


double
CalculateMoveScoreBonus( const child_node_t &child, const double move_score_bonus_weight )
{
  return move_score_bonus_weight * child.rate;
}



double
CalculateUCB1Value( const child_node_t &child, const int total_visits )
{
  const double exploitation_term = static_cast<double>(child.win) / child.move_count;
  const double exploration_term = std::sqrt(2.0 * std::log(total_visits) / child.move_count);

  return exploitation_term + ucb_c * exploration_term;
}




double
CalculateUCB1TunedValue( const child_node_t &child, const int total_visits )
{
  const double p = static_cast<double>(child.win) / child.move_count;
  const double div = std::log(total_visits) / child.move_count;
  const double v = p - p * p + std::sqrt(2.0 * div);

  return p + std::sqrt(div * ((0.25 < v) ? 0.25 : v));
}


int
SelectBestChildIndexByUCB1( const uct_node_t &node, std::mt19937_64 &mt )
{
  const int child_num = node.child_num;
  const int sum = node.move_count;
  const double move_score_bonus_weight = bonus_weight * sqrt(bonus_equivalence / (sum + bonus_equivalence));
  const child_node_t *child = node.child;
  int max_child = 0;
  double max_value = -10000.0;

  for (int i = 0; i < child_num; i++) {
    if (child[i].pw || child[i].open) {
      double ucb_value = 0.0;
      if (child[i].move_count == 0) {
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
