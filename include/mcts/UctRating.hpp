/**
 * @file include/mcts/UctRating.hpp
 * @author Yuki Kobayashi
 * @~english
 * @brief Move scoring for UCT.
 * @~japanese
 * @brief 木探索用の着手評価
 */
#ifndef _UCTRATING_HPP_
#define _UCTRATING_HPP_

#include <string>

#include "board/GoBoard.hpp"
#include "feature/UctFeature.hpp"
#include "pattern/PatternHash.hpp"


/**
 * @~english
 * @brief Dimension for factorization machines.
 * @~japanese
 * @brief Factorization Machinesの二次の項の次元数
 */
constexpr int BTFM_DIMENSION = 5;

/**
 * @~english
 * @brief Maxmum number of large patterns.
 * @~japanese
 * @brief 大きなパターンの個数の最大値
 */
constexpr int LARGE_PAT_MAX = 1000000;

// Ownerは
// 0-5% 6-15% 16-25% 26-35% 36-45% 46-55%
// 56-65% 66-75% 76-85% 86-95% 96-100%
// の11区分
/**
 * @~english
 * @brief The number of ownership feature values.
 * @~japanese
 * @brief Ownerの特徴の値の個数
 */
constexpr int OWNER_MAX = 11;

/**
 * @~english
 * @brief The number of criticality feature values.
 * @~japanese
 * @brief Criticalityの特徴の個数
 */
constexpr int CRITICALITY_MAX = 25;

/**
 * @~english
 * @brief Scale coefficient for criticality feature.
 * @~japanese
 * @brief Criticalityの特徴のスケール係数
 */
constexpr double CRITICALITY_TERM = 100.0;

/**
 * @~english
 * @brief Constant value for criticality feature.
 * @~japanese
 * @brief Criticalityの特徴の定数
 */
constexpr double CRITICALITY_INIT = 0.765745;

/**
 * @~english
 * @brief Coefficient for criticality feature.
 * @~japanese
 * @brief Criticalityの特徴の係数
 */
constexpr double CRITICALITY_BIAS = 0.32;

/**
 * @~english
 * @brief Constant value for ownership feature.
 * @~japanese
 * @brief Ownerの特徴の定数
 */
constexpr double OWNER_K = 1.8;

/**
 * @~english
 * @brief Coefficient for ownership feature.
 * @~japanese
 * @brief Ownerの特徴の係数
 */
constexpr double OWNER_BIAS = 6.6;


/**
 * @struct fm_t
 * @~english
 * @brief Parameters for Bradley-Terry model with factorization machines.
 * @~japanese
 * @brief Factorization Machinesを用いたBradley-Terryモデルのパラメータ
 */
struct fm_t {
  /**
   * @~english
   * @brief First term parameter.
   * @~japanese
   * @brief 1次の項のパラメータ
   */
  double w;

  /**
   * @~english
   * @brief Second term parameters.
   * @~japanese
   * @brief 2次の項のパラメータ
   */
  double v[BTFM_DIMENSION];
};


// Ownershipの特徴スコア
extern double uct_owner[OWNER_MAX];
// Criticalityの特徴スコア
extern double uct_criticality[CRITICALITY_MAX];

//  初期化
void InitializeUctRating( void );

//  戦術的特徴のレートの計算
double CalculateMoveScoreWithBTFM( const game_info_t *game, const int pos, const unsigned int *tactical_features, const int distance_index );

//  現局面の評価
void AnalyzeUctRating( game_info_t *game, int color, double rate[] );

#endif
