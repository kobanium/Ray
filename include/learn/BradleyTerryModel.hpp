#ifndef _BRADLEY_TERRY_MODE_HPP_
#define _BRADLEY_TERRY_MODE_HPP_

#include "learn/LearningSettings.hpp"

// Factorzation Machinesの2次の項の次元
constexpr int FM_DIMENSION = 5;


struct mm_t {
  int w;
  double c;
  double sigma;
  double gamma;
};


struct btfm_t {
  double w;
  double v[FM_DIMENSION];
  double grad_w[TRAIN_THREAD_NUM];
  double grad_v[TRAIN_THREAD_NUM][FM_DIMENSION];
  double first_moment_w;
  double second_moment_w;
  double first_moment_v[FM_DIMENSION];
  double second_moment_v[FM_DIMENSION];
  int id;
};


struct train_thread_arg_t {
  int id;
  int step;
};

#endif
