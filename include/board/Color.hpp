/**
 * @file include/board/Color.hpp
 * @author Yuki Kobayashi
 * @~english
 * @brief Definition of intersection's status.
 * @~japanese
 * @brief 交点の状態の定義と処理
 */
#ifndef _COLOR_HPP_
#define _COLOR_HPP_

/**
 * @enum stone
 * @~english
 * @brief Status of intersection on a go board.
 * @var S_EMPTY
 * Empty intersection.
 * @var S_BLACK
 * Black stone.
 * @var S_WHITE
 * White stone.
 * @var S_OB
 * Out of a go board.
 * @var S_MAX
 * Sentinel status.
 * @~japanese
 * @brief 交点の状態
 * @var S_EMPTY
 * 空点
 * @var S_BLACK
 * 黒石
 * @var S_WHITE
 * 白石
 * @var S_OB
 * 盤外
 * @var S_MAX
 * 番兵
 */
enum stone {
  S_EMPTY,
  S_BLACK,
  S_WHITE,
  S_OB,
  S_MAX,
};


/**
 * @~english
 * @brief Get opponent's color. This is only valid for S_BLACK and S_WHITE.
 * @param[in] color Player's color.
 * @return Opponent's color.
 * @~japanese
 * @brief 色を反転して返す
 * @param[in] color 手番の色
 * @return 相手の手番の色
 */
inline int
GetOppositeColor( const int color )
{
  return (color ^ 0x3);
}

#endif
