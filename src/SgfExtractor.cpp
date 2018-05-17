#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>

#include "GoBoard.h"
#include "Message.h"
#include "SgfExtractor.h"

// SGFの座標を数値に変換
static int ParsePosition( char c );
// 盤の大きさの抽出
static int GetSize( SGF_record_t *kifu, char *sgf_text, int cursor );
// 結果の抽出
static int GetResult( SGF_record_t *kifu, char *sgf_text, int cursor );
// 着手の抽出
static int GetMove( SGF_record_t *kifu, char *sgf_text, int cursor );
// 置き石の数の抽出
static int GetHandicaps( SGF_record_t *kifu, char *sgf_text, int cursor );
// 置き石の座標の抽出
static int GetHandicapPosition( SGF_record_t *kifu, char *sgf_text, int cursor );
// コミの抽出
static int GetKomi( SGF_record_t *kifu, char *sgf_text, int cursor );
// 対局者の名前を抽出
static int GetPlayerName( SGF_record_t *kifu, char *sgf_text, int cursor, int color );
// 無視する情報を飛ばす処理
static int SkipData( SGF_record_t *kifu, char *sgf_text, int cursor );


//////////////////
//  着手の抽出  //
//////////////////
int
GetKifuMove( const SGF_record_t *kifu, int n )
{
  int pos;

  if (kifu->move_x[n] == 0) {
    pos = PASS;
  } else {
    pos = POS(kifu->move_x[n] + (OB_SIZE - 1), kifu->move_y[n] + (OB_SIZE - 1));
  }
  return pos;
}


////////////////////////////
//  SGFファイルの読み込み  //
////////////////////////////
void
ExtractKifu( const char *file_name, SGF_record_t *kifu )
{
  FILE *fp;
  char sgf_text[100005], buffer[10000];
  int cursor = 0;
  
#if defined (_WIN32)
  errno_t err;

  if((err = fopen_s(&fp, file_name, "r")) != 0) {
    printf("Can't open this file!!\n");
    exit(1);
  } 
#else
  if((fp = fopen(file_name, "r")) == NULL) {
    printf("Can't open this file!!\n");
    exit(1);
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
  kifu->komi = 0.0;
  memset(kifu->move_x, 0, sizeof(kifu->move_x));
  memset(kifu->move_y, 0, sizeof(kifu->move_y));
  memset(kifu->black_name, 0, sizeof(kifu->black_name));
  memset(kifu->white_name, 0, sizeof(kifu->white_name));

  
  while ((cursor < 100000) && (sgf_text[cursor] != '\0')) {
    if (sgf_text[cursor] == '\n' ||
	sgf_text[cursor] == '\r' ||				
	sgf_text[cursor] == ' '  ||
	sgf_text[cursor] == ';'  ||
	sgf_text[cursor] == '('  ||
	sgf_text[cursor] == ')'  ||
	sgf_text[cursor] == '\t') { 
      while (cursor < 100000 - 1) {
	cursor++;
	if (sgf_text[cursor] != '\n' &&
	    sgf_text[cursor] != '\r' &&	    
	    sgf_text[cursor] != ' '  &&
	    sgf_text[cursor] != ';'  &&
            sgf_text[cursor] != '('  &&
	    sgf_text[cursor] != ')'  &&
	    sgf_text[cursor] != '\t') 
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
    if (strncmp(&sgf_text[cursor], "SZ[", 3) == 0) cursor = GetSize(kifu, sgf_text, cursor);
    if (strncmp(&sgf_text[cursor], "RE[", 3) == 0) cursor = GetResult(kifu, sgf_text, cursor);
    if (strncmp(&sgf_text[cursor], "HA[", 3) == 0) cursor = GetHandicaps(kifu, sgf_text, cursor);
    if (strncmp(&sgf_text[cursor], "AB[", 3) == 0) cursor = GetHandicapPosition(kifu, sgf_text, cursor);
    if (strncmp(&sgf_text[cursor],  "B[", 2) == 0) cursor = GetMove(kifu, sgf_text, cursor);
    if (strncmp(&sgf_text[cursor],  "W[", 2) == 0) cursor = GetMove(kifu, sgf_text, cursor);
    if (strncmp(&sgf_text[cursor], "KM[", 3) == 0) cursor = GetKomi(kifu, sgf_text, cursor);
    if (strncmp(&sgf_text[cursor], "PB[", 3) == 0) cursor = GetPlayerName(kifu, sgf_text, cursor, S_BLACK);
    if (strncmp(&sgf_text[cursor], "PW[", 3) == 0) cursor = GetPlayerName(kifu, sgf_text, cursor, S_WHITE);

    // 無視するデータ
    if ((strncmp(&sgf_text[cursor], "GM[", 3) == 0) ||
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
      cursor = SkipData(kifu, sgf_text, cursor);
    }
    cursor++;   // 文字を一つ進める
  }
  
  //PrintSgfInformation(kifu);
  //getchar();
}


///////////////////////
//  盤の大きさの抽出  //
///////////////////////
static int
GetSize( SGF_record_t *kifu, char *sgf_text, int cursor )
{
  int tmp_cursor = 3;
  char size[10];
  memset(size, 0, sizeof(char)*10);

  while ((cursor+tmp_cursor < 100000) && (sgf_text[cursor + tmp_cursor] != ']')) tmp_cursor++;
  
  for (int i = 0; i < tmp_cursor - 3; i++) {
    if (cursor + i + 3 < 100000){
      size[i] = sgf_text[cursor + i + 3];
    }
  }
  kifu->board_size = atoi(size);
  while ((cursor+tmp_cursor < 100000) && (sgf_text[cursor + tmp_cursor] != ']')) tmp_cursor++;  
  return cursor + tmp_cursor;
}


/////////////////
//  結果の抽出  //
/////////////////
static int
GetResult( SGF_record_t *kifu, char *sgf_text, int cursor )
{
  int tmp_cursor = 3;
  while ((cursor+tmp_cursor < 100000) && (sgf_text[cursor + tmp_cursor] != ']')) tmp_cursor++;  
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
  while ((cursor+tmp_cursor < 100000) && (sgf_text[cursor + tmp_cursor] != ']')) tmp_cursor++;  
  return cursor + tmp_cursor;
}


/////////////////
//  着手の抽出  //
/////////////////
static int
GetMove( SGF_record_t *kifu, char *sgf_text, int cursor )
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


///////////////////////
//  置き石の数の抽出  //
///////////////////////
static int
GetHandicaps( SGF_record_t *kifu, char *sgf_text, int cursor )
{
  int tmp_cursor = 3;
  char handicaps[10] = {0};
  while ((cursor+tmp_cursor < 100000) && (sgf_text[cursor + tmp_cursor] != ']')) tmp_cursor++;  
  
  for (int i = 0; i < tmp_cursor - 3; i++) {
    if (cursor + i + 3 < 100000){
      handicaps[i] = sgf_text[cursor + i + 3];
    }
  }
  
  kifu->handicaps = atoi(handicaps);
  
  return cursor + tmp_cursor;
}


/////////////////////////
//  置き石の座標の抽出  //
/////////////////////////
static int
GetHandicapPosition( SGF_record_t *kifu, char *sgf_text, int cursor )
{
  int tmp_cursor = 3;
  int handicaps = 0;

  while ((cursor+tmp_cursor < 100000) && ((sgf_text[cursor + tmp_cursor] == '[') || (sgf_text[cursor + tmp_cursor] == ']') || (('a' <= sgf_text[cursor + tmp_cursor]) && (sgf_text[cursor + tmp_cursor] <= 's')))) tmp_cursor++;
  if (sgf_text[cursor + tmp_cursor] != ']'){
    tmp_cursor--;
  }
  
  handicaps = (tmp_cursor - 4) / 4;

  for (int i = 0; i < handicaps; i++) {
    if (cursor + 3 + i * 4 < 100000){
      kifu->handicap_x[i] = ParsePosition(sgf_text[cursor + 3 + i * 4]);
      kifu->handicap_y[i] = ParsePosition(sgf_text[cursor + 4 + i * 4]);
    }
  }
  
  return cursor + tmp_cursor;
}

//////////////////
//  コミの抽出  //
//////////////////
static int
GetKomi( SGF_record_t *kifu, char *sgf_text, int cursor )
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


////////////////////
//  対局名の抽出  //
////////////////////
static int
GetPlayerName( SGF_record_t *kifu, char *sgf_text, int cursor, int color )
{
  int tmp_cursor = 0;
  char size[10];
  memset(size, 0, sizeof(char)*10);

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

///////////////////////////////
//  無視する情報を飛ばす処理  //
///////////////////////////////
static int
SkipData( SGF_record_t *kifu, char *sgf_text, int cursor )
{
  int tmp_cursor = 3;
  
  while ((cursor + tmp_cursor < 100000) &&
	 (sgf_text[cursor + tmp_cursor] != ']')) {
    tmp_cursor++;
  }
  
  return cursor + tmp_cursor;
}


////////////////////////////
//  SGFの座標を数値に変換  //
////////////////////////////
static int
ParsePosition( char c )
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
