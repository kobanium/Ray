#ifndef _SEMEAI_H_
#define _SEMEAI_H_

#include "GoBoard.h"

enum LIBERTY_STATE {
  L_DECREASE,
  L_EVEN,
  L_INCREASE,
};


//  1手で取れるアタリの判定
bool IsCapturableAtari( const game_info_t *game, const int pos, const int color, const int opponent_pos );

//  オイオトシの確認
int CheckOiotoshi( const game_info_t *game, const int pos, const int color, const int opponent_pos );

//  ウッテガエシ用の判定
int CapturableCandidate( const game_info_t *game, const int id );

//  すぐに捕まる手かどうかを判定  
bool IsDeadlyExtension( const game_info_t *game, const int color, const int id );

//  呼吸点がどのように変化するかを確認
int CheckLibertyState( const game_info_t *game, const int pos, const int color, const int id );

//  自己アタリになるトリかどうか判定
bool IsSelfAtariCapture( const game_info_t *game, const int pos, const int color, const int id );

//  1手で取れるアタリ(シミュレーション用)
bool IsCapturableAtariForSimulation( const game_info_t *game, const int pos, const int color, const int id );

//  自己アタリになるトリかどうか判定
bool IsSelfAtariCaptureForSimulation( const game_info_t *game, const int pos, const int color, const int lib );

//  自己アタリになるかどうかの判定
bool IsSelfAtari( const game_info_t *game, const int color, const int pos );

//  欠け眼を継ぐかどうかの判定の準備
bool IsAlreadyCaptured( const game_info_t *game, const int color, const int id, int player_id[], int player_ids );

#endif
