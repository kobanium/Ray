#ifndef _PATTERN_ANALYZER_HPP_
#define _PATTERN_ANALYZER_HPP_

#include "board/BoardData.hpp"
#include "pattern/Pattern.hpp"


constexpr char ANALYZE_KIFU_PATH[] = "/home/user/SGF_Files/tygem-data/shuffle-tygem";

constexpr char ANALYZE_RESULT_PATH[] = "learning_result/analyze";

constexpr int ANALYZE_KIFU_NUM = 300000;

constexpr int HASH_TABLE_MAX = 16777216;

constexpr int HASH_TABLE_LIMIT = 16000000;

constexpr int HASH_TABLE_HALF = 8388608;

constexpr int APPEARANCE_MIN = 10;


enum class PatternHashStatus {
  Empty,
  Registered,
  Deleted,
  Max,
};


struct hash_data_t {
  unsigned long long hash;
  unsigned int pattern[3];
  unsigned long long large_pat[1];
  int count;
  int appearance;
  PatternHashStatus hash_flag;

};


struct hash_table_t {
  hash_data_t data[HASH_TABLE_MAX];
  int num;
};


void AnalyzePattern( void );

#endif
