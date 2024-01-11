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
      ofs << index_list[i] << "\t" << std::scientific << data[i].gamma << "\n";
    }
  }
}


void
OutputBTFMParameter( const std::string filename, const std::vector<btfm_t> &data )
{
  std::ofstream ofs(filename, std::ios::out);

  if (!ofs) {
    std::cerr << "Cannot open \"" << filename << "\"" << std::endl;
    return;
  }

  for (const btfm_t &datum : data) {
    ofs << std::scientific << datum.w;
    for (int k = 0; k < FM_DIMENSION; k++) {
      ofs << " " << std::scientific << datum.v[k];
    }
    ofs << "\n";
  }
}


void
OutputBTFMParameter( const std::string filename, const std::vector<btfm_t> &data, const std::vector<unsigned int> &index_list, const std::vector<bool> &target )
{
  std::ofstream ofs(filename, std::ios::out);

  if (!ofs) {
    std::cerr << "Cannot open \"" << filename << "\"" << std::endl;
    return;
  }

  const std::size_t data_size = target.size();

  for (std::size_t i = 0; i < data_size; i++) {
    if (target[i]) {
      ofs << index_list[i] << "\t" << std::scientific << data[i].w;
      for (int j = 0; j < FM_DIMENSION; j++) {
        ofs << " " << std::scientific << data[i].v[j];
      }
      ofs << "\n";
    }
  }
}


void
OutputBTFMParameter( const std::string filename, const std::vector<btfm_t> &data, const std::vector<unsigned int> &list, const std::vector<index_hash_t> &index, const std::vector<bool> &target )
{
  std::ofstream ofs(filename, std::ios::out);

  if (!ofs) {
    std::cerr << "Cannot open \"" << filename << "\"" << std::endl;
    return;
  }

  const std::size_t data_size = target.size();

  for (std::size_t i = 0; i < data_size; i++) {
    if (target[i]) {
      ofs << list[i] << "\t" << index[list[i]].hash << "\t" << std::scientific << data[i].w;
      for (int j = 0; j < FM_DIMENSION; j++) {
        ofs << " " << std::scientific << data[i].v[j];
      }
      ofs << "\n";
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
  unsigned int index, md2_transpose16[16];

  if (ifs.fail()) {
    std::cerr << "Cannot open \"" << filename << "\"" << std::endl;
    return;
  }

  md2_index = std::vector<int>(MD2_MAX, 0);

  // 学習対象でないMD2パターン用のデータ
  md2_list.push_back(0);
  md2_target.push_back(false);

  while (ifs >> index) {
    MD2Transpose16(index, md2_transpose16);
    for (int i = 0; i < 16; i++) {
      md2_index[md2_transpose16[i]] = target_index;
    }
    md2_list.push_back(index);
    md2_target.push_back(true);
    target_index++;
  }
  std::cerr << "MD2 Target : " << md2_target.size() << std::endl;
}


void
InputLargePatternTarget( const std::string filename, std::vector<index_hash_t> &index, std::vector<unsigned int> &list, std::vector<bool> &target )
{
  std::ifstream ifs(filename, std::ios::in);
  int target_index = 1;
  unsigned long long hash;


  if (ifs.fail()) {
    std::cerr << "Cannot open \"" << filename << "\"" << std::endl;
    return;
  }

  index.resize(HASH_MAX);

  for (index_hash_t &datum : index) {
    datum.hash = 0;
    datum.index = -1;
  }

  // 学習対称でないパターン用のデータ
  list.push_back(0);
  target.push_back(false);

  while (ifs >> hash) {
    const unsigned int trans = TransHash20(hash);
    unsigned int idx = trans;
    do {
      if (index[idx].hash == 0) break;
      idx++;
      if (idx >= HASH_MAX) idx = 0;
    } while (idx != trans);

    index[idx].hash = hash;
    index[idx].index = target_index++;
    list.push_back(idx);
    target.push_back(true);
  }
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
