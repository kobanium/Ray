/**
 * @file src/feature/Territory.cpp
 * @author Yuki Kobayashi
 * @~english
 * @brief Territory checker.
 * @~japanese
 * @brief 領地の判定
 */
#include <algorithm>

#include "feature/Territory.hpp"


// 各交点のOwnershipの計算
static void CalculateOwner( const statistic_t statistic[], const int color, double owner[] );


/**
 * @~english
 * @brief Calculate all ownership values.
 * @param[in] statistic Statistic information for Monte-Carlo simulation.
 * @param[in] color Player's color.
 * @param[out] owner Ownership for each intersections.
 * @~japanese
 * @brief 各交点のOwnerを計算
 * @param[in] statistic モンテカルロ・シミュレーションの統計情報
 * @param[in] color 手番の色
 * @param[out] owner 各交点のOwnerの値
 */
static void
CalculateOwner( const statistic_t statistic[], const int color, double owner[] )
{
  for (int i = 0; i < pure_board_max; i++) {
    const int pos = onboard_pos[i];
    const int sum = statistic[pos].colors[S_BLACK] + statistic[pos].colors[S_WHITE];
    if (sum != 0) {
      owner[pos] = static_cast<double>(statistic[pos].colors[color]) / sum;
    }
  }
}


/**
 * @~english
 * @brief Check remaining dead stones.
 * @param[in] game Board position data.
 * @param[in] color Player's color.
 * @param[in] statistic Statistic information for Monte-Carlo simulation.
 * @return Remaining dead stones flag.
 * @~japanese
 * @brief 死んだ石が残ったか確認
 * @param[in] game 局面情報
 * @param[in] color 手番の色
 * @param[in] statistic モンテカルロ・シミュレーションの統計情報
 * @return 死に石の有無
 */
bool
IsRemainingDeadStone( const game_info_t *game, const int color, const statistic_t statistic[] )
{
  const char *board = game->board;
  const int other = GetOppositeColor(color);
  double owner[BOARD_MAX];

  std::fill_n(owner, BOARD_MAX, 0);

  CalculateOwner(statistic, color, owner);

  for (int i = 0; i < pure_board_max; i++) {
    const int pos = onboard_pos[i];
    if (board[pos] == other && owner[pos] > 0.95) {
      return true;
    }
  }

  return false;
}
