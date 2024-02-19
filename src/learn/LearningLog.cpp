/**
 * @file LearningLog.cpp
 * @author Yuki Kobayashi
 * @~english
 * @brief Configuration and writing supervised learning log file.
 * @~japanese
 * @brief 学習経過ログファイルの設定と出力
 */
#include <iomanip>
#include <iostream>
#include <fstream>
#include <string>

#include "learn/LearningLog.hpp"
#include "util/Utility.hpp"


/**
 * @~english
 * @brief Path of learning log file.
 * @~japanese
 * @brief 学習経過ログファイルのパス
 */
static std::string learning_log_file_path;


/**
 * @~english
 * @brief Set learning log file path.
 * @~japanese
 * @brief 学習経過ログファイルパスの設定
 */
void
SetLearningLogFilePath( void )
{
  learning_log_file_path = GetWorkingDirectory() + PATH_SEPARATOR + "learning_result" + PATH_SEPARATOR + "logfile.log";
}


/**
 * @~english
 * @brief Output learning log.
 * @param[in] update_steps Current epoch.
 * @param[in] all_moves Total training samples.
 * @param[in] consume_time Training time of current step. 
 * @~japanese
 * @brief 学習ログの出力
 * @param[in] update_steps 現在のエポック数
 * @param[in] all_moves 学習サンプル数
 * @param[in] consume_time 学習にかかった時間
 */
void
OutputLearningLogFile( const int update_steps, const int all_moves, const double consume_time )
{
  std::ofstream ofs(learning_log_file_path, std::ios::app);

  if (!ofs) {
    std::cerr << "Training log file open error." << std::endl;
    std::cerr << update_steps << ", all_moves : " << all_moves << ", run_time : " << consume_time << " seconds" << std::endl;
  } else {
    ofs << "step " << update_steps << ", all_moves : " << all_moves << ", run_time : ";
    ofs << std::fixed << std::setprecision(3) << consume_time << " seconds\n";
  }
}


/**
 * @~english
 * @brief Output learning log.
 * @param[in] update_steps Current epoch.
 * @param[in] all_moves Total training samples.
 * @param[in] consume_time Training time of current step. 
 * @param[in] accuracy Accuracy of move predictions.
 * @param[in] is_train Training flag.
 * @~japanese
 * @brief 学習ログの出力
 * @param[in] update_steps 現在のエポック数
 * @param[in] all_moves 学習サンプル数
 * @param[in] consume_time 学習にかかった時間
 * @param[in] accuracy 着手予測の正解率
 * @param[in] is_train 学習実行時フラグ
 */
void
OutputLearningLogFile( const int update_steps, const int all_moves, const double consume_time, const double accuracy, const bool is_train )
{
  std::ofstream ofs(learning_log_file_path, std::ios::app);
  std::string phase = is_train ? "Training" : "Testing ";

  if (!ofs) {
    std::cerr << phase << " step " << update_steps << ", accuracy : " << accuracy << std::endl;
    std::cerr << ", all_moves : " << all_moves << ", run_time : " << consume_time << " seconds" << std::endl;
  } else {
    ofs << phase << " step " << update_steps << ", accuracy : " << accuracy;
    ofs << ", all_moves : " << all_moves << ", run_time : ";
    ofs << std::fixed << std::setprecision(3) << consume_time << " seconds\n";
  }
}
