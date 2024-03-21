/**
 * @file include/pattern/Pattern.hpp
 * @author Yuki Kobayashi
 * @~english
 * @brief Management of neighborhood stones' pattern.
 * @~japanese
 * @brief 近傍の石の配置のパターンの管理
 */
#ifndef _PATTERN_HPP_
#define _PATTERN_HPP_

/**
 * @~english
 * @brief The number of MD2 Patterns
 * @~japanese
 * @brief MD2パターンの最大数
 */
constexpr int MD2_MAX = 16777216;

/**
 * @~english
 * @brief The number of 3x3 Patterns
 * @~japanese
 * @brief 3x3パターンの最大数
 */
constexpr int PAT3_MAX = 65536;

/**
 * @~english
 * @brief The number of valid MD2 Patterns
 * @~japanese
 * @brief MD2パターンの数(有効なもののみ)
 */
constexpr int MD2_LIMIT = 1060624;

/**
 * @~english
 * @brief The number of valid 3x3 Patterns 
 * @~japanese
 * @brief 3x3パターンの数(有効なもののみ)
 */
constexpr int PAT3_LIMIT = 4468;


/**
 * @enum MD
 * @~english
 * @brief Indexes of the data storing the pattern's bitstring.
 * @var MD_2
 * MD2 Pattern (including 3x3 Pattern)
 * @var MD_3
 * MD3 Pattern
 * @var MD_4
 * MD4 Pattern
 * @var MD_MAX
 * Sentinel Status
 * @~japanese
 * @brief パターンのビット列を格納しているデータのインデックス
 * @var MD_2
 * MD2パターン
 * @var MD_3
 * MD3パターン
 * @var MD_4
 * MD4パターン
 * @var MD_MAX
 * 番兵
 */
enum MD {
  MD_2,
  MD_3,
  MD_4,
  MD_MAX
};

/**
 * @enum LARGE_MD
 * @~english
 * @brief Indexes of the data storing the pattern's bitstring.
 * @var MD_5
 * MD5 Pattern
 * @var MD_LARGE_MAX
 * Sentinal status
 * @~japanese
 * @brief パターンのビット列を格納しているデータのインデックス
 * @var MD_5
 * MD5パターン
 * @var MD_LARGE_MAX
 * 番兵
 */
enum LARGE_MD {
  MD_5,
  MD_LARGE_MAX
};


/**
 * @~english
 * @brief Pattern of neighborhood stones.
 * @~japanese
 * @brief 近傍の石のパターン
 */
struct pattern_t {
  /**
   * @~english
   * @brief Pattern data.
   * @~japanese
   * @brief 配石パターン
  */
  unsigned int list[MD_MAX];

  /**
   * @~english
   * @brief Large pattern data.
   * @~japanese
   * @brief 大きい配石パターン
   */
  unsigned long long large_list[MD_LARGE_MAX];
};


//  初期設定
void ClearPattern( pattern_t *pat );

//  更新
void UpdatePat3Empty( pattern_t *pat, const int pos );
void UpdatePat3Stone( pattern_t *pat, const int color, const int pos );
void UpdateMD2Empty( pattern_t *pat, const int pos );
void UpdateMD2Stone( pattern_t *pat, const int color, const int pos );
void UpdatePatternEmpty( pattern_t *pat, const int pos );
void UpdatePatternStone( pattern_t *pat, const int color, const int pos );

//  同一パターン
void Pat3Transpose8( const unsigned int pat3, unsigned int *transp );
void Pat3Transpose16( const unsigned int pat3, unsigned int *transp );
void MD2Transpose8( const unsigned int md2, unsigned int *transp );
void MD2Transpose16( const unsigned int md2, unsigned int *transp );
void MD3Transpose8( const unsigned int md3, unsigned int *transp );
void MD3Transpose16( const unsigned int md3, unsigned int *transp );
void MD4Transpose8( const unsigned int md4, unsigned int *transp );
void MD4Transpose16( const unsigned int md4, unsigned int *transp );
void MD5Transpose8( const unsigned long long md5, unsigned long long *transp );
void MD5Transpose16( const unsigned long long md5, unsigned long long *transp );

//  色反転
unsigned int Pat3Reverse( const unsigned int pat3 );
unsigned int MD2Reverse( const unsigned int md2 );
unsigned int MD3Reverse( const unsigned int md3 );
unsigned int MD4Reverse( const unsigned int md4 );
unsigned long long MD5Reverse( const unsigned long long md5 );

//  上下対称
unsigned int Pat3VerticalMirror( const unsigned int pat3 );
unsigned int MD2VerticalMirror( const unsigned int md2 );
unsigned int MD3VerticalMirror( const unsigned int md3 );
unsigned int MD4VerticalMirror( const unsigned int md4 );
unsigned long long MD5VerticalMirror( const unsigned long long md5 );

//  左右対称
unsigned int Pat3HorizontalMirror( const unsigned int pat3 );
unsigned int MD2HorizontalMirror( const unsigned int md2 );
unsigned int MD3HorizontalMirror( const unsigned int md3 );
unsigned int MD4HorizontalMirror( const unsigned int md4 );
unsigned long long MD5HorizontalMirror( const unsigned long long md5 );

//  90度回転
unsigned int Pat3Rotate90( const unsigned int pat3 );
unsigned int MD2Rotate90( const unsigned int md2 );
unsigned int MD3Rotate90( const unsigned int md3 );
unsigned int MD4Rotate90( const unsigned int md4 );
unsigned long long MD5Rotate90( const unsigned long long md5 );

//  パターンを返す
unsigned int Pat3( const pattern_t *pat, const int pos );
unsigned int MD2( const pattern_t *pat, const int pos );
unsigned int MD3( const pattern_t *pat, const int pos );
unsigned int MD4( const pattern_t *pat, const int pos );
unsigned long long MD5( const pattern_t *pat, const int pos );

//  表示
void DisplayInputPat3( const unsigned int pat3 );
void DisplayInputMD2( const unsigned int md2 );
void DisplayInputMD3( const unsigned int md3 );
void DisplayInputMD4( const unsigned int md4 );
void DisplayInputMD5( const unsigned long long md5 );
void DisplayInputPattern( const pattern_t *pattern, const int size );

#endif
