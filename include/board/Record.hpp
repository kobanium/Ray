#ifndef _RECORD_HPP_
#define _RECORD_HPP_

#include "board/Constant.hpp"

// 着手を記録する構造体
struct record_t {
  int color;                // 着手した石の色
  int pos;                  // 着手箇所の座標
  unsigned long long hash;  // 局面のハッシュ値
};

#endif
