/**
 * @file include/board/ZobristHash.hpp
 * @author Yuki Kobayashi
 * @~english
 * @brief Management hash value of game state.
 * @~japanese
 * @brief 碁盤の状態を表現したハッシュ値の管理
 */
#ifndef _ZOBRISTHASH_HPP_
#define _ZOBRISTHASH_HPP_

#include <vector>

#include "board/GoBoard.hpp"


/**
 * @~english
 * @brief Default size of hash table.
 * @~japanese
 * @brief ハッシュテーブルのデフォルトサイズ
 */
const unsigned int UCT_HASH_SIZE = 16384;


/**
 * @enum hash
 * @~english
 * @brief Indexes for hash value.
 * @var HASH_PASS
 * Hash value for pass.
 * @var HASH_BLACK
 * Hash value for black stone.
 * @var HASH_WHITE
 * Hash value for white stone.
 * @var HASH_KO
 * Hash value for ko.
 * @~japanese
 * @brief ハッシュのインデックス用
 * @var HASH_PASS
 * パスのハッシュ値
 * @var HASH_BLACK
 * 黒石のハッシュ値
 * @var HASH_WHITE
 * 白石のハッシュ値
 * @var HASH_KO
 * 劫のハッシュ値
 */
enum hash {
  HASH_PASS,
  HASH_BLACK,
  HASH_WHITE,
  HASH_KO,
};


/**
 * @struct node_hash_t
 * @~english
 * @brief Element of hash table.
 * @~japanese
 * @brief ハッシュテーブルの要素
 */
struct node_hash_t {
  /**
   * @~english
   * @brief Hash value.
   * @~japanese
   * @brief 局面のハッシュ値
   */
  unsigned long long hash;

  /**
   * @~english
   * @brief Player's color.
   * @~japanese
   * @brief 手番の色
   */
  int color;

  /**
   * @~english
   * @brief The number of move counts.
   * @~japanese
   * @brief 着手数
   */
  int moves;

  /**
   * @~english
   * @brief Use flag.
   * @~japanese
   * @brief 使用の有無のフラグ
   */
  bool flag;
};


//  UCTのノード用のビット列 (局面の合流なし)
extern unsigned long long move_bit[MAX_RECORDS][BOARD_MAX][HASH_KO + 1];

//  局面を表現するためのビット列
extern unsigned long long hash_bit[BOARD_MAX][HASH_KO + 1];

//  ナカデの形を表現するためのビット列
extern unsigned long long shape_bit[BOARD_MAX];              

//  UCT用ハッシュテーブル
extern node_hash_t *node_hash;

//  UCT用ハッシュテーブルのザイズ
extern unsigned int uct_hash_size; 


//  ハッシュテーブルのサイズの設定
void SetHashSize( const unsigned int new_size );

//  bit列の初期化
void InitializeHash( void );

//  UCTノードのハッシュの初期化
void InitializeUctHash( void );

//  UCTノードのハッシュ情報のクリア
void ClearUctHash( void );

//  古いデータの削除
void DeleteOldHash( const game_info_t *game );

//  未使用のインデックスを探す
unsigned int SearchEmptyIndex( const unsigned long long hash, const int color, const int moves );

//  ハッシュ値に対応するインデックスを返す
unsigned int FindSameHashIndex( const unsigned long long hash, const int color, const int moves );

//  ハッシュ表が埋まっていないか確認
bool CheckRemainingHashSize( void );

//  現局面から到達しないノードを削除
void ClearNotDescendentNodes( std::vector<int> &indexes );

#endif
