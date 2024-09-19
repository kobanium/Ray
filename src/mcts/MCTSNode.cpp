/**
 * @file MCTSNode.cpp
 * @author Yuki Kobayashi
 * @~english
 * @brief Operation for MCTS nodes.
 * @~japanese
 * @brief MCTSのノードの操作
 */
#include <algorithm>

#include "mcts/MCTSNode.hpp"


/**
 * @~english
 * @brief Initialize information for a node.
 * @param[in, out] node MCTS node
 * @param[in] pm1 Previous move coordinate.
 * @param[in] pm2 Move coordinate before previous move.
 * @~japanese
 * @brief ノードの初期化
 * @param[in, out] node MCTSノード
 * @param[in] pm1 1手前の着手箇所
 * @param[in] pm2 2手前の着手箇所
 */
void
InitializeNode( uct_node_t &node, const int pm1, const int pm2 )
{
  node.previous_move1 = pm1;
  node.previous_move2 = pm2;
  node.move_count = 0;
  node.win = 0;
  node.virtual_loss = 0;
  node.width = 0;
  node.child_num = 0;
  std::fill_n(node.seki, BOARD_MAX, false);
  std::fill_n(node.ownership, BOARD_MAX, 0.0);
}


/**
 * @~english
 * @brief Initialize information for a candidate.
 * @param[in, out] child Child node.
 * @param[in, out] child_num The number of child nodes.
 * @param[in] pos Intersection.
 * @param[in] ladder Ladder capturable flag.
 * @~japanese
 * @brief 候補手の初期化
 * @param[in, out] child 子ノード
 * @param[in, out] child_num 子ノードの個数
 * @param[in] pos 着手箇所
 * @param[in] ladder シチョウで逃げられないフラグ
 */
void
InitializeCandidate( child_node_t &child, int &child_num, const int pos, const bool ladder )
{
  child.pos = pos;
  child.move_count = 0;
  child.win = 0;
  child.virtual_loss = 0;
  child.index = NOT_EXPANDED;
  child.rate = 0.0;
  child.pw = false;
  child.open = false;
  child.ladder = ladder;

  child_num++;
}



/**
 * @~english
 * @brief Add virtual loss.
 * @param[in, out] node MCTS node.
 * @param[in, out] child Child node.
 * @~japanese
 * @brief Virtual Lossの加算
 * @param[in, out] node MCTSノード
 * @param[in, out] child 子ノード
 */
void
AddVirtualLoss( uct_node_t &node, child_node_t &child )
{
  node.virtual_loss++;
  child.virtual_loss++;
}


/**
 * @~english
 * @brief Update search result.
 * @param[in, out] node MCTS node.
 * @param[in, out] child Child node.
 * @param[in] result Search result.
 * @~japanese
 * @brief 探索結果の更新
 * @param[in, out] node MCTSノード
 * @param[in, out] child 子ノード
 * @param[in] result 探索結果
 */
void
UpdateResult( uct_node_t &node, child_node_t &child, const int result )
{
  node.win += result;
  node.move_count++;
  node.virtual_loss--;

  child.win += result;
  child.move_count++;
  child.virtual_loss--;
}


/**
 * @~english
 * @brief Reuse root candidates without ladder capturable move (For UCB search).
 * @param[in, out] node MCTS node.
 * @param[in] ladder Ladder capturable flags.
 * @~japanese
 * @brief シチョウで逃げられない手を除いたルートの候補手の再利用
 * @param[in, out] node MCTSノード
 * @param[in] ladder シチョウで逃げられない箇所のフラグ
 */
void
ReuseRootCandidateWithoutLadderMove( uct_node_t &node, const bool ladder[] )
{
  const int child_num = node.child_num;
  child_node_t *child = node.child;

  for (int i = 0; i < child_num; i++) {
    const int pos = child[i].pos;

    child[i].rate = 0.0;
    child[i].pw = false;
    child[i].open = false;
    if (ladder[pos]) {
      node.move_count -= child[i].move_count;
      node.win -= child[i].win;
      child[i].move_count = 0;
      child[i].win = 0;
    }

    child[i].ladder = ladder[pos];
  }
}


/**
 * @~english
 * @brief Calculate winning rate of pass.
 * @param[in] node MCTS node.
 * @return Winning rate of pass.
 * @~japanese
 * @brief パスの勝率の計算
 * @param[in] node MCTSノード
 * @return パスの勝率
 */
double
CalculatePassWinningPercentage( const uct_node_t &node )
{
  const child_node_t &pass_child = node.child[PASS_INDEX];

  if (pass_child.move_count != 0) {
    return static_cast<double>(pass_child.win) / pass_child.move_count;
  } else {
    return 0.0;
  }
}


/**
 * @~english
 * @brief Calculate winning rate.
 * @param[in] child MCTS child node.
 * @return Winning rate of MCTS child node.
 * @~japanese
 * @brief 勝率の計算
 * @param[in] child 子ノード
 * @return 子ノードの勝率
 */
double
CalculateWinningRate( const child_node_t &child )
{
  if (child.move_count > 0) {
    return static_cast<double>(child.win) / child.move_count;
  } else {
    return 0.0;
  }
}


/**
 * @~english
 * @brief Update Monte-Carlo ownership.
 * @param[in, out] node MCTS node.
 * @param[in] game End position data.
 * @param[in] current_color Current player's color.
 * @~japanese
 * @brief モンテカルロ・シミュレーションのOwnershipの更新
 * @param[in, out] node MCTSノード
 * @param[in] game 終局時の局面情報
 * @param[in] current_color 現在の手番の色
 */
void
UpdateOwnership( uct_node_t &node, game_info_t *game, const int current_color )
{
  const char *board = game->board;

  for (int i = 0; i < pure_board_max; i++) {
    const int pos = onboard_pos[i];
    int color = board[pos];

    if (color == S_EMPTY) color = territory[Pat3(game->pat, pos)];

    if (color == current_color) {
      node.ownership[pos] += 1.0;
    } else if (color == S_EMPTY) {
      node.ownership[pos] += 0.5;
    }
  }
}
