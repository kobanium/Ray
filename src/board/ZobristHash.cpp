/**
 * @file src/board/ZobristHash.cpp
 * @author Yuki Kobayashi
 * @~english
 * @brief Hash table for Monte-Carlo tree search.
 * @~japanese
 * @brief モンテカルロ木探索用のハッシュ表
 */
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <random>
#include <vector>

#include "board/ZobristHash.hpp"
#include "feature/Nakade.hpp"


/**
 * @~english
 * @brief Bit strings for MCTS nodes.
 * @~japanese
 * @brief UCTのノード用のビット列
 */
unsigned long long move_bit[MAX_RECORDS][BOARD_MAX][HASH_KO + 1]; 

/**
 * @~english
 * @brief Bit strings for describing positions.
 * @~japanese
 * @brief 局面を表現する用のビット列
 */
unsigned long long hash_bit[BOARD_MAX][HASH_KO + 1];

/**
 * @~english
 * @brief Bit strings for nakade evaluation.
 * @~japanese
 * @brief ナカデ評価用のビット列
 */
unsigned long long shape_bit[BOARD_MAX];  

/**
 * @~english
 * @brief Hash table for MCTS nodes.
 * @~japanese
 * @brief MCTSノード用のハッシュ表
 */
node_hash_t *node_hash;

/**
 * @~english
 * @brief The number of hash entries.
 * @~japanese
 * @brief ハッシュエントリの個数
 */
static unsigned int used;

/**
 * @~english
 * @brief Oldest move in hash entries.
 * @~japanese
 * @brief ハッシュ表にある最も古いデータが持つ手数
 */
static int oldest_move;

/**
 * @~english
 * @brief Hash table size.
 * @~japanese
 * @brief ハッシュ表の大きさ
 */
unsigned int uct_hash_size = UCT_HASH_SIZE;

/**
 * @~english
 * @brief Limitation for the number of hash entries.
 * @~japanese
 * @brief ハッシュエントリの上限値
 */
unsigned int uct_hash_limit = UCT_HASH_SIZE * 9 / 10;

/**
 * @~english
 * @brief Hash table has enough empty entries or not.
 * @~japanese
 * @brief ハッシュ表に余裕があるかどうかを表すフラグ
 */
bool enough_size;


/**
 * @~english
 * @brief Set hash table size.
 * @param[in] new_size Hash table size.
 * @~japanese
 * @brief ハッシュ表のサイズの設定
 * @param[in] new_size 設定するハッシュ表のサイズ
 */
void
SetHashSize( const unsigned int new_size )
{
  if (!(new_size & (new_size - 1))) {
    uct_hash_size = new_size;
    uct_hash_limit = new_size * 9 / 10;
  } else {
    std::cerr << "Hash size must be 2 ^ n" << std::endl;
    for (int i = 1; i <= 20; i++) {
      std::cerr << "2^" << i << ":" << (1 << i) << std::endl;
    }
    exit(1);
  }

}


/**
 * @~english
 * @brief Transform hash value to hash key.
 * @param[in] hash Hash value.
 * @return Hash key.
 * @~japanese
 * @brief ハッシュ値をキーに変換
 * @param[in] hash 局面のハッシュ値
 * @return ハッシュキー
 */
unsigned int
TransHash( const unsigned long long hash )
{
  return ((hash & 0xffffffff) ^ ((hash >> 32) & 0xffffffff)) & (uct_hash_size - 1);
}


/**
 * @~english
 * @brief Initialize bit strings.
 * @~japanese
 * @brief Bit列の初期化
 */
void
InitializeHash( void )
{
  std::random_device rnd;
  std::mt19937_64 mt(rnd());

  for (int i = 0; i < MAX_RECORDS; i++) {
    for (int j = 0; j < BOARD_MAX; j++) {
      move_bit[i][j][HASH_PASS] = mt();
      move_bit[i][j][HASH_BLACK] = mt();
      move_bit[i][j][HASH_WHITE] = mt();
      move_bit[i][j][HASH_KO] = mt();
    }
  }
    
  for (int i = 0; i < BOARD_MAX; i++) {  
    hash_bit[i][HASH_PASS]  = mt();
    hash_bit[i][HASH_BLACK] = mt();
    hash_bit[i][HASH_WHITE] = mt();
    hash_bit[i][HASH_KO]    = mt();
    shape_bit[i] = mt();
  }

  node_hash = new node_hash_t[uct_hash_size];

  if (node_hash == NULL) {
    std::cerr << "Cannot allocate memory" << std::endl;
    exit(1);
  }

  enough_size = true;

  InitializeNakadeHash();
}


/**
 * @~english
 * @brief Initialize hash table.
 * @~japanese
 * @brief ハッシュ表の初期化
 */
void
InitializeUctHash( void )
{
  used = 0;
  enough_size = true;
  oldest_move = 1;
  
  for (unsigned int i = 0; i < uct_hash_size; i++) {
    node_hash[i].flag = false;
    node_hash[i].hash = 0;
    node_hash[i].color = 0;
  }
}


/**
 * @~english
 * @brief Clear hash table entries.
 * @~japanese
 * @brief ハッシュ表のクリア
 */
void
ClearUctHash( void )
{
  used = 0;
  enough_size = true;

  for (unsigned int i = 0; i < uct_hash_size; i++) {
    node_hash[i].flag = false;
    node_hash[i].hash = 0;
    node_hash[i].color = 0;
    node_hash[i].moves = 0;
  }
}


/**
 * @~english
 * @brief Clear all not descendent nodes.
 * @param[in] indexes Indexes of not descendent nodes.
 * @~japanese
 * @brief 現局面の子孫ではないノードの削除
 * @param[in] indexes 子孫ではないノードのインデックス
 */
void
ClearNotDescendentNodes( std::vector<int> &indexes )
{
  auto iter = indexes.begin();

  for (int i = 0; i < (int)uct_hash_size; i++) {
    if (*iter == i) {
      iter++;
    } else if (node_hash[i].flag) {
      node_hash[i].flag = false;
      node_hash[i].hash = 0;
      node_hash[i].color = 0;
      node_hash[i].moves = 0;
      used--;
    }
  }

  enough_size = true;
}


/**
 * @~english
 * @brief Delete old hash entries.
 * @param[in] game Board position data.
 * @~japanese
 * @brief 古いエントリーを削除
 * @param[in] game 局面情報
 */
void
DeleteOldHash( const game_info_t *game )
{
  while (oldest_move < game->moves) {
    for (unsigned int i = 0; i < uct_hash_size; i++) {
      if (node_hash[i].flag && node_hash[i].moves == oldest_move) {
        node_hash[i].flag = false;
        node_hash[i].hash = 0;
        node_hash[i].color = 0;
        node_hash[i].moves = 0;
        used--;
      }
    }
    oldest_move++;
  }

  enough_size = true;
}


/**
 * @~english
 * @brief Search empty index on hash table.
 * @param[in] hash Hash value of current position.
 * @param[in] color Player's color.
 * @param[in] moves The number of move count.
 * @return Unused hash table's index.
 * @~japanese
 * @brief 未使用のインデックスを探して返す
 * @param[in] hash 現局面のハッシュ値
 * @param[in] color 手番の色
 * @param[in] moves 着手数
 * @return 未使用のインデックス
 */
unsigned int
SearchEmptyIndex( const unsigned long long hash, const int color, const int moves )
{
  const unsigned int key = TransHash(hash);
  unsigned int i = key;

  do {
    if (!node_hash[i].flag) {
      node_hash[i].flag = true;
      node_hash[i].hash = hash;
      node_hash[i].moves = moves;
      node_hash[i].color = color;
      used++;
      if (used > uct_hash_limit) enough_size = false;
      return i;
    }
    i++;
    if (i >= uct_hash_size) i = 0;
  } while (i != key);

  return uct_hash_size;
}


/**
 * @~english
 * @brief Search element which has same hash value.
 * @param[in] hash Hash value of current position.
 * @param[in] color Player's color.
 * @param[in] moves The number of move count.
 * @return Index on hash table.
 * @~japanese
 * @brief ハッシュ値に対応するインデックスを返す
 * @param[in] hash 局面のハッシュ値
 * @param[in] color 手番の色
 * @param[in] moves 着手数
 * @return ハッシュ表のインデックス
 */
unsigned int
FindSameHashIndex( const unsigned long long hash, const int color, const int moves)
{
  const unsigned int key = TransHash(hash);
  unsigned int i = key;

  do {
    if (!node_hash[i].flag) {
      return uct_hash_size;
    } else if (node_hash[i].hash == hash &&
               node_hash[i].color == color &&
               node_hash[i].moves == moves) {
      return i;
    }
    i++;
    if (i >= uct_hash_size) i = 0;
  } while (i != key);

  return uct_hash_size;
}


/**
 * @~english
 * @brief Check enough empty hash table size. 
 * @return Check result.
 * @~japanese
 * @brief ハッシュ表に十分な空きがあるか判定
 * @return 判定結果
 */
bool
CheckRemainingHashSize( void )
{
  return enough_size;
}

