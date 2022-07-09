#ifndef _RATING_HPP_
#define _RATING_HPP_

#include <string>
#include <random>

#include "board/GoBoard.hpp"
#include "UctRating.hpp"


// MD2パターンに入る手の数
const int UPDATE_NUM = 13;

const int F_MASK_MAX = 30;

// Simulation Parameter
const double NEIGHBOR_BIAS = 7.52598;
const double JUMP_BIAS = 4.63207;
const double PO_BIAS = 1.66542;


////////////
//  関数  //
////////////

//  MD2に収まる座標の計算
void SetNeighbor( void );

//  初期化
void InitializeRating( void );

//  着手(Elo Rating)
int RatingMove( game_info_t *game, int color, std::mt19937_64 *mt );

//  レーティング 
void Rating( game_info_t *game, int color, long long *sum_rate, long long *sum_rate_row, long long *rate );

//  レーティング 
void PartialRating( game_info_t *game, int color, long long *sum_rate, long long *sum_rate_row, long long *rate );

//  現局面の評価値
void AnalyzePoRating( game_info_t *game, int color, double rate[] );

#endif
