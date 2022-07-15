#ifndef _LEARNING_UTILITY_HPP_
#define _LEARNING_UTILITY_HPP_

#include <string>
#include <vector>

#include "learn/BradleyTerryModel.hpp"


void OutputGamma( const std::string filename, const std::vector<mm_t> &data );
  
void OutputGamma( const std::string filename, const std::vector<mm_t> &data, const std::vector<unsigned int> &index_list );

void OutputGammaAdditionMode( const std::string filename, const mm_t &datum );

void InputMD2Target( const std::string filename, std::vector<int> &md2_index, std::vector<unsigned int> &md2_list, std::vector<bool> &md2_target );

int GetUniquePattern( unsigned int pattern[], const int array_size );

std::string TrimRightSpace( const std::string &src );

#endif
