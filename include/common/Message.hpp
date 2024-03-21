/**
 * @file include/common/Message.hpp
 * @author Yuki Kobayashi
 * @~english
 * @brief Console message.
 * @~japanese
 * @brief コンソールメッセージ出力
 */
#ifndef _MESSAGE_HPP_
#define _MESSAGE_HPP_

#include "board/GoBoard.hpp"
#include "mcts/UctSearch.hpp"


//  エラーメッセージの出力の設定
void SetDebugMessageMode( const bool flag );

//  盤面の表示
void PrintBoard( const game_info_t *game );

//  連の情報の表示              
//    呼吸点の数, 座標          
//    連を構成する石の数, 座標  
//    隣接する敵の連のID
void PrintString( const game_info_t *game );

//  各座標の連IDの表示  
void PrintStringID( const game_info_t *game );

//  連リストの繋がりを表示(Debug用)
void PrintStringNext( const game_info_t *game );

//  オーナーの表示
void PrintOwner( const uct_node_t *root, const statistic_t *statistic, const int color, const int count, double *own );

//  最善応手列の表示
void PrintBestSequence( const game_info_t *game, const uct_node_t *uct_node, const int root, const int start_color );

//  探索の情報の表示
void PrintPlayoutInformation( const uct_node_t *root, const int po_speed, const double finish_time, const int pre_simulated );

//  座標の出力
void PrintPoint( const int pos );

//  コミの値の出力
void PrintKomiValue( void );

//  Ponderingのプレイアウト回数の出力
void PrintPonderingCount( const int count );

//  探索時間の出力
void PrintPlayoutLimits( const double time_limit, const int playout_limit );

//  再利用した探索回数の出力
void PrintReuseCount( const int count );

void PrintResignThresholdIsTooLarge( const double threshold );

void PrintResignThresholdIsTooSmall( const double threshold );

void PrintLeelaZeroAnalyze( const uct_node_t *root );

void PrintRate( const game_info_t *game, const int color );

#endif
