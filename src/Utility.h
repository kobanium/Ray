#ifndef _UTILITY_H_
#define _UTILITY_H_

#include <ctime>

// 消費時間の算出
double GetSpendTime( const clock_t start_time );

#if !defined (_WIN32)
double GetSpendTimeForLinux( const struct timeval *start_time );
#endif

// データ読み込み(float)
void InputTxtFLT( const char *filename, float *ap, const int array_size );

// データ読み込み(double)
void InputTxtDBL( const char *filename, double *ap, const int array_size );

#endif
