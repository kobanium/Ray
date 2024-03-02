/**
 * @file include/learn/LearningUtility.hpp
 * @author Yuki Kobayashi
 * @~english
 * @brief Utility functions for Learning.
 * @~japanese
 * @brief 学習用の処理
 */
#ifndef _LEARNING_UTILITY_HPP_
#define _LEARNING_UTILITY_HPP_

#include <string>
#include <vector>

#include "learn/BradleyTerryModel.hpp"
#include "pattern/PatternHash.hpp"


// 通常のデータの出力
void OutputGamma( const std::string filename, const std::vector<mm_t> &data );

// MD2パターン用の出力
void OutputGamma( const std::string filename, const std::vector<mm_t> &data, const std::vector<unsigned int> &index_list );

// 学習過程を追記
void OutputGammaAdditionMode( const std::string file, const mm_t &datum );

// 学習したパラメータの出力
void OutputBTFMParameter( const std::string filename, const std::vector<btfm_t> &data );

// 学習したパラメータの出力
void OutputBTFMParameter( const std::string filename, const std::vector<btfm_t> &data, const std::vector<unsigned int> &index_list, const std::vector<bool> &target );

// 学習したパラメータの出力
void OutputBTFMParameter( const std::string filename, const std::vector<btfm_t> &data, const std::vector<unsigned int> &list, const std::vector<index_hash_t> &index, const std::vector<bool> &target );

// 学習対象のMD2パターンの読み込み
void InputMD2Target( const std::string filename, std::vector<int> &md2_index, std::vector<unsigned int> &md2_list, std::vector<bool> &md2_target );

// 学習対象のパターンの読み込み
void InputLargePatternTarget( const std::string filename, std::vector<index_hash_t> &index, std::vector<unsigned int> &list, std::vector<bool> &target );

// ユニークなパターン数のカウント
int GetUniquePattern( unsigned int pattern[], const int array_size );

// 右側のスペースを削除
std::string TrimRightSpace( const std::string &src );

#endif
