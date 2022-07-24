#ifndef _LEARNING_LOG_HPP_
#define _LEARNING_LOG_HPP_


void SetLearningLogFilePath( void );

void OutputLearningLogFile( const int update_steps, const int all_moves, const double consume_time );

void OutputLearningLogFile( const int update_steps, const int all_moves, const double consume_time, const double accuracy );

#endif
