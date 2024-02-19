/**
 * @file PatternAnalyzer.hpp
 * @author Yuki Kobayashi
 * @~english
 * @brief Pattern analyzer for correcting learning targets.
 * @~japanese
 * @brief 学習対象のパターンを収集するための分析処理
 */
#ifndef _PATTERN_ANALYZER_HPP_
#define _PATTERN_ANALYZER_HPP_

#include "board/BoardData.hpp"
#include "pattern/Pattern.hpp"


/**
 * @~english
 * @brief Path to the directory where sgf files to be analyzed are stored.
 * @~japanese
 * @brief 分析対象の棋譜が格納されているディレクトリのパス
 */
constexpr char ANALYZE_KIFU_PATH[] = "/home/user/sgf";

/**
 * @~english
 * @brief Fist SGF file index for collecting pattern targets.
 * @~japanese
 * @brief 分析対象とする棋譜の最初のインデックス
 */
constexpr int ANALYZE_KIFU_START = 1;

/**
 * @~english
 * @brief Last SGF file index for collecting pattern targets.
 * @~japanese
 * @brief 分析対象とする棋譜の最後のインデックス
 */
constexpr int ANALYZE_KIFU_END = 30000;

/**
 * @~english
 * @brief Hash table size.
 * @~japanese
 * @brief ハッシュテーブルサイズ
 */
constexpr int HASH_TABLE_MAX = 16777216;

/**
 * @~english
 * @brief Limitation on the number of registered data.
 * @~japanese
 * @brief 登録データの上限値
*/
constexpr int HASH_TABLE_LIMIT = 16000000;

/**
 * @~english
 * @brief Half size of hash table.
 * @~japanese
 * @brief ハッシュテーブルサイズの半分
 */
constexpr int HASH_TABLE_HALF = HASH_TABLE_MAX / 2;

/**
 * @~english
 * @brief Minimum appearance count for target pattern.
 * @~japanese
 * @brief 学習対象とするパターンの最小出現回数
 */
constexpr int APPEARANCE_MIN = 10;


/**
 * @enum PatternHashStatus
 * @~english
 * @brief Status of hash table element.
 * @var Empty
 * Unregistered.
 * @var Registered
 * Already registered.
 * @var Deleted
 * Already deleted.
 * @var Max
 * Sentinel.
 * @~japanese
 * @brief ハッシュテーブルの要素の状態
 * @var Empty
 * 未登録
 * @var Registered
 * 登録済み
 * @var Deleted
 * 削除済み
 * @var Max
 * 番兵
 */
enum class PatternHashStatus {
  Empty,
  Registered,
  Deleted,
  Max,
};


/**
 * @struct hash_data_t
 * @~english
 * @brief Pattern analysis data.
 * @~japanese
 * @brief パターン分析データ
 */
struct hash_data_t {
  /**
   * @~english
   * @brief Hash value.
   * @~japanese
   * @brief ハッシュ値
   */
  unsigned long long hash;

  /**
   * @~english
   * @brief Stone pattern.
   * @~japanese
   * @brief パターン
   */
  unsigned int pattern[3];

  /**
   * @~english
   * @brief Large stone pattern.
   * @~japanese
   * @brief 大きなパターン
   */
  unsigned long long large_pat[1];

  /**
   * @~english
   * @brief The number of pattern played count.
   * @~japanese
   * @brief パターンの着手回数
   */
  int count;

  /**
   * @~english
   * @brief The number of pattern appearance.
   * @~japanese
   * @brief パターンの出現回数
   */
  int appearance;

  /**
   * @~english
   * @brief Status of a hash table element.
   * @~japanese
   * @brief 登録データの状態
   */
  PatternHashStatus hash_flag;
};


/**
 * @struct hash_table_t
 * @~english
 * @brief Hash table for pattern analysis data.
 * @~japanese
 * @brief パターン分析データのハッシュテーブル
 */
struct hash_table_t {
  /**
   * @~english
   * @brief Hash table for patttern analysis data.
   * @~japanese
   * @brief パターン分析データのテーブル
   */
  hash_data_t data[HASH_TABLE_MAX];

  /**
   * @~english
   * @brief The number of registered data.
   * @~japanese
   * @brief 登録済みのデータ数
   */
  int num;
};


// 学習対象の配石パターンの収集
void AnalyzePattern( void );

#endif
