/**
 * @file include/mcts/Rating.hpp
 * @author Yuki Kobayashi
 * @~english
 * @brief Rating calculation for Monte-Carlo simulation
 * @~japanese
 * @brief モンテカルロ・シミュレーションで用いる手のレーティング計算
 */
#ifndef _RATING_HPP_
#define _RATING_HPP_

#include <string>
#include <random>

#include "board/GoBoard.hpp"


/**
 * @~english
 * @brief The number of intersections in MD2 pattern.
 * @~japanese
 * @brief MD2パターンに含まれる交点数
 */
constexpr int UPDATE_NUM = 13;

/**
 * @~english
 * @brief Feature bit mask length.
 * @~japanese
 * @brief 特徴ビットマスクの長さ
 */
const int F_MASK_MAX = 30;

/**
 * @~english
 * @brief Scale parameter for neighbor move.
 * @~japanese
 * @brief 直前の手の周囲の手のボーナス
 */
constexpr double NEIGHBOR_BIAS = 7.52598;

/**
 * @~english
 * @brief Scale parameter for jump move.
 * @~japanese
 * @brief 直前の手のトビの手のボーナス
 */
constexpr double JUMP_BIAS = 4.63207;


//  MD2に収まる座標の計算
void SetNeighbor( void );

//  初期化
void InitializeRating( void );

//  着手(Elo Rating)
int RatingMove( game_info_t *game, int color, std::mt19937_64 &mt );

//  レーティング 
void Rating( game_info_t *game, int color, long long *sum_rate, long long *sum_rate_row, long long *rate );

//  レーティング 
void PartialRating( game_info_t *game, int color, long long *sum_rate, long long *sum_rate_row, long long *rate );

//  現局面の評価値
void AnalyzePoRating( game_info_t *game, int color, double rate[] );

#endif
