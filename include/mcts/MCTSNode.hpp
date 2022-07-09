#ifndef _MCTS_NODE_HPP_
#define _MCTS_NODE_HPP_

#include "board/Constant.hpp"
#include "board/GoBoard.hpp"

#include <atomic>
#include <mutex>



// 探索手の最大数(盤上全体 + パス)
constexpr int UCT_CHILD_MAX = PURE_BOARD_MAX + 1;

// 未展開のノードのインデックス
constexpr int NOT_EXPANDED = -1;

// パスの手のインデックス
constexpr int PASS_INDEX = 0;

// Virtual Loss
constexpr int VIRTUAL_LOSS = 1;


struct child_node_t {
  short pos;                    // 着手する座標
  std::atomic<int> move_count;  // 探索回数
  std::atomic<int> win;         // 勝った回数
  int index;                    // 次のノードのインデックス
  float rate;                   // 着手の評価値
  unsigned char mask;           // マスク
  bool pw;                      // Progressive Wideningのフラグ
  bool open;                    // 常に探索候補に入れるかどうかのフラグ
  bool ladder;                  // シチョウのフラグ
};


struct uct_node_t {
  int previous_move1;                 // 1手前の着手
  int previous_move2;                 // 2手前の着手
  std::atomic<int> move_count;        // 探索回数
  std::atomic<int> win;               // 勝った回数
  int width;                          // 探索幅
  int child_num;                      // 子ノードの個数
  child_node_t child[UCT_CHILD_MAX];  // 子ノードの情報
  bool seki[BOARD_MAX];               // セキの判定
};



void InitializeCandidate( child_node_t &child, int &child_num, const int pos, const bool ladder );

void AddVirtualLoss( uct_node_t &node, child_node_t &child );

void UpdateResult( uct_node_t &node, child_node_t &child, const int result );

void ReuseRootCandidateWithoutLadderMove( uct_node_t &node, const bool ladder[] );

void InitializeNode( uct_node_t &node, const int pm1, const int pm2 );

#endif
