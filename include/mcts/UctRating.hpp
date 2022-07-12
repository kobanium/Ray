#ifndef _UCTRATING_HPP_
#define _UCTRATING_HPP_

#include <string>

#include "board/GoBoard.hpp"
#include "feature/UctFeature.hpp"
#include "pattern/PatternHash.hpp"


const int LFR_DIMENSION = 5;

const int POS_ID_MAX = 64;        // 7bitで表現

const int LARGE_PAT_MAX = 150000;

// Ownerは
// 0-5% 6-15% 16-25% 26-35% 36-45% 46-55%
// 56-65% 66-75% 76-85% 86-95% 96-100%
// の11区分
const int OWNER_MAX = 11;
const int CRITICALITY_MAX = 7;//13;  //7

const int UCT_PHYSICALS_MAX = (1 << 14);

const double CRITICALITY_INIT = 0.765745;
const double CRITICALITY_BIAS = 0.036;

const double OWNER_K = 0.05;
const double OWNER_BIAS = 34.0;


struct latent_factor_t {
  double w;
  double v[LFR_DIMENSION];
};


extern double uct_owner[OWNER_MAX];
extern double uct_criticality[CRITICALITY_MAX];

extern index_hash_t md3_index[HASH_MAX];
extern index_hash_t md4_index[HASH_MAX];
extern index_hash_t md5_index[HASH_MAX];

extern unsigned long long atari_mask;
extern unsigned long long capture_mask;


//  初期化
void InitializeUctRating( void );
void InitializePhysicalFeaturesSet( void );

//  戦術的特徴のレートの計算
double CalculateLFRScore( game_info_t *game, int pos, int pat_index[], uct_features_t *uct_features );

//  現局面の評価
void AnalyzeUctRating( game_info_t *game, int color, double rate[] );

#endif
