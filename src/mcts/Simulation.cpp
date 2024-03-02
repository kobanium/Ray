/**
 * @file Simulation.cpp
 * @author Yuki Kobayashi
 * @~english
 * @brief Monte Carlo simulation.
 * @~japanese
 * @brief モンテカルロシミュレーション
 */
#include <random>

#include "board/GoBoard.hpp"
#include "board/Point.hpp"
#include "common/Message.hpp"
#include "mcts/Rating.hpp"
#include "mcts/Simulation.hpp"


/**
 * @~english
 * @brief Simulate a match with Monte Carlo method.
 * @param[in] game Board situation.
 * @param[in] starting_color First playing color.
 * @param[in] mt Random number generator.
 * @~japanese
 * @brief モンテカルロ法による対局のシミュレーション
 * @param[in] game 局面の情報
 * @param[in] starting_color シミュレーション開始手番
 * @param[in] mt 乱数生成器
 */
void
Simulation( game_info_t *game, int starting_color, std::mt19937_64 &mt )
{
  int color = starting_color, pos = -1;
  int pass_count = 0;

  // シミュレーション打ち切り手数を設定
  int length = MAX_MOVES - game->moves;

  if (length < 0) {
    return;
  }

  // レートの初期化  
  std::fill_n(game->sum_rate, 2, 0);
  std::fill(game->sum_rate_row[0], game->sum_rate_row[2], 0);
  std::fill(game->rate[0], game->rate[2], 0);

  // 黒番のレートの計算
  Rating(game, S_BLACK, &game->sum_rate[0], game->sum_rate_row[0], game->rate[0]);
  // 白番のレートの計算
  Rating(game, S_WHITE, &game->sum_rate[1], game->sum_rate_row[1], game->rate[1]);

  // 終局まで対局をシミュレート
  while (length-- && pass_count < 2) {
    // 着手を生成する
    pos = RatingMove(game, color, mt);
    // 石を置く
    PoPutStone(game, pos, color);
    // パスの確認
    pass_count = (pos == PASS) ? (pass_count + 1) : 0;
    // 手番の入れ替え
    color = GetOppositeColor(color);
  }
}
