/**
 * @file src/learn/PatternAnalyzer.cpp
 * @author Yuki Kobayashi.
 * @~english
 * @brief Stones pattern analysis for supervised learning.
 * @~japanese
 * @brief 教師あり学習用の配石パターンの抽出処理
 */
#include <algorithm>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>

#include "board/Color.hpp"
#include "board/GoBoard.hpp"
#include "board/Point.hpp"
#include "common/Message.hpp"
#include "learn/LearningSettings.hpp"
#include "learn/PatternAnalyzer.hpp"
#include "pattern/PatternHash.hpp"
#include "sgf/SgfExtractor.hpp"
#include "util/Utility.hpp"


static void ClearAllHashData( std::unique_ptr<hash_table_t> &count );

static void Count( std::unique_ptr<hash_table_t> &hash_table, unsigned long long hash, pattern_t *pat );

static int SearchData( std::unique_ptr<hash_table_t> &hash_table, unsigned long long hash );

static int SearchEmpty( std::unique_ptr<hash_table_t> &hash_table, unsigned long long hash );

static void RemoveData( std::unique_ptr<hash_table_t> &hash_table );

static void AddData( std::unique_ptr<hash_table_t> &hash_table, unsigned long long hash, pattern_t *pat );

static void OutputTargetPattern( std::unique_ptr<int[]> &md2_count, std::unique_ptr<hash_table_t> &md3_count, std::unique_ptr<hash_table_t> &md4_count, std::unique_ptr<hash_table_t> &md5_count );


/**
 * @~english
 * @brief Correct stones pattern.
 * @~japanese
 * @brief 配石パターンの収集
 */
void
AnalyzePattern( void )
{
  std::unique_ptr<int[]> md2_count(new int[MD2_MAX]);
  std::unique_ptr<hash_table_t> md3_count(new hash_table_t);
  std::unique_ptr<hash_table_t> md4_count(new hash_table_t);
  std::unique_ptr<hash_table_t> md5_count(new hash_table_t);
  game_info_t *game = AllocateGame(), *init_game = AllocateGame();
  pattern_hash_t hash_pat;
  SGF_record_t kifu;
  unsigned int md2_transp[16];
  unsigned int md2;
  unsigned int min_md2;


  std::cerr << "Start" << std::endl;

  std::fill_n(md2_count.get(), MD2_MAX, 0);
  ClearAllHashData(md3_count);
  ClearAllHashData(md4_count);
  ClearAllHashData(md5_count);

  InitializeBoard(init_game);

  for (int i = ANALYZE_KIFU_START; i <= ANALYZE_KIFU_END; i++) {
    int color = S_BLACK;
    std::ostringstream oss;

    CopyGame(game, init_game);

    oss << ANALYZE_KIFU_PATH << PATH_SEPARATOR << i << ".sgf";


    ExtractKifu(oss.str().c_str(), &kifu);

    std::cerr << oss.str() << ", " << kifu.moves << std::endl;

    for (int j = 0; j < kifu.moves; j++) {
      const int pos = GetKifuMove(&kifu, j);

      if (pos != PASS) {
        md2 = MD2(game->pat, pos);
        MD2Transpose16(md2, md2_transp);
        min_md2 = MD2_MAX;
        for (int k = 0; k < 16; k++) {
          if (min_md2 > md2_transp[k]) {
            min_md2 = md2_transp[k];
          }
        }
        md2_count[min_md2]++;

        PatternHash(&game->pat[pos], &hash_pat);
        Count(md3_count, hash_pat.list[MD_3], &game->pat[pos]);
        Count(md4_count, hash_pat.list[MD_4], &game->pat[pos]);
        Count(md5_count, hash_pat.list[MD_MAX + MD_5], &game->pat[pos]);
      }

      PutStone(game, pos, color);
      color = GetOppositeColor(color);
    }
  }

  OutputTargetPattern(md2_count, md3_count, md4_count, md5_count);

  FreeGame(game);
  FreeGame(init_game);

  std::cerr << "Finish" << std::endl;
}


/**
 * @~english
 * @brief Initialize pattern analysis data.
 * @param[in, out] count Pattern analysis data.
 * @~japanese
 * @brief 配石パターン収集用データの初期化
 * @param[in, out] count 配石パターン収集用データ
 */
static void
ClearAllHashData( std::unique_ptr<hash_table_t> &count )
{
  for (int i = 0; i < HASH_TABLE_MAX; i++) {
    count->data[i].hash = 0;
    for (int j = 0; j < 3; j++) {
      count->data[i].pattern[j] = 0;
    }
    count->data[i].large_pat[0] = 0;
    count->data[i].count = 0;
    count->data[i].appearance = 0;
    count->data[i].hash_flag = PatternHashStatus::Empty;
  }
  count->num;
}


/**
 * @~english
 * @brief Count up stones pattern.
 * @param[in, out] hash_table Pattern analysis data.
 * @param[in] hash Hash value.
 * @param[in] pat Stones pattern.
 * @~japanese
 * @brief 配石パターンの収集
 * @param[in, out] hash_table 配石パターン収集用データ
 * @param[in] hash ハッシュ値
 * @param[in] pat 配石パターン
 */
static void
Count( std::unique_ptr<hash_table_t> &hash_table, unsigned long long hash, pattern_t *pat )
{
  const int key = SearchData(hash_table, hash);

  if (key == -1) {
    if (hash_table->num >= HASH_TABLE_LIMIT) {
      RemoveData(hash_table);
    }
    AddData(hash_table, hash, pat);
  } else {
    hash_table->data[key].count++;
  }
}


/**
 * @~english
 * @brief Search stones pattern data.
 * @param[in] hash_table Stones pattern data.
 * @param[in] hash Hash value.
 * @~japanese
 * @brief 配石パターンデータの検索
 * @param[in] hash_table 配石パターンデータ
 * @param[in] hash ハッシュ値
 */
static int
SearchData( std::unique_ptr<hash_table_t> &hash_table, unsigned long long hash )
{
  const unsigned int key = TransHash24(hash);
  hash_data_t *data = hash_table->data;
  unsigned int i = key;

  do {
    if (data[i].hash_flag == PatternHashStatus::Empty) {
      return -1;
    } else if (data[i].hash_flag == PatternHashStatus::Registered &&
               data[i].hash == hash) {
      return i;
    }
    i++;
    if (i >= HASH_TABLE_MAX) i = 0;
  } while (i != key);

  return -1;
}


/**
 * @~english
 * @brief Remove stones pattern data.
 * @param[in, out] hash_table Stones pattern data.
 * @~japanese
 * @brief 配石パターンデータの削除
 * @param[in, out] hash_table 配石パターンデータ
 */
static void
RemoveData( std::unique_ptr<hash_table_t> &hash_table )
{
  hash_data_t *data = hash_table->data;

  for (int count = 1;; count++) {
    for (int i = 0; i < HASH_TABLE_MAX; i++) {
      if (data[i].hash_flag == PatternHashStatus::Registered) {
        if (data[i].count <= count) {
          data[i].hash_flag = PatternHashStatus::Deleted;
          hash_table->num--;
        }
      }
    }
    if (hash_table->num < HASH_TABLE_HALF) break;
  }
}


/**
 * @~english
 * @brief Add pattern data to stones pattern data.
 * @param[in, out] hash_table Stones pattern data.
 * @param[in] hash Hash value.
 * @param[in] pat Stones pattern.
 * @~japanese
 * @brief 配石パターンデータの追加
 * @param[in, out] hash_table 配石パターンデータ
 * @param[in] hash ハッシュ値
 * @param[in] pat 配石パターン
 */
static void
AddData( std::unique_ptr<hash_table_t> &hash_table, unsigned long long hash, pattern_t *pat )
{
  int key = SearchEmpty(hash_table, hash);
  if (key == -1) exit(1);

  hash_table->data[key].hash = hash;
  hash_table->data[key].pattern[MD_2] = pat->list[MD_2];
  hash_table->data[key].pattern[MD_3] = pat->list[MD_3];
  hash_table->data[key].pattern[MD_4] = pat->list[MD_4];
  hash_table->data[key].large_pat[MD_5] = pat->large_list[MD_5];
  hash_table->data[key].count = 1;
  hash_table->data[key].hash_flag = PatternHashStatus::Registered;

  hash_table->num++;
}


/**
 * @~english
 * @brief Search empty data index.
 * @param[in] hash_table Stones pattern data.
 * @param[in] hash Hash value.
 * @return Empty data index.
 * @~japanese
 * @brief 空のデータのインデックスの取得
 * @param[in] hash_table 配石パターンデータ
 * @param[in] hash ハッシュ値
 * @return からのデータのインデックス
 */
static int
SearchEmpty( std::unique_ptr<hash_table_t> &hash_table, unsigned long long hash )
{
  const unsigned int key = TransHash24(hash);
  hash_data_t *data = hash_table->data;
  unsigned int i = key;

  do {
    if (data[i].hash_flag == PatternHashStatus::Empty ||
        data[i].hash_flag == PatternHashStatus::Deleted) {
      return i;
    }
    i++;
    if (i >= HASH_TABLE_MAX) i = 0;
  } while (i != key);

  return -1;
}


/**
 * @~english
 * @brief Output stones pattern's appearance count.
 * @param[in] md2_count MD2 pattern data.
 * @param[in] md3_count MD3 pattern data.
 * @param[in] md4_count MD4 pattern data.
 * @param[in] md5_count MD5 pattern data.
 * @~japanese
 * @brief パターンの出現回数の出力
 * @param[in] md2_count MD2パターンのデータ
 * @param[in] md3_count MD3パターンのデータ
 * @param[in] md4_count MD4パターンのデータ
 * @param[in] md5_count MD5パターンのデータ
 */
static void
OutputTargetPattern( std::unique_ptr<int[]> &md2_count,
                     std::unique_ptr<hash_table_t> &md3_count,
                     std::unique_ptr<hash_table_t> &md4_count,
                     std::unique_ptr<hash_table_t> &md5_count )
{
  const std::string result_path = GetWorkingDirectory() + PATH_SEPARATOR +
    LEARNING_RESULT_DIR_NAME + PATH_SEPARATOR +
    PATTERN_TARGET_DIR_NAME + PATH_SEPARATOR;
  std::ofstream md2_ofs(result_path + MD2_TARGET_FILE_NAME);
  std::ofstream md3_ofs(result_path + MD3_TARGET_FILE_NAME);
  std::ofstream md4_ofs(result_path + MD4_TARGET_FILE_NAME);
  std::ofstream md5_ofs(result_path + MD5_TARGET_FILE_NAME);

  for (int i = 0; i < MD2_MAX; i++) {
    if (md2_count[i] >= APPEARANCE_MIN) {
      md2_ofs << i << std::endl;
    }
  }

  for (int i = 0; i < HASH_TABLE_MAX; i++) {
    if (md3_count->data[i].hash_flag == PatternHashStatus::Registered &&
        md3_count->data[i].count >= APPEARANCE_MIN) {
      md3_ofs << md3_count->data[i].hash << std::endl;
    }
  }

  for (int i = 0; i < HASH_TABLE_MAX; i++) {
    if (md4_count->data[i].hash_flag == PatternHashStatus::Registered &&
        md4_count->data[i].count >= APPEARANCE_MIN) {
      md4_ofs << md4_count->data[i].hash << std::endl;
    }
  }

  for (int i = 0; i < HASH_TABLE_MAX; i++) {
    if (md5_count->data[i].hash_flag == PatternHashStatus::Registered &&
        md5_count->data[i].count >= APPEARANCE_MIN) {
      md5_ofs << md5_count->data[i].hash << std::endl;
    }
  }
}
