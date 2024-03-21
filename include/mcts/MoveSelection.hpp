/**
 * @file MoveSelection.hpp
 * @author Yuki Kobayashi
 * @~english
 * @brief Move selection from search result.
 * @~japanese
 * @brief 着手選択
 */
#ifndef _MOVE_SELECTION_HPP_
#define _MOVE_SELECTION_HPP_

#include "board/GoBoard.hpp"
#include "mcts/MCTSNode.hpp"


/**
 * @~english
 * @brief Winning rate threshold for pass validity.
 * @~japanese
 * @brief パスを許可する勝率の閾値
 */
constexpr double PASS_THRESHOLD = 0.90;

/**
 * @~english
 * @brief Monte-Carlo simulation's winning rate threshold for resignation.
 * @~japanese
 * @brief 投了する勝率の閾値(Playout)
 */
constexpr double RESIGN_THRESHOLD = 0.20;


// CGOSモードの設定
void SetCaptureAllMode( const bool flag );

// 投了する閾値の設定
void SetResignThreshold( const double threshold );

// 探索回数最大の子ノードのインデックスを取得
int SelectMaxVisitChild( const uct_node_t &root );

// 探索結果から着手選択
int SelectMove( const game_info_t *game, const uct_node_t &root, const int color, double &best_wp );

#endif
