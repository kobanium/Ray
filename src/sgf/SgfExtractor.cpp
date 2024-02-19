/**
 * @file src/sgf/SgfExtractor.cpp
 * @author Yuki Kobayashi
 * @~english
 * @brief SGF file loader.
 * @~japanese
 * @brief SGFファイルローダ
 */
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>

#include "board/GoBoard.hpp"
#include "common/Message.hpp"
#include "sgf/SgfExtractor.hpp"


// SGFの座標を数値に変換
static int ParsePosition( const char c );
// 盤の大きさの抽出
static int GetSize( SGF_record_t *kifu, const char *sgf_text, const int cursor );
// 結果の抽出
static int GetResult( SGF_record_t *kifu, const char *sgf_text, const int cursor );
// 着手の抽出
static int GetMove( SGF_record_t *kifu, const char *sgf_text, const int cursor );
// 置き石の数の抽出
static int GetHandicaps( SGF_record_t *kifu, const char *sgf_text, const int cursor );
// 置き石の座標の抽出
static int GetHandicapPosition( SGF_record_t *kifu, const char *sgf_text, const int cursor, const int color );
// コミの抽出
static int GetKomi( SGF_record_t *kifu, const char *sgf_text, const int cursor );
// 対局者の名前を抽出
static int GetPlayerName( SGF_record_t *kifu, const char *sgf_text, const int cursor, const int color );
// 無視する情報を飛ばす処理
static int SkipData( const char *sgf_text, const int cursor );


/**
 * @~english
 * @brief Check skipped charactor for SGF.
 * @param[in] c Input charactor.
 * @return Skipped charactor flag.
 * @~japanese
 * @brief 空白文字かを判定
 * @param[in] c 入力文字
 * @return 空白文字判定結果
 */
static inline bool IsSgfSpace( const char c ) {
  return c == '\n' || c == '\r' || c == ' ' || c == '\t';
}


/**
 * @~english
 * @brief Get move from kifu data.
 * @param[in] kifu Kifu data.
 * @param[in] n Index
 * @return Coordinate of move.
 * @~japanese
 * @brief 棋譜データから着手の座標を取得
 * @param[in] kifu 棋譜データ
 * @param[in] n 手番
 * @return 着手の座標
 */
int
GetKifuMove( const SGF_record_t *kifu, const int n )
{
  if (kifu->move_x[n] == 0) {
    return PASS;
  } else {
    return POS(kifu->move_x[n] + (OB_SIZE - 1), kifu->move_y[n] + (OB_SIZE - 1));
  }
}


/**
 * @~english
 * @brief Get handicap move from kifu data.
 * @param[in] kifu Kifu data.
 * @param[in] n Index
 * @return Coordinate of a handicap stone.
 * @~japanese
 * @brief 棋譜データから置き石の座標を取得
 * @param[in] kifu 棋譜データ
 * @param[in] n 手番
 * @return 置き石の座標
 */
int
GetHandicapStone( const SGF_record_t *kifu, const int n )
{
  if (kifu->handicap_x[n] == 0) {
    return PASS;
  } else {
    return POS(kifu->handicap_x[n] + (OB_SIZE - 1), kifu->handicap_y[n] + (OB_SIZE - 1));
  }
}


/**
 * @~english
 * @brief Read SGF file.
 * @param[in] file_name SGF file name.
 * @param[out] kifu Kifu data.
 * @~japanese
 * @brief SGFファイルの読み込み
 * @param[in] file_name SGFファイル名
 * @param[out] kifu 棋譜データ
 */
int
ExtractKifu( const char *file_name, SGF_record_t *kifu )
{
  FILE *fp;
  char sgf_text[100005], buffer[10000];
  int cursor = 0;
  
#if defined (_WIN32)
  errno_t err;

  if((err = fopen_s(&fp, file_name, "r")) != 0) {
    printf("Cannot open this file (%s)!!\n", file_name);
    return -1;
  } 
#else
  if((fp = fopen(file_name, "r")) == NULL) {
    printf("Cannot open this file (%s)!!\n", file_name);
    return -1;
  }
#endif  

  memset(sgf_text, 0, sizeof(char) * 100005);
  
  while (fgets(buffer, 10000, fp) != NULL) {
#if defined (_WIN32)
    strcat_s(sgf_text, 10000, buffer);
#else
    strncat(sgf_text, buffer, 10000);
#endif
  }
  fclose(fp);
  
  kifu->board_size = 19;
  kifu->result = R_UNKNOWN;
  kifu->moves = 0;
  kifu->handicaps = 0;
  kifu->handicap_stones = 0;
  kifu->komi = 0.0;
  kifu->start_color = S_BLACK;
  memset(kifu->move_x, 0, sizeof(kifu->move_x));
  memset(kifu->move_y, 0, sizeof(kifu->move_y));
  memset(kifu->black_name, 0, sizeof(kifu->black_name));
  memset(kifu->white_name, 0, sizeof(kifu->white_name));
  memset(kifu->handicap_x, 0, sizeof(kifu->handicap_x));
  memset(kifu->handicap_y, 0, sizeof(kifu->handicap_y));
  memset(kifu->handicap_color, 0, sizeof(kifu->handicap_color));
  
  while ((cursor < 100000) && (sgf_text[cursor] != '\0')) {
    if (IsSgfSpace(sgf_text[cursor]) ||
        sgf_text[cursor] == ';'  ||
        sgf_text[cursor] == '('  ||
        sgf_text[cursor] == ')') { 
      while (cursor < 100000 - 1) {
        cursor++;
        if (!IsSgfSpace(sgf_text[cursor]) &&
            sgf_text[cursor] != ';'  &&
            sgf_text[cursor] != '('  &&
            sgf_text[cursor] != ')') 
          break;
      }
    }    
    if (cursor >= 100000){
      break;
    }

    // GetSize             : 碁盤のサイズの抽出 
    // GetResult           : 結果の抽出 
    // GetHandicaps        : 置き石の個数の抽出 
    // GetHandicapPosition : 置き石の座標
    // GetMove             : 着手の抽出
    // GetKomi             : コミの抽出
    // GetPlayerName       : 対局者の名前の抽出
    if (strncmp(&sgf_text[cursor], "SZ[", 3) == 0) {
      cursor = GetSize(kifu, sgf_text, cursor);
    } else if (strncmp(&sgf_text[cursor], "RE[", 3) == 0) {
      cursor = GetResult(kifu, sgf_text, cursor);
    } else if (strncmp(&sgf_text[cursor], "HA[", 3) == 0) {
      cursor = GetHandicaps(kifu, sgf_text, cursor);
    } else if (strncmp(&sgf_text[cursor], "AB[", 3) == 0) {
      cursor = GetHandicapPosition(kifu, sgf_text, cursor, S_BLACK);
    } else if (strncmp(&sgf_text[cursor], "AW[", 3) == 0) {
      cursor = GetHandicapPosition(kifu, sgf_text, cursor, S_WHITE);
    } else if (strncmp(&sgf_text[cursor], "KM[", 3) == 0) {
      cursor = GetKomi(kifu, sgf_text, cursor);
    } else if (strncmp(&sgf_text[cursor], "PB[", 3) == 0) {
      cursor = GetPlayerName(kifu, sgf_text, cursor, S_BLACK);
    } else if (strncmp(&sgf_text[cursor], "PW[", 3) == 0) {
      cursor = GetPlayerName(kifu, sgf_text, cursor, S_WHITE);
    } else if (strncmp(&sgf_text[cursor],  "B[", 2) == 0) {
      cursor = GetMove(kifu, sgf_text, cursor);
    } else if (strncmp(&sgf_text[cursor],  "W[", 2) == 0) {
      cursor = GetMove(kifu, sgf_text, cursor);
    } else if ((strncmp(&sgf_text[cursor], "GM[", 3) == 0) ||
               (strncmp(&sgf_text[cursor], "FF[", 3) == 0) ||
               (strncmp(&sgf_text[cursor], "DT[", 3) == 0) ||
               (strncmp(&sgf_text[cursor], "PC[", 3) == 0) ||
               (strncmp(&sgf_text[cursor], "RU[", 3) == 0) ||
               (strncmp(&sgf_text[cursor], "CA[", 3) == 0) ||
               (strncmp(&sgf_text[cursor], "TM[", 3) == 0) ||
               (strncmp(&sgf_text[cursor], "OT[", 3) == 0) ||
               (strncmp(&sgf_text[cursor], "TB[", 3) == 0) ||
               (strncmp(&sgf_text[cursor], "TW[", 3) == 0) ||
               (strncmp(&sgf_text[cursor], "WR[", 3) == 0) ||
               (strncmp(&sgf_text[cursor], "BR[", 3) == 0)) {      
      // 無視するデータ
      cursor = SkipData(sgf_text, cursor);
    }
    cursor++;   // 文字を一つ進める
  }

  return 0;
}


/**
 * @~english
 * @brief Get board size.
 * @param[in] kifu Kifu data.
 * @param[in] sgf_text SGF text.
 * @param[in] cursor Current cursor for SGF text.
 * @return Next cursor position.
 * @~japanese
 * @brief 盤の大きさの抽出
 * @param[in] kifu 棋譜データ
 * @param[in] sgf_text SGFファイルを読み込んだテキスト
 * @param[in] cursor 現在のカーソルの位置
 * @return 次のカーソルの位置
 */
static int
GetSize( SGF_record_t *kifu, const char *sgf_text, const int cursor )
{
  int tmp_cursor = 3;
  char size[10];
  
  memset(size, 0, sizeof(char) * 10);

  while ((cursor + tmp_cursor < 100000) && (sgf_text[cursor + tmp_cursor] != ']')) tmp_cursor++;
  
  for (int i = 0; i < tmp_cursor - 3; i++) {
    if (cursor + i + 3 < 100000){
      size[i] = sgf_text[cursor + i + 3];
    }
  }
  kifu->board_size = atoi(size);
  while ((cursor + tmp_cursor < 100000) && (sgf_text[cursor + tmp_cursor] != ']')) tmp_cursor++;  
  return cursor + tmp_cursor;
}


/**
 * @~english
 * @brief Get game result.
 * @param[in] kifu Kifu data.
 * @param[in] sgf_text SGF text.
 * @param[in] cursor Current cursor for SGF text.
 * @return Next cursor position.
 * @~japanese
 * @brief 結果の抽出
 * @param[in] kifu 棋譜データ
 * @param[in] sgf_text SGFファイルを読み込んだテキスト
 * @param[in] cursor 現在のカーソルの位置
 * @return 次のカーソルの位置
 */
static int
GetResult( SGF_record_t *kifu, const char *sgf_text, const int cursor )
{
  int tmp_cursor = 3;

  while ((cursor + tmp_cursor < 100000) && (sgf_text[cursor + tmp_cursor] != ']')) tmp_cursor++;  

  if (cursor + 3 < 100000){
    switch (sgf_text[cursor + 3]) {
    case 'B':
      kifu->result = R_BLACK;
      break;
    case 'W':
      kifu->result = R_WHITE;
      break;
    case '0':
    case 'D':
      kifu->result = R_JIGO;
      break;
    default:
      break;
    }
  }
  while ((cursor + tmp_cursor < 100000) && (sgf_text[cursor + tmp_cursor] != ']')) tmp_cursor++;  
  return cursor + tmp_cursor;
}


/**
 * @~english
 * @brief Get move coordinate.
 * @param[in] kifu Kifu data.
 * @param[in] sgf_text SGF text.
 * @param[in] cursor Current cursor for SGF text.
 * @return Next cursor position.
 * @~japanese
 * @brief 着手の抽出
 * @param[in] kifu 棋譜データ
 * @param[in] sgf_text SGFファイルを読み込んだテキスト
 * @param[in] cursor 現在のカーソルの位置
 * @return 次のカーソルの位置
 */
static int
GetMove( SGF_record_t *kifu, const char *sgf_text, const int cursor )
{
  int tmp_cursor = 0;

  if (cursor + 3 < 100000){
    if (kifu->moves == 0) {
      if (sgf_text[cursor] == 'B') {
        kifu->start_color = S_BLACK;
      } else {
        kifu->start_color = S_WHITE;
      }
    }
  
    if (sgf_text[cursor + 2] == ']') {
      kifu->move_x[kifu->moves] = PASS;
      kifu->move_y[kifu->moves] = PASS;
      tmp_cursor = 2;
    } else {
      kifu->move_x[kifu->moves] = ParsePosition(sgf_text[cursor + 2]);
      kifu->move_y[kifu->moves] = ParsePosition(sgf_text[cursor + 3]);
      tmp_cursor = 4;
      while ((cursor + tmp_cursor < 100000) && (sgf_text[cursor + tmp_cursor] != ']')) tmp_cursor++;        
    }

    kifu->moves++;
  }

  return cursor + tmp_cursor;
}


/**
 * @~english
 * @brief Get the number of handicap stones.
 * @param[in] kifu Kifu data.
 * @param[in] sgf_text SGF text.
 * @param[in] cursor Current cursor for SGF text.
 * @return Next cursor position.
 * @~japanese
 * @brief 置き石の個数の抽出
 * @param[in] kifu 棋譜データ
 * @param[in] sgf_text SGFファイルを読み込んだテキスト
 * @param[in] cursor 現在のカーソルの位置
 * @return 次のカーソルの位置
 */
static int
GetHandicaps( SGF_record_t *kifu, const char *sgf_text, const int cursor )
{
  int tmp_cursor = 3;
  char handicaps[10] = {0};

  while ((cursor + tmp_cursor < 100000) && (sgf_text[cursor + tmp_cursor] != ']')) tmp_cursor++;  
  
  for (int i = 0; i < tmp_cursor - 3; i++) {
    if (cursor + i + 3 < 100000){
      handicaps[i] = sgf_text[cursor + i + 3];
    }
  }
  
  kifu->handicaps = atoi(handicaps);
  
  return cursor + tmp_cursor;
}


/**
 * @~english
 * @brief Get handicap's coordinate.
 * @param[in] kifu Kifu data.
 * @param[in] sgf_text SGF text.
 * @param[in] cursor Current cursor for SGF text.
 * @param[in] color Stone color.
 * @return Next cursor position.
 * @~japanese
 * @brief 置き石の座標の抽出
 * @param[in] kifu 棋譜データ
 * @param[in] sgf_text SGFファイルを読み込んだテキスト
 * @param[in] cursor 現在のカーソルの位置
 * @param[in] color 石の色
 * @return 次のカーソルの位置
 */
static int
GetHandicapPosition( SGF_record_t *kifu, const char *sgf_text, const int cursor, const int color )
{
  int tmp_cursor = 3;

  while ((cursor + tmp_cursor < 100000) &&
         ((sgf_text[cursor + tmp_cursor] == '[') ||
          (sgf_text[cursor + tmp_cursor] == ']') ||
          IsSgfSpace(sgf_text[cursor + tmp_cursor]) ||
          (('a' <= sgf_text[cursor + tmp_cursor]) && 
           (sgf_text[cursor + tmp_cursor] <= 's')))) tmp_cursor++;

  if (sgf_text[cursor + tmp_cursor] != ']'){
    tmp_cursor--;
  }
  
  int handicaps = 0;

  for (int cur = cursor + 2; cur < cursor + tmp_cursor;) {
    while (IsSgfSpace(sgf_text[cur])) cur++;
    if (sgf_text[cur] != '[')
      return cursor + tmp_cursor;
    cur++; while (IsSgfSpace(sgf_text[cur])) cur++;
    kifu->handicap_x[handicaps + kifu->handicap_stones] = ParsePosition(sgf_text[cur]);
    cur++; while (IsSgfSpace(sgf_text[cur])) cur++;
    kifu->handicap_y[handicaps + kifu->handicap_stones] = ParsePosition(sgf_text[cur]);
    cur++; while (IsSgfSpace(sgf_text[cur])) cur++;
    if (sgf_text[cur] != ']')
      return cursor + tmp_cursor;
    cur++;
    kifu->handicap_color[handicaps + kifu->handicap_stones] = color;
    handicaps++;
  }

  kifu->handicap_stones += handicaps;
  
  return cursor + tmp_cursor;
}


/**
 * @~english
 * @brief Get komi value.
 * @param[in] kifu Kifu data.
 * @param[in] sgf_text SGF text.
 * @param[in] cursor Current cursor for SGF text.
 * @return Next cursor position.
 * @~japanese
 * @brief コミの値の抽出
 * @param[in] kifu 棋譜データ
 * @param[in] sgf_text SGFファイルを読み込んだテキスト
 * @param[in] cursor 現在のカーソルの位置
 * @return 次のカーソルの位置
 */
static int
GetKomi( SGF_record_t *kifu, const char *sgf_text, const int cursor )
{
  int tmp_cursor = 3;
  char komi[10] = {0};
  
  while ((cursor + tmp_cursor < 100000) && (sgf_text[cursor + tmp_cursor] != ']')) tmp_cursor++;
  
  for (int i = 0; i < tmp_cursor - 3; i++) {
    if (cursor + i + 3 < 100000){
      komi[i] = sgf_text[cursor + i + 3];
    }
  }
  kifu->komi = atof(komi);
  
  while ((cursor + tmp_cursor < 100000) && (sgf_text[cursor + tmp_cursor] != ']')) tmp_cursor++;
  
  return cursor + tmp_cursor;
}


/**
 * @~english
 * @brief Get match name.
 * @param[in] kifu Kifu data.
 * @param[in] sgf_text SGF text.
 * @param[in] cursor Current cursor for SGF text.
 * @param[in] color Player's color.
 * @return Next cursor position.
 * @~japanese
 * @brief 対局の名前の抽出
 * @param[in] kifu 棋譜データ
 * @param[in] sgf_text SGFファイルを読み込んだテキスト
 * @param[in] cursor 現在のカーソルの位置
 * @param[in] color 手番の色
 * @return 次のカーソルの位置
 */
static int
GetPlayerName( SGF_record_t *kifu, const char *sgf_text, const int cursor, const int color )
{
  int tmp_cursor = 0;

  while ((cursor + tmp_cursor < 100000) && (sgf_text[cursor + tmp_cursor] != ']')) tmp_cursor++;

  if (color == S_BLACK) {
    for (int i = 0; i < tmp_cursor - 3; i++) {
      if (cursor + i + 3 < 100000){
        kifu->black_name[i] = sgf_text[cursor + i + 3];
      }
    }
  } else {
    for (int i = 0; i < tmp_cursor - 3; i++) {
      if (cursor + i + 3 < 100000){
        kifu->white_name[i] = sgf_text[cursor + i + 3];
      }
    }
  }

  while ((cursor + tmp_cursor < 100000) && (sgf_text[cursor + tmp_cursor] != ']')) tmp_cursor++;  
  return cursor + tmp_cursor;
}


/**
 * @~english
 * @brief Skip ignored data.
 * @param[in] sgf_text SGF text.
 * @param[in] cursor Current cursor for SGF text.
 * @return Next cursor position.
 * @~japanese
 * @brief 無視する情報を飛ばす処理
 * @param[in] sgf_text SGFファイルを読み込んだテキスト
 * @param[in] cursor 現在のカーソルの位置
 * @return 次のカーソルの位置
 */
static int
SkipData( const char *sgf_text, const int cursor )
{
  int tmp_cursor = 3;
  
  while ((cursor + tmp_cursor < 100000) &&
         (sgf_text[cursor + tmp_cursor] != ']')) {
    tmp_cursor++;
  }
  
  return cursor + tmp_cursor;
}


/**
 * @~english
 * @brief Convert SGF style coordinate to internal expression.
 * @param[in] c Charactor of coordinate.
 * @return Coordinate with internal expression. 
 * @~japanese
 * @brief SGFの座標を数値に変換
 * @param[in] c SGF形式の座標
 * @return 内部表現の座標
 */
static int
ParsePosition( const char c )
{
  switch (c) {
    case 'a': return 1;
    case 'b': return 2;
    case 'c': return 3;
    case 'd': return 4;
    case 'e': return 5;
    case 'f': return 6;
    case 'g': return 7;
    case 'h': return 8;
    case 'i': return 9;
    case 'j': return 10;
    case 'k': return 11;
    case 'l': return 12;
    case 'm': return 13;
    case 'n': return 14;
    case 'o': return 15;
    case 'p': return 16;
    case 'q': return 17;
    case 'r': return 18;
    case 's': return 19;
    default : return PASS;
  }
}
