#ifndef _UCTRATING_HPP_
#define _UCTRATING_HPP_

#include <string>

#include "board/GoBoard.hpp"
#include "feature/UctFeature.hpp"
#include "pattern/PatternHash.hpp"


constexpr int BTFM_DIMENSION = 5;

constexpr int LARGE_PAT_MAX = 1000000;

// Ownerは
// 0-5% 6-15% 16-25% 26-35% 36-45% 46-55%
// 56-65% 66-75% 76-85% 86-95% 96-100%
// の11区分
constexpr int OWNER_MAX = 11;
constexpr int CRITICALITY_MAX = 25;

constexpr double CRITICALITY_TERM = 100.0;
constexpr double CRITICALITY_INIT = 0.765745;
constexpr double CRITICALITY_BIAS = 0.32;

constexpr double OWNER_K = 6.6;
constexpr double OWNER_BIAS = 1.8;


struct fm_t {
  double w;
  double v[BTFM_DIMENSION];
};

extern double uct_owner[OWNER_MAX];
extern double uct_criticality[CRITICALITY_MAX];

//  初期化
void InitializeUctRating( void );
void InitializePhysicalFeaturesSet( void );

//  戦術的特徴のレートの計算
double CalculateMoveScoreWithBTFM( const game_info_t *game, const int pos, const unsigned int *tactical_features, const int distance_index );

//  現局面の評価
void AnalyzeUctRating( game_info_t *game, int color, double rate[] );

#endif
