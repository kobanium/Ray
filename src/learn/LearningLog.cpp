#include <iomanip>
#include <iostream>
#include <fstream>
#include <string>

#include "learn/LearningLog.hpp"
#include "util/Utility.hpp"


static std::string learning_log_file_path;


void
SetLearningLogFilePath( void )
{
  learning_log_file_path = GetWorkingDirectory() + PATH_SEPARATOR + "learning_result" + PATH_SEPARATOR + "logfile.log";
}


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
