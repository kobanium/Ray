/**
 * @file include/board/Record.hpp
 * @author Yuki Kobayashi
 * @~english
 * @brief Definition of move history record.
 * @~japanese
 * @brief 着手履歴データの定義
 */
#ifndef _RECORD_HPP_
#define _RECORD_HPP_

#include "board/Constant.hpp"

/**
 * @struct record_t
 * @~english
 * @brief Move histroy record.
 * @~japanese
 * @brief 着手履歴を記録する構造体
 */
struct record_t {
  /**
   * @~english
   * @brief Stone color.
   * @~japanese
   * @brief 着手した石の色
   */
  int color;

  /**
   * @~english
   * @brief Coordinate.
   * @~japanese
   * @brief 着手した座標
   */
  int pos;

  /**
   * @~english
   * @brief Zobrist hash value.
   * @~japanese
   * @brief 局面のハッシュ値
   */
  unsigned long long hash;
};

#endif
