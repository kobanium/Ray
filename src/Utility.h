#ifndef _UTILITY_H_
#define _UTILITY_H_

#include <chrono>

////////////
//  関数  //
////////////
typedef std::chrono::high_resolution_clock ray_clock;

// 消費時間の算出
inline double GetSpendTime(const ray_clock::time_point& start_time) {
  return std::chrono::duration_cast<std::chrono::milliseconds>(ray_clock::now() - start_time).count() / 1000.0;
}

//  データ読み込み(float)
void InputTxtFLT( const char *filename, float *ap, const int array_size );

//  データ読み込み(double)
void InputTxtDBL( const char *filename, double *ap, const int array_size );

#endif
