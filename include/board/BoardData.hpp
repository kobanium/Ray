#ifndef _BOARD_DATA_HPP_
#define _BOARD_DATA_HPP_

#include "board/Color.hpp"
#include "board/Constant.hpp"
#include "board/Record.hpp"
#include "pattern/Pattern.hpp"


// 連を表す構造体
struct string_t {
  char color;                    // 連の色
  int libs;                      // 連の持つ呼吸点数
  short lib[STRING_LIB_MAX];     // 連の持つ呼吸点の座標
  int neighbors;                 // 隣接する敵の連の数
  short neighbor[MAX_NEIGHBOR];  // 隣接する敵の連の連番号
  int origin;                    // 連の始点の座標
  int size;                      // 連を構成する石の数
  bool flag;                     // 連の存在フラグ
};


// 局面を表す構造体
struct game_info_t {
  record_t record[MAX_RECORDS];  // 着手箇所と色の記録
  int moves;                        // 着手数の記録
  int prisoner[S_MAX];              // アゲハマ
  int ko_pos;                       // 劫となっている箇所
  int ko_move;                      // 劫となった時の着手数

  unsigned long long current_hash;     // 現在の局面のハッシュ値
  unsigned long long previous1_hash;   // 1手前の局面のハッシュ値
  unsigned long long previous2_hash;   // 2手前の局面のハッシュ値
  unsigned long long positional_hash;  // 局面のハッシュ値(石の位置のみ)
  unsigned long long move_hash;        // 着手列のハッシュ値(合流なし)

  char board[BOARD_MAX];            // 盤面

  int pass_count;                   // パスした回数

  pattern_t pat[BOARD_MAX];    // 周囲の石の配置

  string_t string[MAX_STRING];        // 連のデータ(19x19 : 573,845bytes)
  int string_id[STRING_POS_MAX];    // 各座標の連のID
  int string_next[STRING_POS_MAX];  // 連を構成する石のデータ構造

  bool candidates[BOARD_MAX];  // 候補手かどうかのフラグ
  bool seki[BOARD_MAX];

  unsigned char tactical_features[BOARD_MAX * 6]; // シミュレーション用の戦術的特徴

  int capture_num[S_OB];                   // 前の着手で打ち上げた石の数
  int capture_pos[S_OB][PURE_BOARD_MAX];   // 前の着手で石を打ち上げた座標

  int update_num[S_OB];                    // 戦術的特徴が更新された数
  int update_pos[S_OB][PURE_BOARD_MAX];    // 戦術的特徴が更新された座標

  long long rate[2][BOARD_MAX];           // シミュレーション時の各座標のレート
  long long sum_rate_row[2][BOARD_SIZE];  // シミュレーション時の各列のレートの合計値
  long long sum_rate[2];                  // シミュレーション時の全体のレートの合計値
};

#endif
