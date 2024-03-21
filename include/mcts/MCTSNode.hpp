/**
 * @file include/mcts/MCTSNode.hpp
 * @author Yuki Kobayashi
 * @~english
 * @brief Node definition for Monte-Carlo Tree Search
 * @~japanese
 * @brief モンテカルロ木探索のノードの定義
 */
#ifndef _MCTS_NODE_HPP_
#define _MCTS_NODE_HPP_

#include "board/Constant.hpp"
#include "board/GoBoard.hpp"

#include <atomic>
#include <mutex>


/**
 * @~english
 * @brief The maximum number of move candidates.
 * @~japanese
 * @brief 候補手の最大数(盤上全体 + パス)
 */
constexpr int UCT_CHILD_MAX = PURE_BOARD_MAX + 1;

/**
 * @~english
 * @brief Index for unexpanded node.
 * @~japanese
 * @brief 未展開ノードのインデックス
 */
constexpr int NOT_EXPANDED = -1;

/**
 * @~english
 * @brief Index for pass.
 * @~japanese
 * @brief パスのインデックス
 */
constexpr int PASS_INDEX = 0;

/**
 * @~english
 * @brief Virtual loss.
 * @~japanese
 * @brief Virtual Loss
 */
constexpr int VIRTUAL_LOSS = 1;

/**
 * @~english
 * @brief Vritual loss weight.
 * @~japanese
 * @brief Virtual Lossの重み
 */
constexpr double VIRTUAL_LOSS_WEIGHT = 1.0;

/**
 * @struct child_node_t
 * @~english
 * @brief Child node data.
 * @~japanese
 * @brief 子ノードの情報
 */
struct child_node_t {
  /**
   * @~english
   * @brief Move coordinate.
   * @~japanese
   * @brief 着手する座標
   */
  short pos;

  /**
   * @~english
   * @brief Search count.
   * @~japanese
   * @brief 探索回数
   */
  std::atomic<int> move_count;

  /**
   * @~english
   * @brief Vritual loss counter.
   * @~japanese
   * @brief Virtual Lossのカウンタ
   */
  std::atomic<int> virtual_loss;

  /**
   * @~english
   * @brief The number of winning situations by Monte-Carlo simulation.
   * @~japanese
   * @brief モンテカルロ・シミュレーションの勝った回数
   */
  std::atomic<int> win;

  /**
   * @~english
   * @brief Index to child node.
   * @~japanese
   * @brief 子ノードのインデックス
   */
  int index;

  /**
   * @~english
   * @brief Move score (or Policy).
   * @~japanese
   * @brief 着手のレート
   */
  float rate;

  /**
   * @~english
   * @brief Progressive widening opening flag.
   * @~japanese
   * @brief Progressive Wideningで展開されたフラグ
   */
  bool pw;

  /**
   * @~english
   * @brief Forced opening flag.
   * @~japanese
   * @brief 強制的に手を読むフラグ
   */
  bool open;

  /**
   * @~english
   * @brief Flag to be captured by ladder.
   * @~japanese
   * @brief シチョウで逃げる手のフラグ
   */
  bool ladder;
};


/**
 * @struct uct_node_t
 * @~english
 * @brief UCT node data.
 * @~japanese
 * @brief UCTノードデータ
 */
struct uct_node_t {
  /**
   * @~english
   * @brief Previous move.
   * @~japanese
   * @brief 1手前の着手箇所
   */
  int previous_move1;

  /**
   * @~english
   * @brief Move before previous move.
   * @~japanese
   * @brief 2手前の着手箇所
   */
  int previous_move2;

  /**
   * @~english
   * @brief Search count.
   * @~japanese
   * @brief ノードの探索回数
   */
  std::atomic<int> move_count;

  /**
   * @~english
   * @brief The number of winning situations by Monte-Carlo simulation.
   * @~japanese
   * @brief モンテカルロ・シミュレーションの勝った回数
   */
  std::atomic<int> win;

  /**
   * @~english
   * @brief Virtual loss counter.
   * @~japanese
   * @brief Virtual Lossのカウンタ
   */
  std::atomic<int> virtual_loss;

  /**
   * @~english
   * @brief The number of search candidates.
   * @~japanese
   * @brief 探索する手の幅
   */
  int width;

  /**
   * @~english
   * @brief The number of child nodes.
   * @~japanese
   * @brief 子ノードの個数
   */
  int child_num;

  /**
   * @~english
   * @brief Children node information.
   * @~japanese
   * @brief 子ノードの情報
   */
  child_node_t child[UCT_CHILD_MAX];

  /**
   * @~english
   * @brief Seki point.
   * @~japanese
   * @brief セキの評価結果
   */
  bool seki[BOARD_MAX];

  /**
   * @~english
   * @brief Ownership of Monte-Carlo simulation.
   * @~japanese
   * @brief Ownership
   */
  double ownership[BOARD_MAX];
};



void InitializeCandidate( child_node_t &child, int &child_num, const int pos, const bool ladder );

void AddVirtualLoss( uct_node_t &node, child_node_t &child );

void UpdateResult( uct_node_t &node, child_node_t &child, const int result );

void ReuseRootCandidateWithoutLadderMove( uct_node_t &node, const bool ladder[] );

void InitializeNode( uct_node_t &node, const int pm1, const int pm2 );

double CalculatePassWinningPercentage( const uct_node_t &node );

double CalculateWinningRate( const child_node_t &child );

void UpdateOwnership( uct_node_t &node, game_info_t *game, const int current_color );

#endif
