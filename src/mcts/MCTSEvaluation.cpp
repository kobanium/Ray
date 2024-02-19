/**
 * @file src/mcts/MCTSEvaluation.cpp
 * @author Yuki Kobayashi
 * @~english
 * @brief Move evaluation and move selection for MCTS nodes.
 * @~japanese
 * @brief MCTSノードの着手評価と着手選択
 */
#include "mcts/MCTSEvaluation.hpp"
#include "mcts/ucb/UCBEvaluation.hpp"


/**
 * @~english
 * @brief Select next move.
 * @param[in] node MCTS node.
 * @param[in] moves The number of current moves.
 * @param[in] color Player's color.
 * @param[in] mt Random number generator.
 * @return Node index for next move.
 * @~japanese
 * @brief 次の着手の選択
 * @param[in] node MCTSノード
 * @param[in] moves 現在の手数
 * @param[in] color 手番の色
 * @param[in] mt 乱数生成器
 * @return 次の手に対応するノードのインデックス
 */
int
SelectMaxUcbChild( uct_node_t &node, const int moves, const int color, std::mt19937_64 &mt )
{
  const int move_count = node.move_count;

  if ((move_count & 0x7f) == 0 && move_count != 0) {
    //SortByDynamicParameters(node, color);
  }

  /*
  if (IsOverExpansionCount(node)) {
    AddCandidate(node);
  }
  */
  return SelectBestChildIndexByUCB1(node, mt);
}
