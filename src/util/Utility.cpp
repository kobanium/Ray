/**
 * @file src/util/Utility.cpp
 * @author Yuki Kobayashi
 * @~english
 * @brief Utility functions.
 * @~japanese
 * @brief 汎用処理
 */
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <regex>
#include <string>
#include <vector>

#include "util/Utility.hpp"


/**
 * @~english
 * @brief Working directory.
 * @~japanese
 * @brief プログラムのワーキングディレクトリ
 */
static std::string working_directory;


/**
 * @~english
 * @brief Set Ray's working directory.
 * @param[in] program_path Current working directory.
 * @~japanese
 * @brief ワーキングディレクトリの設定
 * @param[in] program_path 設定するワーキングディレクトリ
 */
void
SetWorkingDirectory( const char *program_path )
{
  working_directory = std::string(program_path);
}


/**
 * @~english
 * @brief Get Ray's Working directory.
 * @return Ray's working directory.
 * @~japanese
 * @brief ワーキングディレクトリの取得
 * @return Rayのワーキングディレクトリ
 */
std::string
GetWorkingDirectory( void )
{
  return working_directory;
}


/**
 * @~english
 * @brief Read a text file (float type data).
 * @param[in] filename File name of a text file.
 * @param[out] ap Returned data.
 * @param[in] array_size The number of read lines.
 * @~japanese
 * @brief テキストファイルの読み込み (float)
 * @param[in] filename 読み込むファイル名
 * @param[out] ap 読み込んだデータの格納先
 * @param[in] array_size 読み込むデータの個数
 */
void
InputTxtFLT( const char *filename, float *ap, const int array_size )
{
  FILE *fp;
#if defined (_WIN32)
  errno_t err;

  err = fopen_s(&fp, filename, "r");
  if (err != 0) {
    std::cerr << "can not open -" << filename << "-" << std::endl;
    exit(1);
  }
  for (int i = 0; i < array_size; i++) {
    if (fscanf_s(fp, "%f", &ap[i]) == EOF) {
      std::cerr << "Read Error : " << filename << std::endl;
    }
  }
#else
  fp = fopen(filename, "r");
  if (fp == NULL) {
    std::cerr << "can not open -" << filename << "-" << std::endl;
  }
  for (int i = 0; i < array_size; i++) {
    if (fscanf(fp, "%f", &ap[i]) == EOF) {
      std::cerr << "Read Error : " << filename << std::endl;
      exit(1);
    }
  }
#endif
  fclose(fp);
}


/**
 * @~english
 * @brief Read a text file (double type data).
 * @param[in] filename File name of a text file.
 * @param[out] ap Returned data.
 * @param[in] array_size The number of read lines.
 * @~japanese
 * @brief テキストファイルの読み込み (double)
 * @param[in] filename 読み込むファイル名
 * @param[out] ap 読み込んだデータの格納先
 * @param[in] array_size 読み込むデータの個数
 */
void
InputTxtDBL( const char *filename, double *ap, const int array_size )
{
  FILE *fp;
#if defined (_WIN32)
  errno_t err;

  err = fopen_s(&fp, filename, "r");
  if (err != 0) {
    std::cerr << "can not open -" << filename << "-" << std::endl;
  }
  for (int i = 0; i < array_size; i++) {
    if (fscanf_s(fp, "%lf", &ap[i]) == EOF) {
      std::cerr << "Read Error : " << filename << std::endl;
    }
  }
#else
  fp = fopen(filename, "r");
  if (fp == NULL) {
    std::cerr << "can not open -" << filename << "-" << std::endl;
  }
  for (int i = 0; i < array_size; i++) {
    if (fscanf(fp, "%lf", &ap[i]) == EOF) {
      std::cerr << "Read Error : " << filename << std::endl;
    }
  }
#endif
  fclose(fp);
}


/**
 * @~english
 * @brief Join strings.
 * @param[in] str_vec Vector of string that is joined.
 * @param[in] delimiter Delimiter string.
 * @return Joined string.
 * @~japanese
 * @brief 指定の区切り文字で文字列を結合処理
 * @param[in] str_vec 結合対象文字列
 * @param[in] delimiter 区切り文字
 * @return 結合した文字列
 */
std::string
JoinStrings( const std::vector<std::string> &str_vec, const char *delimiter = 0 )
{
  std::string result;

  if (!str_vec.empty()) {
    result += str_vec[0];
    for (decltype(str_vec.size()) i = 1, c = str_vec.size(); i < c; i++) {
      if (delimiter) result += delimiter;
      result += str_vec[i];
    }
  }
  return result;
}


/**
 * @~english
 * @brief Split string.
 * @param[in] str Original string.
 * @param[in] pattern Regex pattern.
 * @return Splitted strings.
 * @~japanese
 * @brief 文字列の分割処理
 * @param[in] str 分割する文字列
 * @param[in] pattern 正規表現文字列
 * @return 分割した文字列
 */
std::vector<std::string>
SplitString( const std::string &str, const std::string pattern )
{
  std::vector<std::string> result;
  std::regex reg{pattern};

  std::copy(std::sregex_token_iterator{str.begin(), str.end(), reg, -1},
            std::sregex_token_iterator{},
            std::back_inserter(result));

  return result;
}
