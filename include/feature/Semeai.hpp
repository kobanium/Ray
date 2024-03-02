/**
 * @file include/feature/Semeai.hpp
 * @author Yuki Kobayashi
 * @~english
 * @brief Semeai checker.
 * @~japanese
 * @brief 攻め合いの確認
 */
#ifndef _SEMEAI_HPP_
#define _SEMEAI_HPP_

#include "board/GoBoard.hpp"


/**
 * @enum LIBERTY_STATE
 * @~english
 * @brief Change in the number of liberties.
 * @var L_DECREASE
 * Decrease liberty.
 * @var L_EVEN
 * Unchange liberty.
 * @var L_INCREASE
 * Increase liberty.
 * @~japanese
 * @brief 呼吸点の数の変化
 * @var L_DECREASE
 * 呼吸点が減少
 * @var L_EVEN
 * 呼吸点変化なし
 * @var L_INCREASE
 * 呼吸点が増加
 */
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
bool IsAlreadyCaptured( const game_info_t *game, const int id, int player_id[], int player_ids );

//  自己アタリ
bool IsMeaningfulSelfAtari( const game_info_t *game, const int color, const int pos );

#endif
