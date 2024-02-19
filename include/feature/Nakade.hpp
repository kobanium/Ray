/**
 * @file Nakade.hpp
 * @author Yuki Kobayashi
 * @~english
 * @brief Nakade checker.
 * @~japanese
 * @brief ナカデの確認
 */
#ifndef _NAKADE_HPP_
#define _NAKADE_HPP_

#include "board/GoBoard.hpp"

/**
 * @~english
 * @brief Not nakade value.
 * @~japanese
 * @brief ナカデではない時の値
 */
constexpr int NOT_NAKADE = -1;

/**
 * @~english
 * @brief Maximum data size for nakade queue.
 * @~japanese
 * @brief ナカデを確認するキューの最大データ長
 */
constexpr int NAKADE_QUEUE_SIZE = 30;


/**
 * @struct nakade_queue_t
 * @~english
 * @brief Queue for nakade check.
 * @~japanese
 * @brief ナカデを確認するためのキュー
 */
struct nakade_queue_t {
  /**
   * @~english
   * @brief Coordinate.
   * @~japanese
   * @brief 座標
   */
  int pos[NAKADE_QUEUE_SIZE];

  /**
   * @~english
   * @brief Index of head element.
   * @~japanese
   * @brief 先頭のインデックス
   */
  int head;

  /**
   * @~english
   * @brief Index of tail element.
   * @~japanese
   * @brief 末尾のインデックス
   */
  int tail;
};


// ナカデのハッシュの初期設定
void InitializeNakadeHash( void );

// 自己アタリがナカデの形になっているか(シミュレーション用)
bool IsNakadeSelfAtari( const game_info_t *game, const int pos, const int color );

// 自己アタリがナカデの形になっているか(UCT用)
bool IsUctNakadeSelfAtari( const game_info_t *game, const int pos, const int color );

// 直前の手で囲ったエリアにナカデの急所があるかを確認
// ナカデの急所があれば, その座標を
// なければ, -1を返す
void SearchNakade( const game_info_t *game, int *nakade_num, int *nakade_pos );

// 直前の手で取られた石がナカデの形になっているかを確認
// ナカデの急所があれば, その座標を返す
// なければ, -1を返す
int CheckRemovedStoneNakade( const game_info_t *game, const int color );

#endif
