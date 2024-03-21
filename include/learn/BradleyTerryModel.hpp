/**
 * @file include/learn/BradleyTerryModel.hpp
 * @author Yuki Kobayashi
 * @~english
 * @brief Data definition of Bradley-Terry model for supervised learning.
 * @~japanese
 * @brief 教師あり学習で利用するBTモデルのデータ定義
 */
#ifndef _BRADLEY_TERRY_MODE_HPP_
#define _BRADLEY_TERRY_MODE_HPP_

#include "learn/LearningSettings.hpp"

/// @~english
/// @brief Dimension of Factorization Machines' second term.
/// @~japanese
/// @brief Factorization Machinesの2次の項の次元
constexpr int FM_DIMENSION = 5;


/**
 * @~english
 * @brief Data structure of Minorization Maximization method.
 * @~japanese
 * @brief MM法で用いるデータ構造
 */
struct mm_t {
  /**
   * @~english
   * @brief Total number of supervisor's move.
   * @~japanese
   * @brief 教師データの合計着手回数
   */
  int w;

  /**
   * @~english
   * @brief Strength of team.
   * @~japanese
   * @brief チームの強さ.
   */
  double c;

  /**
   * @~english
   * @brief Total sum.
   * @~japanese
   * @brief 総和
   */
  double sigma;

  /**
   * @~english
   * @brief Gamma value of feature.
   * @~japanese
   * @brief γ値
   */
  double gamma;
};

/**
 * @~english
 * @brief Data structure of Factorization Machines based Bradley-Terry model.
 * @~japanese
 * @brief Factorization Machinesを用いたBTモデルのデータ構造
 */
struct btfm_t {
  /**
   * @~english
   * @brief Feature value of first term.
   * @~japanese
   * @brief 1次の項の特徴の値
   */
  double w;

  /**
   * @~english
   * @brief Feature value of second term.
   * @~japanese
   * @brief 2次の項の特徴の値
   */
  double v[FM_DIMENSION];

  /**
   * @~english
   * @brief Gradient of first term.
   * @~japanese
   * @brief 1次の項の勾配
   */
  double grad_w[TRAIN_THREAD_NUM];

  /**
   * @~english
   * @brief Gradient of second term.
   * @~japanese
   * @brief 2次の項の勾配
   */
  double grad_v[TRAIN_THREAD_NUM][FM_DIMENSION];

  /**
   * @~english
   * @brief First momentum of first term.
   * @~japanese
   * @brief 1次の項の1次Momentum
   */
  double first_moment_w;

  /**
   * @~english
   * @brief Second momentum of first term.
   * @~japanese
   * @brief 1次の項の2次Momentum
   */
  double second_moment_w;

  /**
   * @~english
   * @brief First momentum of first term.
   * @~japanese
   * @brief 2次の項の1次Momentum
   */
  double first_moment_v[FM_DIMENSION];

  /**
   * @~english
   * @brief Second momentum of second term.
   * @~japanese
   * @brief 2次の項の2次Momentum
   */
  double second_moment_v[FM_DIMENSION];

  /**
   * @~english
   * @brief Feature ID.
   * @~japanese
   * @brief 特徴ID
   */
  int id;
};


/**
 * @~english
 * @brief Arguments for training worker thread.
 * @~japanese
 * @brief 学習ワーカスレッドの引数
 */
struct train_thread_arg_t {
  /**
   * @~english
   * @brief Training worker thread ID.
   * @~japanese
   * @brief 学習ワーカスレッドID
   */
  int id;

  /**
   * @~english
   * @brief Current number of training steps.
   * @~japanese
   * @brief 現在の学習ステップ数
   */
  int step;
};

#endif
