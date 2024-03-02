/**
 * @file include/util/Utility.hpp
 * @author Yuki Kobayashi
 * @~english
 * @brief Utility functions.
 * @~japanese
 * @brief 多用途処理
 */
#ifndef _UTILITY_HPP_
#define _UTILITY_HPP_

#include <chrono>
#include <string>
#include <vector>

/**
 * @~english
 * @brief Path separator charactor.
 * @~japanese
 * @brief パスセパレータの文字
 */
#if defined (_WIN32)
const std::string PATH_SEPARATOR = "\\";
#else
const std::string PATH_SEPARATOR = "/";
#endif


/**
 * @typedef ray_clock;
 * @~english
 * @brief Type definition for time measurement.
 * @~japanese
 * @brief 時間計測用の型定義
 */
typedef std::chrono::high_resolution_clock ray_clock;


/**
 * @~english
 * @brief Calculate time consumption (second).
 * @param[in] start_time Starting time.
 * @return Time consumption.
 * @~japanese
 * @brief 消費時間の算出
 * @param[in] start_time 開始時間
 * @return 消費時間
 */
inline double
GetSpendTime( const ray_clock::time_point& start_time )
{
  return std::chrono::duration_cast<std::chrono::milliseconds>(ray_clock::now() - start_time).count() / 1000.0;
}


// データ読み込み(float)
void InputTxtFLT( const char *filename, float *ap, const int array_size );

// データ読み込み(double)
void InputTxtDBL( const char *filename, double *ap, const int array_size );

// ワーキングディレクトリの設定
void SetWorkingDirectory( const char *program_path );

// ワーキングディレクトリの取得
std::string GetWorkingDirectory( void );

// 文字列の結合
std::string JoinStrings( const std::vector<std::string> &str_vec, const char *deliminter );

// 指定文字列で文字列を分割
std::vector<std::string> SplitString( const std::string &str, const std::string pattern );

#endif
