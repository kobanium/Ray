/**
 * @file include/board/Point.hpp
 * @author Yuki Kobayashi
 * @~english
 * @brief Coordinate conversion.
 * @~japanese
 * @brief 座標の変換
 */
#ifndef _POINT_HPP_
#define _POINT_HPP_

#include <string>

/**
 * @def GOGUI_X(pos)
 * @brief \~english Get x-axis coordinate with GOGUI expression.
 *        \~japanese GOGUI形式のx座標の取得
 */
#define GOGUI_X(pos) (gogui_x[CORRECT_X(pos)])

/**
 * @def GOGUI_Y(pos)
 * @brief \~english Get y-axis coordinate with GOGUI expression.
 *        \~japanese GOGUI形式のy座標の取得
 */
#define GOGUI_Y(pos) (pure_board_size + 1 - CORRECT_Y(pos))


/**
 * @~english
 * @brief X-axis coordinate charactor in GTP format.
 * @~japanese
 * @brief GTPで用いるX座標の文字
 */
const char gogui_x[] = { 
  'I', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'J', 
  'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 
  'U', 'V', 'W', 'X', 'Y', 'Z' 
};


// GTP形式の座標から1次元の内部表現に変換
int StringToInteger( const char *cpos );

// 1次元の内部表現からGTP形式の座標に変換
void IntegerToString( const int pos, char *cpos );

// 1次元の内部表現からSGF形式の座標に変換
std::string ParsePoint( const int pos );

// 2次元の内部表現からSGF形式の座標に変換
std::string ParseSgfPoint( const int x, const int y );

#endif
