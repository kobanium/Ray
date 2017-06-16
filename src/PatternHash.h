#ifndef _PATTERNHASH_H_
#define _PATTERNHASH_H_

#include "GoBoard.h"
#include "Pattern.h"

const int HASH_MAX = 1048576; // 2^20
const int BIT_MAX = 60;

#define TRANS20(hash)	(((hash&0xFFFFFFFF)^((hash>>32)&0xFFFFFFFF))&0xFFFFF)

// パターン
struct pattern_hash_t {
  unsigned long long list[MD_MAX + MD_LARGE_MAX];
};

// インデックス 
struct index_hash_t {
  unsigned long long hash;
  int index;
};

////////////
//  関数  //
////////////

//  パターンのハッシュ関数
void PatternHash( const pattern_t *pat, pattern_hash_t *hash_pat );

//  インデックスを探索
int SearchIndex( const index_hash_t *index, const unsigned long long hash );

#endif	// _PATTTERNHASH_H_ 
