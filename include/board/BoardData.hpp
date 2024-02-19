/**
 * @file include/board/BoardData.hpp
 * @author Yuki Kobayashi
 * @~english
 * @brief Board position data structure definition.
 * @~japanese
 * @brief 碁盤のデータ構造の定義
 */
#ifndef _BOARD_DATA_HPP_
#define _BOARD_DATA_HPP_

#include "board/Color.hpp"
#include "board/Constant.hpp"
#include "board/Record.hpp"
#include "pattern/Pattern.hpp"


/**
 * @struct string_t
 * @~english
 * @brief Structure of a string (connected stones).
 * @~japanese
 * @brief 連を表現する構造体
 */
struct string_t {
  /**
   * @~english
   * @brief Stone color.
   * @~japanese
   * @brief 連を構成する石の色
   */
  char color;

  /**
   * @~english
   * @brief The number of liberties.
   * @~japanese
   * @brief 連が持つ呼吸点の個数
   */
  int libs;

  /**
   * @~english
   * @brief Coordinates of liberties.
   * @~japanese
   * @brief 連が持つ呼吸点の座標
   */
  short lib[STRING_LIB_MAX];

  /**
   * @~english
   * @brief The number of neighbor strings.
   * @~japanese
   * @brief 隣接する相手の連の個数
   */
  int neighbors;

  /**
   * @~english
   * @brief Neighbor string IDs.
   * @~japanese
   * @brief 隣接する相手の連の連番号
   */
  short neighbor[MAX_NEIGHBOR];

  /**
   * @~english
   * @brief First coordinate of a string.
   * @~japanese
   * @brief 連の始点の座標
   */
  int origin;

  /**
   * @~english
   * @brief The number of stones in a string.
   * @~japanese
   * @brief 連を構成する石の数
   */
  int size;

  /**
   * @~english
   * @brief String existence flag.
   * @~japanese
   * @brief 連の存在フラグ
   */
  bool flag;
};


/**
 * @struct game_info_t
 * @~english
 * @brief Structure of game position.
 * @~japanese
 * @brief 局面を表す構造体
 */
struct game_info_t {
  /**
   * @~english
   * @brief Move history records (coordinates and colors).
   * @~japanese
   * @brief 着手座標と手番の履歴
   */
  record_t record[MAX_RECORDS];

  /**
   * @~english
   * @brief The number of moves.
   * @~japanese
   * @brief 現局面の着手数
   */
  int moves;

  /**
   * @~english
   * @brief The number of captured stones.
   * @~japanese
   * @brief アゲハマの数
   */
  int prisoner[S_MAX];

  /**
   * @~english
   * @brief Cooridnate that cannot be placed a stone by ko.
   * @~japanese
   * @brief 劫で着手できない座標
   */
  int ko_pos;

  /**
   * @~english
   * @brief Move count that ko appeared.
   * @~japanese
   * @brief 劫が発生した時の着手数
   */
  int ko_move;

  /**
   * @~english
   * @brief Zobrist hash value of current position.
   * @~japanese
   * @brief 現在の局面のハッシュ値
   */
  unsigned long long current_hash;

  /**
   * @~english
   * @brief Zobrist hash value of previous position.
   * @~japanese
   * @brief 1手前の局面のハッシュ値
   */
  unsigned long long previous1_hash;

  /**
   * @~english
   * @brief Zobrist hash value of previous position.
   * @~japanese
   * @brief 2手前の局面のハッシュ値
   */
  unsigned long long previous2_hash;

  /**
   * @~english
   * @brief Zobrist hash value of current position (Ignoring ko situation).
   * @~japanese
   * @brief 現在の局面のハッシュ値 (劫は考慮しない)
   */
  unsigned long long positional_hash;

  /**
   * @~english
   * @brief Zobrist hash value of move history.
   * @~japanese
   * @brief 着手の履歴のハッシュ値
   */
  unsigned long long move_hash;

  /**
   * @~english
   * @brief Colors of all coordinates.
   * @~japanese
   * @brief 盤面
   */
  char board[BOARD_MAX];

  /**
   * @~english
   * @brief The number of passes in a move history.
   * @~japanese
   * @brief 今までパスした回数
   */
  int pass_count;

  /**
   * @~english
   * @brief Neighbor stones patterns.
   * @~japanese
   * @brief 周囲の石の配置
   */
  pattern_t pat[BOARD_MAX];

  /**
   * @~english
   * @brief String data.
   * @~japanese
   * @brief 連のデータ
   */
  string_t string[MAX_STRING];

  /**
   * @~english
   * @brief String ID of each coordinates.
   * @~japanese
   * @brief 各座標の連ID
   */
  int string_id[STRING_POS_MAX];

  /**
   * @~english
   * @brief Next string's coordinates of each coordinates.
   * @~japanese
   * @brief 連を構成する石の座標
   */
  int string_next[STRING_POS_MAX];

  /**
   * @~english
   * @brief Legal move flags.
   * @~japanese
   * @brief 候補手のフラグ
   */
  bool candidates[BOARD_MAX];

  /**
   * @~english
   * @brief Seki flag.
   * @~japanese
   * @brief セキのフラグ
   */
  bool seki[BOARD_MAX];

  /**
   * @~english
   * @brief Tactical features data for Monte-Carlo simulation.
   * @~japanese
   * @brief 戦術的特徴 (モンテカルロ・シミュレーション用)
   */
  unsigned char tactical_features[BOARD_MAX * 6];

  /**
   * @~english
   * @brief The number of captured stones by previous move.
   * @~japanese
   * @brief 前の着手で打ち上げた石の個数
   */
  int capture_num[S_OB];

  /**
   * @~english
   * @brief Coordinates of captured stone by previous move.
   * @~japanese
   * @brief 前の着手で石を打ち上げた座標
   */
  int capture_pos[S_OB][PURE_BOARD_MAX];

  /**
   * @~english
   * @brief The number of coordinates that need to be updated for tactical features.
   * @~japanese
   * @brief 戦術的特徴が更新された座標の個数
   */
  int update_num[S_OB];

  /**
   * @~english
   * @brief Coordinates that need to be updated for tactical features.
   * @~japanese
   * @brief 戦術的特徴が更新された座標
   */
  int update_pos[S_OB][PURE_BOARD_MAX];

  /**
   * @~english
   * @brief Weights of each coordinate for Monte-Carlo simulation.
   * @~japanese
   * @brief シミュレーション時の各座標のレート
   */
  long long rate[2][BOARD_MAX];

  /**
   * @~english
   * @brief Sum of weights of each row for Monte-Carlo simulation.
   * @~japanese
   * @brief シミュレーション時の各列のレートの合計値
   */
  long long sum_rate_row[2][BOARD_SIZE];

  /**
   * @~english
   * @brief Sum weight of all coordinates for Monte-Carlo simulation.
   * @~japanese
   * @brief シミュレーション時の全体のレートの合計値
   */
  long long sum_rate[2];
};

#endif
