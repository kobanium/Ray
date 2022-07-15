#include <fstream>
#include <iostream>


#include "learn/LearningUtility.hpp"
#include "pattern/Pattern.hpp"


void
OutputGamma( const std::string filename, const std::vector<mm_t> &data )
{
  std::ofstream ofs(filename, std::ios::out);

  if (!ofs) {
    std::cerr << "Cannot open \"" << filename << "\"" << std::endl;
    return;
  }

  for (const mm_t &datum : data) {
    ofs << std::scientific << datum.gamma << "\n";
  }
}


void
OutputGamma( const std::string filename, const std::vector<mm_t> &data, const std::vector<unsigned int> &index_list )
{
  std::ofstream ofs(filename, std::ios::out);

  if (!ofs) {
    std::cerr << "Cannot open \"" << filename << "\"" << std::endl;
    return;
  }

  const std::size_t data_size = data.size();

  for (std::size_t i = 0; i < data_size; i++) {
    if (data[i].gamma != 1.0) {
      ofs << index_list[i] << "\t" << data[i].gamma << "\n";
    }
  }
}


void
OutputGammaAdditionMode( const std::string filename, const mm_t &datum )
{
  std::ofstream ofs(filename, std::ios::app);

  if (!ofs) {
    std::cerr << "Cannot open \"" << filename << "\"" << std::endl;
    return;
  }

  ofs << std::scientific << datum.gamma << "\n";
}


void
InputMD2Target( const std::string filename, std::vector<int> &md2_index, std::vector<unsigned int> &md2_list, std::vector<bool> &md2_target )
{
  std::ifstream ifs(filename, std::ios::in);
  int target_index = 1;
  unsigned int index;

  if (ifs.fail()) {
    std::cerr << "Cannot open \"" << filename << "\"" << std::endl;
    return;
  }

  md2_index = std::vector<int>(MD2_MAX, 0);

  // 学習対象でないMD2パターン用のデータ
  md2_list.push_back(0);
  md2_target.push_back(false);

  while (ifs >> index) {
    md2_index[index] = target_index++;
    md2_list.push_back(index);
    md2_target.push_back(true);
  }
  std::cerr << "MD2 Target : " << md2_target.size() << std::endl;
}


int
GetUniquePattern( unsigned int pattern[], const int array_size )
{
  int size = 0;

  for (int i = 0; i < array_size; i++) {
    bool is_unique = true;
    for (int j = 0; j < size; j++) {
      if (pattern[i] == pattern[j]) {
        is_unique = false;
        break;
      }
    }
    if (is_unique) {
      pattern[size++] = pattern[i];
    }
  }

  return size;
}


std::string
TrimRightSpace( const std::string &src )
{
  std::string trimCharacters = " 　";
  size_t pos = src.find_last_not_of(trimCharacters);
  std::string dst = src.substr(0, pos + 1);
  return dst;
}
