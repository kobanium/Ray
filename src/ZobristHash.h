#ifndef _ZOBRISTHASH_H_
#define _ZOBRISTHASH_H_

#include "GoBoard.h"

////////////
//  定数  //
////////////
enum hash{
  HASH_PASS,
  HASH_BLACK,
  HASH_WHITE,
  HASH_KO,
};

const unsigned int UCT_HASH_SIZE = 16384;

//////////////
//  構造体  //
//////////////

typedef struct {
  unsigned long long hash;
  int color;
  int moves;
  bool flag;
} node_hash_t;


////////////
//  変数  //
////////////

//  bit列
extern unsigned long long hash_bit[BOARD_MAX][HASH_KO + 1];  
extern unsigned long long shape_bit[BOARD_MAX];              

//  UCT用ハッシュテーブル
extern node_hash_t *node_hash;

//  UCT用ハッシュテーブルのザイズ
extern unsigned int uct_hash_size; 

////////////
//  関数  //
////////////

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

#endif
