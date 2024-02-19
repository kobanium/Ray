/**
 * @file src/board/DynamicKomi.cpp
 * @author Yuki Kobayashi
 * @~english
 * @brief Dynamic komi.
 * @~japanese
 * @brief ダイナミックコミ
 */
#include <iomanip>
#include <iostream>

#include "board/DynamicKomi.hpp"
#include "board/GoBoard.hpp"
#include "common/Message.hpp"


/**
 * @~english
 * @brief The number of handicap stones.
 * @~japanese
 * @brief 置き石の個数
 */
static int handicap_num = 0;

/**
 * @~english
 * @brief The number of handicap stones for testing.
 * @~japanese
 * @brief 置き石の個数 (テスト用)
 */
static int const_handicap_num = 0;

/**
 * @~english
 * @brief Dynamic komi mode.
 * @~japanese
 * @brief ダイナミックコミのモード
 */
static enum DYNAMIC_KOMI_MODE dk_mode = DK_OFF;


// コミの値を直線的に減らすDynamic Komi
static void LinearHandicap( const game_info_t *game );

// 勝率に基づいてコミの値を変更するDynamic Komi
static void ValueSituational( const uct_node_t *root, const int color );


/**
 * @~english
 * @brief Set the number of handicap stones for testing.
 * @param[in] num The number of handicap stones.
 * @~japanese
 * @brief テスト用の置き石の個数の設定
 * @param[in] num 置き石の個数
 */
void
SetConstHandicapNum( const int num )
{
  const_handicap_num = num;
}


/**
 * @~english
 * @brief Set the number of handicap stones.
 * @param[in] num The number of handicap stones.
 * @~japanese
 * @brief 置き石の個数の設定
 * @param[in] num 置き石の個数
 */
void
SetHandicapNum( const int num )
{
  if (const_handicap_num == 0) {
    handicap_num = num;
    if (dk_mode != DK_OFF && handicap_num == 0) {
      dk_mode = DK_OFF;
    } else if (dk_mode == DK_OFF &&  handicap_num != 0) {
      dk_mode = DK_LINEAR;
    } 
  } else {
    handicap_num = const_handicap_num;
    dk_mode = DK_LINEAR;
  }
}


/**
 * @~english
 * @brief Update dynamic komi.
 * @param[in] game Board position data.
 * @param[in] root MCTS root node.
 * @param[in] color Player's color.
 * @~japanese
 * @brief ダイナミックコミの更新
 * @param[in] game 局面情報
 * @param[in] root ルートノード
 * @param[in] color 手番の色
 */
void
DynamicKomi( const game_info_t *game, const uct_node_t *root, const int color )
{
  if (handicap_num != 0) {
    switch(dk_mode) {
      case DK_LINEAR:
        LinearHandicap(game);
        break;
      case DK_VALUE:
        ValueSituational(root, color);
        break;
      default:
        break;
    }
  }
}


/**
 * @~english
 * @brief Update dynamic komi with linear decreasing.
 * @param[in] game Board position data.
 * @~japanese
 * @brief 線形で減少するダイナミックコミの更新
 * @param[in] game 局面情報
 */
static void
LinearHandicap( const game_info_t *game )
{
  if (game->moves > LINEAR_THRESHOLD - 15) {
  // 手数が進んだらコミを変動しない
    dynamic_komi[0] = static_cast<double>(handicap_num) + 0.5;
  } else {
    // 新しいコミの値の計算
    dynamic_komi[0] = HANDICAP_WEIGHT * handicap_num * (1.0 - (static_cast<double>(game->moves) / LINEAR_THRESHOLD));
  }
  // 新しいコミの値を代入
  dynamic_komi[S_BLACK] = dynamic_komi[0] + 1;
  dynamic_komi[S_WHITE] = dynamic_komi[0] - 1;

  PrintKomiValue();
}


/**
 * @~english
 * @brief Update dynamic komi considering winning ratio on a root node.
 * @param[in] root MCTS root node.
 * @param[in] color Player's color.
 * @~japanese
 * @brief ルードノードでの勝率を考慮したダイナミックコミの更新
 * @param[in] root ルートノード
 * @param[in] color 手番の色
 */
static void
ValueSituational( const uct_node_t *root, const int color )
{
  const double win_rate = static_cast<double>(root->win) / root->move_count;

  // 次の探索の時のコミを求める
  if (color == S_BLACK) {
    if (win_rate < RED) {
      dynamic_komi[0]--;
    } else if (win_rate > GREEN) {
      dynamic_komi[0]++;
    }
  } else if (color == S_WHITE) {
    if (win_rate < RED) {
      dynamic_komi[0]++;
    } else if (win_rate > GREEN) {
      dynamic_komi[0]--;
    }
  }

  dynamic_komi[S_BLACK] = dynamic_komi[0] + 1.0;
  dynamic_komi[S_WHITE] = dynamic_komi[0] - 1.0;

  PrintKomiValue();
}
