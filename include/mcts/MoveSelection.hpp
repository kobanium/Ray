#ifndef _MOVE_SELECTION_HPP_
#define _MOVE_SELECTION_HPP_

#include "board/GoBoard.hpp"
#include "mcts/MCTSNode.hpp"


// パスする勝率の閾値
constexpr double PASS_THRESHOLD = 0.90;

// 投了する閾値
constexpr double RESIGN_THRESHOLD = 0.20;


// 投了する閾値の設定
void SetResignThreshold( const double threshold );

// 探索回数最大の子ノードのインデックスを取得
int SelectMaxVisitChild( const uct_node_t &root );

// 探索結果から着手選択
int SelectMove( const game_info_t *game, const uct_node_t &root, double &best_wp );

#endif
