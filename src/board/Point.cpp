/**
 * @file src/board/Point.cpp
 * @author Yuki Kobayashi
 * @~english
 * @brief Converter of coordinate.
 * @~japanese
 * @brief 座標の変換
 */
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cctype>
#include <iostream>

#include "board/GoBoard.hpp"
#include "board/Point.hpp"


/**
 * @~english
 * @brief Pass string
 * @~japanese
 * @brief パスを表現する文字列
 */
constexpr char pass[] = "pass";

/**
 * @~english
 * @brief Resignation.
 * @~japanese
 * @brief 投了を表現する文字列
 */
constexpr char resign[] = "resign";


/**
 * @~english
 * @brief Return coordinate with internal representation.
 * @param[in] cpos Coordinate.
 * @return Coordinate with internal representation.
 * @~japanese
 * @brief 内部表現の座標を返す
 * @param[in] cpos 座標
 * @return 内部表現の座標
 */
int
StringToInteger( const char *cpos )
{
  char alphabet;
  int x, y, pos;

  if (strcmp(cpos, "pass") == 0 || 
      strcmp(cpos, "PASS") == 0){
    pos = PASS;
  } else {
    alphabet = (char)toupper(cpos[0]);
    x = 0;
    for (int i = 1; i <= pure_board_size; i++) {
      if (gogui_x[i] == alphabet) {
        x = i;
      }
    }
    y = pure_board_size - atoi(&cpos[1]) + 1;
    pos = POS(x + (OB_SIZE - 1), y + (OB_SIZE - 1));
  }

  return pos;
}


/**
 * @~english
 * @brief Return coordinate in GTP format.
 * @param[in] pos Coordinate.
 * @param[out] cpos Coordinate string in GTP representation.
 * @~japanese
 * @brief 座標の文字列を返す
 * @param[in] pos 座標
 * @param[out] cpos GTP形式の座標の文字列
 */
void
IntegerToString( const int pos, char *cpos )
{
  int x, y;

  if (pos == PASS) {
#if defined (_WIN32)
    sprintf_s(cpos, 5, "%s", pass);
#else
    snprintf(cpos, 5, "%s", pass);
#endif
  } else if (pos == RESIGN) {
#if defined (_WIN32)
    sprintf_s(cpos, 7, "%s", resign);
#else
    snprintf(cpos, 7, "%s", resign);
#endif
  } else {
    x = X(pos) - (OB_SIZE - 1);
    y = pure_board_size - (Y(pos) - OB_SIZE);
    cpos[0] = gogui_x[x];
    if (y / 10 == 0) {
      cpos[1] = static_cast<char>('0' + y % 10);
      cpos[2] = '\0';
    } else {
      cpos[1] = static_cast<char>('0' + y / 10);
      cpos[2] = static_cast<char>('0' + y % 10);
      cpos[3] = '\0';
    }
  }
}


/**
 * @~english
 * @brief Return coordinate string.
 * @param[in] pos Coordinate.
 * @return Coordinate string.
 * @~japanese
 * @brief 座標の文字列を返す
 * @param[in] pos 座標
 * @return 座標の文字列
 */
std::string
ParsePoint( const int pos )
{
  if (pos == PASS) {
    return std::string(pass);
  } else if (pos == RESIGN) {
    return std::string(resign);
  } else {
    return std::string(1, GOGUI_X(pos)) + std::to_string(GOGUI_Y(pos));
  }
}


/**
 * @~english
 * @brief Return coordinate string with SGF.
 * @param[in] x X-axis coordinate.
 * @param[in] y Y-axis coordinate.
 * @return Coordinate string with SGF.
 * @~japanese
 * @brief SGF形式の座標の文字列を返す
 * @param[in] x x座標
 * @param[in] y y座標
 * @return SGF形式の座標の文字列
 */
std::string
ParseSgfPoint( const int x, const int y )
{
  std::string sgf_pos;

  if ((x ==  0 && y ==  0) ||
      (x == -1 && y == -1)) {
    sgf_pos = std::string("tt");
  } else {
    sgf_pos  = static_cast<char>(x - 1 + 'a');
    sgf_pos += static_cast<char>(y - 1 + 'a');
  }

  return sgf_pos;
}
