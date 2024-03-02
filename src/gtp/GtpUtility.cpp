#include "board/Color.hpp"
#include "gtp/GtpUtility.hpp"


/**
 * @~english
 * @brief Get color from input string.
 * @param[in] s_color Color string.
 * @return Player's color.
 * @~japanese
 * @brief 入力文字列から手番を取得
 * @param[in] s_color 入力文字列
 * @return 手番の色
 */
int
GetColor( const std::string &s_color )
{
  const char c = static_cast<char>(tolower(static_cast<int>(s_color[0])));

  if (c == 'b') {
    return S_BLACK;
  } else if (c == 'w') {
    return S_WHITE;
  } else {
    return S_EMPTY;
  }
}
