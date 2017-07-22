#ifndef _SEARCH_BOARD_H_
#define _SEARCH_BOARD_H_

#include "GoBoard.h"

//////////////
//  構造体  //
//////////////
struct search_game_info_t{
  record_t record[MAX_RECORDS];               // 着手箇所と色の記録
  int moves;                                  // 着手数の記録
  int prisoner[S_MAX];                        // アゲハマ
  int ko_pos;                                 // 劫となっている箇所
  int ko_move;                                // 劫となった時の着手数
  char board[BOARD_MAX];                      // 碁盤
  pattern_t pat[BOARD_MAX];                   // 配石パターン
  string_t string[MAX_STRING];                // 連のデータ
  int string_id[STRING_POS_MAX];              // 各座標の連のID
  int string_next[STRING_POS_MAX];            // 連を構成する石のデータ
  bool candidates[BOARD_MAX];                 // 高保守かどうかのフラグ
  int stone[MAX_RECORDS][4][PURE_BOARD_MAX];  // 変化があった連の石の記録
  int stones[MAX_RECORDS][4];                 // 変化があった連の石の個数の記録
  int strings_id[MAX_RECORDS][4];             // 変化があった連のIDの記録
  int strings[MAX_RECORDS];                   // 変化があった連の個数の記録
  char string_color[MAX_RECORDS][4];          // 連の色の記録
  int ko_move_record[MAX_RECORDS];            // 劫が発生した手数の記録
  int ko_pos_record[MAX_RECORDS];             // 劫が発生した箇所の記録
};

////////////
//  関数  //
////////////

// メモリの確保
search_game_info_t* AllocateSearchGame( void );

// メモリの開放
void FreeSearchGame( search_game_info_t *game );

// 盤面情報をコピー(必要な情報のみ)
void CopyGameForSearch( search_game_info_t *dst, const game_info_t *src );

// 石を置く
void PutStoneForSearch( search_game_info_t *game, const int pos, const int color );

// 合法手判定
bool IsLegalForSearch( const search_game_info_t *game, const int pos, const int color );

// テスト
void SearchBoardTest( void );

// 1手戻す
void Undo( search_game_info_t *game );

// 碁盤の出力
void PrintSearchBoard( const search_game_info_t *game ); 

#endif
