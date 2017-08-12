#include <cstdlib>
#include <iostream>
#include <random>

#include "Nakade.h"
#include "ZobristHash.h"

using namespace std;


////////////
//  変数  //
////////////

// 局面を表すためのビット列
unsigned long long hash_bit[BOARD_MAX][HASH_KO + 1];

// ナカデの形を判断するためのビット列
unsigned long long shape_bit[BOARD_MAX];  

// ハッシュ表
node_hash_t *node_hash;

// ハッシュのエントリ数
static unsigned int used;

// ハッシュ表にある最も古いデータが持つ手数
static int oldest_move;

// ハッシュ表のサイズ
unsigned int uct_hash_size = UCT_HASH_SIZE;

// 探索停止するハッシュ表のエントリ数
unsigned int uct_hash_limit = UCT_HASH_SIZE * 9 / 10;

// ハッシュ表に余裕があるかどうかを表すフラグ
bool enough_size;


////////////////////////////////////
//  ハッシュテーブルのサイズの設定  //
////////////////////////////////////
void
SetHashSize( const unsigned int new_size )
{
  if (!(new_size & (new_size - 1))) {
    uct_hash_size = new_size;
    uct_hash_limit = new_size * 9 / 10;
  } else {
    cerr << "Hash size must be 2 ^ n" << endl;
    for (int i = 1; i <= 20; i++) {
      cerr << "2^" << i << ":" << (1 << i) << endl;
    }
    exit(1);
  }

}


/////////////////////////
//  インデックスの取得  //
/////////////////////////
unsigned int
TransHash( const unsigned long long hash )
{
  return ((hash & 0xffffffff) ^ ((hash >> 32) & 0xffffffff)) & (uct_hash_size - 1);
}


/////////////////////
//  bit列の初期化  //
/////////////////////
void
InitializeHash( void )
{
  std::random_device rnd;
  std::mt19937_64 mt(rnd());

  for (int i = 0; i < BOARD_MAX; i++) {  
    hash_bit[i][HASH_PASS]  = mt();
    hash_bit[i][HASH_BLACK] = mt();
    hash_bit[i][HASH_WHITE] = mt();
    hash_bit[i][HASH_KO]    = mt();
    shape_bit[i] = mt();
  }

  node_hash = new node_hash_t[uct_hash_size];

  if (node_hash == NULL) {
    cerr << "Cannot allocate memory" << endl;
    exit(1);
  }

  enough_size = true;

  InitializeNakadeHash();
}


//////////////////////////////////
//  UCTノードのハッシュの初期化  //
//////////////////////////////////
void
InitializeUctHash( void )
{
  oldest_move = 1;
  used = 0;

  for (unsigned int i = 0; i < uct_hash_size; i++) {
    node_hash[i].flag = false;
    node_hash[i].hash = 0;
    node_hash[i].color = 0;
  }
}


/////////////////////////////////////
//  UCTノードのハッシュ情報のクリア  //
/////////////////////////////////////
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


///////////////////////
//  古いデータの削除  //
///////////////////////
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


//////////////////////////////////////
//  未使用のインデックスを探して返す  //
//////////////////////////////////////
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


////////////////////////////////////////////
//  ハッシュ値に対応するインデックスを返す  //
////////////////////////////////////////////
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


////////////////////////////////////////////////
//  ハッシュテーブルに余裕があるかどうかの判定  //
////////////////////////////////////////////////
bool
CheckRemainingHashSize( void )
{
  return enough_size;
}

