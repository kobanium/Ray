/**
 * @file MoveSelection.cpp
 * @author Yuki Kobayashi
 * @~english
 * @brief Move selection from search result.
 * @~japanese
 * @brief 着手選択処理
 */
#include <iostream>

#include "common/Message.hpp"
#include "feature/Territory.hpp"
#include "mcts/MoveSelection.hpp"
#include "mcts/UctSearch.hpp"


/**
 * @~english
 * @brief Winning rate threshold for resignation.
 * @~japanese
 * @brief 投了する勝率の閾値
 */
static double resign_threshold = RESIGN_THRESHOLD;

/**
 * @~english
 * @brief Capturing all opponent's dead stones mode.
 * @~japanese
 * @brief 相手の全ての死に石を打ち上げるモード
 */
static bool capture_all_mode = false;


/**
 * @~english
 * @brief Set capturing all opponent's dead stones mode.
 * @param[in] flag Activation flag.
 * @~japanese
 * @brief 相手の全ての死に石を打ち上げるモードの設定
 * @param[in] flag 有効化フラグ
 */
void
SetCaptureAllMode( const bool flag )
{
  capture_all_mode = flag;
}


/**
 * @~english
 * @brief Set Winning rate threshold for resignation.
 * @param[in] threshold Winning rate threshold for resignation.
 * @~japanese
 * @brief 投了する勝率の閾値の設定
 * @param[in] threshold 投了する勝率の閾値
 */
void
SetResignThreshold( const double threshold )
{
  if (threshold <= 0.0) {
    PrintResignThresholdIsTooSmall(threshold);
    resign_threshold = 0.0;
  } else if (threshold > 1.0) {
    PrintResignThresholdIsTooLarge(threshold);
  } else {
    resign_threshold = threshold;
  }
}


/**
 * @~english
 * @brief Select move from max visited move.
 * @param[in] root Root node.
 * @return Next move.
 * @~japanese
 * @brief 探索回数最大の手を選択
 * @param[in] root ルート
 * @return 次の着手
 */
int
SelectMaxVisitChild( const uct_node_t &root )
{
  const child_node_t *child = root.child;
  int select_index = PASS_INDEX;
  int max_count = child[PASS_INDEX].move_count;

  for (int i = 1; i < root.child_num; i++) {
    if (child[i].move_count > max_count) {
      select_index = i;
      max_count = child[i].move_count;
    }
  }

  return select_index;
}


/**
 * @~english
 * @brief Select move from search result.
 * @param[in] game Board position data.
 * @param[in] root Root node.
 * @param[in] color Player's color.
 * @param[out] best_wp Best winning percentage of Monte-Carlo simulation.
 * @return Next move.
 * @~japanese
 * @brief 探索の結果から着手を選択
 * @param[in] game 局面情報
 * @param[in] root ルート
 * @param[in] color 手番の色
 * @param[out] best_wp シミュレーション勝率の最大値
 * @return 次の着手
 */
int
SelectMove( const game_info_t *game, const uct_node_t &root, const int color, double &best_wp )
{
  const child_node_t *child = root.child;
  const int select_index = SelectMaxVisitChild(root);
  const double pass_wp = CalculatePassWinningPercentage(root);
  statistic_t statistic[BOARD_MAX];

  CopyStatistic(statistic);

  best_wp = static_cast<double>(child[select_index].win) / child[select_index].move_count;

  if (best_wp < resign_threshold) {
    return RESIGN;
  } else if (game->moves >= MAX_MOVES) {
    return PASS;
  } else if (capture_all_mode) {
    if (child[select_index].pos == PASS &&
        IsRemainingDeadStone(game, color, statistic)) {
      int index = 1, max_count = child[index].move_count;
      for (int i = 2; i < root.child_num; i++) {
        if (child[i].move_count > max_count) {
          index = i;
          max_count = child[i].move_count;
        }
      }
      return child[index].pos;
    } else {
      return child[select_index].pos;
    }
  } else {
    if (pass_wp >= PASS_THRESHOLD &&
        (game->record[game->moves - 1].pos == PASS)) {
      return PASS;
    } else if (game->moves > 3 &&
               game->record[game->moves - 1].pos == PASS &&
               game->record[game->moves - 3].pos == PASS) {
      return PASS;
    } else {
      return child[select_index].pos;
    }
  }
}
