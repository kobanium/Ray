#ifndef _COLOR_HPP_
#define _COLOR_HPP_

enum stone {
  S_EMPTY,  // 空点
  S_BLACK,  // 黒石
  S_WHITE,  // 白石
  S_OB,     // 盤外
  S_MAX,    // 番兵
};


// 色の反転
inline int
GetOppositeColor( const int color )
{
  return (color ^ 0x3);
}

#endif
