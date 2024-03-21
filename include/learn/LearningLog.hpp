/**
 * @file include/learn/LearningLog.hpp
 * @author Yuki Kobayashi
 * @~english
 * @brief Log writer for machine learning process.
 * @~japanese
 * @brief 学習経過ログ出力
 */
#ifndef _LEARNING_LOG_HPP_
#define _LEARNING_LOG_HPP_

// 学習ログファイルの出力パスの設定
void SetLearningLogFilePath( void );

// 1ステップ学習した結果のログファイルを出力
void OutputLearningLogFile( const int update_steps, const int all_moves, const double consume_time );

// 1ステップ学習した結果のログファイルを出力
void OutputLearningLogFile( const int update_steps, const int all_moves, const double consume_time, const double accuracy, const bool is_train );

#endif
