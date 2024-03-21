/**
 * @file include/pattern/PatternHash.hpp
 * @author Yuki Kobayashi
 * @~english
 * @brief Zobrist hash for neighborhood stones' pattern.
 * @~japanese
 * @brief 近傍の配石パターンを表現するZobrist Hash
 */
#ifndef _PATTERNHASH_HPP_
#define _PATTERNHASH_HPP_

#include "board/GoBoard.hpp"
#include "pattern/Pattern.hpp"


/**
 * @~english
 * @brief Default pattern hash table size (2^20).
 * @~japanese
 * @brief パターン用ハッシュテーブルのデフォルトサイズ (2^20)
 */
constexpr int HASH_MAX = 1048576; // 2^20

/**
 * @~english
 * @brief Bit mask size for pattern hash.
 * @~japanese
 * @brief パターン用ハッシュのビットマスクのサイズ
 */
constexpr int BIT_MAX = 60;


/**
 * @~english
 * @brief Hash value of each pattern.
 * @~japanese
 * @brief 各サイズのパターンのハッシュ値
 */
struct pattern_hash_t {
  /**
   * @~english
   * @brief List of hash value
   * @~japanese
   * @brief ハッシュ値のリスト
   */
  unsigned long long list[MD_MAX + MD_LARGE_MAX];
};


/**
 * @~english
 * @brief Corresspondance table of indexes in hash table.
 * @~japanese
 * @brief ハッシュテーブルのインデックスの対応表
 */
struct index_hash_t {
  /**
   * @~english
   * @brief Hash value
   * @~japanese
   * @brief ハッシュ値
   */
  unsigned long long hash;
  /**
   * @~english
   * @brief Index of hash table.
   * @~japanese
   * @brief ハッシュテーブルのインデックス
   */
  int index;
};


/**
 * @~english
 * @brief Return 24-bit compressed zobrist hash value.
 * @param[in] hash Zobrist hash value
 * @return 24-bit compressed zobrist hash value.
 * @~japanese
 * @brief 24bitに圧縮したハッシュ値を返す
 * @param[in] hash ハッシュ値
 * @return 24bitに圧縮したハッシュ値を返す
 */
inline unsigned int
TransHash24( const unsigned long long hash )
{
  return ((hash & 0xFFFFFFFF) ^ ((hash >> 32) & 0xFFFFFFFF)) & 0xFFFFFF;
}


/**
 * @~english
 * @brief Return 20-bit compressed hash value.
 * @param[in] hash Hash value
 * @return 20-bit compressed zobrist hash value.
 * @~japanese
 * @brief 20bitに圧縮したハッシュ値を返す
 * @param[in] hash ハッシュ値
 * @return 20bitに圧縮したハッシュ値を返す
 */
inline unsigned int
TransHash20( const unsigned long long hash )
{
  return ((hash & 0xFFFFFFFF) ^ ((hash >> 32) & 0xFFFFFFFF)) & 0xFFFFF;
}


//  パターンのハッシュ関数
void PatternHash( const pattern_t *pat, pattern_hash_t *hash_pat );

//  インデックスを探索
int SearchIndex( const index_hash_t *index, const unsigned long long hash );

#endif
