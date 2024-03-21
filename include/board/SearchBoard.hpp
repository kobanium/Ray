/**
 * @file include/board/SearchBoard.hpp
 * @author Yuki Kobayashi
 * @~english
 * @brief Board position operation for search.
 * @~japanese
 * @brief 探索用の対局情報の管理.
 */
#ifndef _SEARCH_BOARD_HPP_
#define _SEARCH_BOARD_HPP_

#include "board/GoBoard.hpp"


/**
 * @struct undo_record_t
 * @~english
 * @brief Move history record for rollback operation.
 * @~japanese
 * @brief Undo処理のための履歴
 */
struct undo_record_t {
  /**
   * @~english
   * @brief Stones records for rollback operation.
   * @~japanese
   * @brief 変化のあった連の石の記録
   */
  int stone[4][PURE_BOARD_MAX];

  /**
   * @~english
   * @brief Records of the number of stones for rollback operation.
   * @~japanese
   * @brief 変化があった連の石の個数の記録
   */
  int stones[4];

  /**
   * @~english
   * @brief String ID records for rollback operation.
   * @~japanese
   * @brief 変化があった連のIDの記録
   */
  int strings_id[4];

  /**
   * @~english
   * @brief String records for rollback operation.
   * @~japanese
   * @brief 変化があった連の個数の記録
   */
  int strings;

  /**
   * @~english
   * @brief  String's color record for rollback operation.
   * @~japanese
   * @brief 連の色の記録
   */
  char string_color[4];

  /**
   * @~english
   * @brief Move count record of ko occurance for rollback operation..
   * @~japanese
   * @brief 劫が発生した手数の記録
   */
  int ko_move_record;

  /**
   * @~english
   * @brief Record of ko occurance for roll back operation..
   * @~japanese
   * @brief 劫が発生した箇所の記録
   */
  int ko_pos_record;
};

/**
 * @struct search_game_info_t
 * @~english
 * @brief Game position for fast search.
 * @~japanese
 * @brief 探索用の局面情報
 */
struct search_game_info_t {
  explicit search_game_info_t( const game_info_t *src );

  /**
   * @~english
   * @brief Move history record.
   * @~japanese
   * @brief 着手箇所と手番の記録
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
   * @brief アゲハマの個数
   */
  int prisoner[S_MAX];

  /**
   * @~english
   * @brief Coordinate that cannot be placed a stone by Ko.
   * @~japanese
   * @brief 劫で着手できない箇所
   */
  int ko_pos;

  /**
   * @~english
   * @brief Move count that ko appeared
   * @~japanese
   * @brief 劫となった時の着手数
   */
  int ko_move;

  /**
   * @~english
   * @brief Go board.
   * @~japanese
   * @brief 盤面
   */
  char board[BOARD_MAX];

  /**
   * @~english
   * @brief Stone patterns.
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
   * @brief String ID of each coordinate.
   * @~japanese
   * @brief 各座標の連のID
   */
  int string_id[STRING_POS_MAX];

  /**
   * @~english
   * @brief Next string's coordinate of each coordinate.
   * @~japanese
   * @brief 連を構成する石のデータ
   */
  int string_next[STRING_POS_MAX];

  /**
   * @~english
   * @brief Legal move flag
   * @~japanese
   * @brief 候補手フラグ
   */
  bool candidates[BOARD_MAX];

  /**
   * @~english
   * @brief Move history record for undo operation.
   * @~japanese
   * @brief Undo操作のための着手履歴
   */
  undo_record_t undo[MAX_RECORDS];
};


// 石を置く
void PutStoneForSearch( search_game_info_t *game, const int pos, const int color );

// 合法手判定
bool IsLegalForSearch( const search_game_info_t *game, const int pos, const int color );

// 1手戻す
void Undo( search_game_info_t *game );

#endif
