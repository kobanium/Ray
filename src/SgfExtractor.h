#ifndef _SGF_EXTRACTOR_H_
#define _SGF_EXTRACTOR_H_

struct SGF_record_t {
  int start_color;      // 最初に着手した色
  int moves;            // 着手数
  int move_x[800];      // 着手(x座標)
  int move_y[800];      // 着手(y座標)
  int result;           // 試合結果
  int handicaps;        // 置き石の数
  int handicap_x[20];   // 置き石(x座標)
  int handicap_y[20];   // 置き石(y座標)
  int board_size;       // 盤のサイズ
  char black_name[256]; // 黒番のプレイヤーの名前
  char white_name[256]; // 白番のプレイヤーの名前
  double komi;          // コミ
};


enum KIFU_RESULT {
  R_JIGO,    // 持碁
  R_BLACK,   // 黒勝ち
  R_WHITE,   // 白勝ち
  R_UNKNOWN, // 不明
};


// SGFファイルの読み込み
void ExtractKifu( const char *file_name, SGF_record_t *kifu );

// 着手を抽出
int GetKifuMove( const SGF_record_t *kifu, int n );

#endif
