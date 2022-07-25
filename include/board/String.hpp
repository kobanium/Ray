#ifndef _STRING_HPP_
#define _STRING_HPP_

#include "board/BoardData.hpp"


// 新しい連の作成
void MakeString( game_info_t *game, const int pos, const int color );

// 連に石を追加する
void AddStone( game_info_t *game, const int pos, const int color, const int id );

// 連同士の結合の判定
void ConnectString( game_info_t *game, const int pos, const int color, const int connection, const int id[] );

// 呼吸点の除去(プレイアウト用)
void PoRemoveLiberty( game_info_t *game, string_t *string, const int pos, const int color );

// 連の除去(プレイアウト用)
int PoRemoveString( game_info_t *game, string_t *string, const int color );

// 呼吸点の除去
void RemoveLiberty( game_info_t *game, string_t *string, const int pos );

// 連の除去
int RemoveString( game_info_t *game, string_t *string, const int color );

#endif
