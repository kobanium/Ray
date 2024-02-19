/**
 * @file include/sgf/SgfExtractor.hpp
 * @author Yuki Kobayashi
 * @~english
 * @brief Smart Game format file loader.
 * @~japanese
 * @brief Smart Game formatファイルローダ
 */
#ifndef _SGF_EXTRACTOR_HPP_
#define _SGF_EXTRACTOR_HPP_

/**
 * @struct SGF_record_t
 * @~english
 * @brief Data read from Smart Game Format file.
 * @~japanese
 * @brief SGFファイルから読み込んだデータ
 */
struct SGF_record_t {
  /**
   * @~english
   * @brief First move color.
   * @~japanese
   * @brief 最初に着手した手番の色
   */
  int start_color;

  /**
   * @~english
   * @brief The number of moves.
   * @~japanese
   * @brief 着手数
   */
  int moves;

  /**
   * @~english
   * @brief X-coordinate of each move.
   * @~japanese
   * @brief 着手(x座標)
   */
  int move_x[800];

  /**
   * @~english
   * @brief Y-coordinate of each move.
   * @~japanese
   * @brief 着手(y座標)
   */
  int move_y[800];

  /**
   * @~english
   * @brief Match result.
   * @~japanese
   * @brief 試合結果
   */
  int result;

  /**
   * @~english
   * @brief The number of handicap stones.
   * @~japanese
   * @brief 置き石の数
   */
  int handicaps;

  /**
   * @~english
   * @brief X-coordinate of each handicap stone.
   * @~japanese
   * @brief 置き石(x座標)
   */
  int handicap_x[800];

  /**
   * @~english
   * @brief Y-coordinate of each handicap stone.
   * @~japanese
   * @brief 置き石(y座標)
   */
  int handicap_y[800];

  /**
   * @~english
   * @brief Color of each handicap stone.
   * @~japanese
   * @brief 置き石の色
   */
  int handicap_color[800];

  /**
   * @~english
   * @brief The number of handicap stones.
   * @~japanese
   * @brief 置き石の個数(内部処理用)
   */
  int handicap_stones;

  /**
   * @~english
   * @brief Size of board.
   * @~japanese
   * @brief 盤のサイズ
   */
  int board_size;

  /**
   * @~english
   * @brief Black player's name.
   * @~japanese
   * @brief 黒番のプレイヤの名前
   */
  char black_name[256];

  /**
   * @~english
   * @brief White player's name.
   * @~japanese
   * @brief 白番のプレイヤの名前
   */
  char white_name[256];

  /**
   * @~english
   * @brief Komi
   * @~japanese
   * @brief コミ
   */
  double komi;
};


/**
 * @enum KIFU_RESULT
 * @~english
 * @brief Result of the match.
 * @var R_JIGO
 * Draw.
 * @var R_BLACK
 * Black won.
 * @var R_WHITE
 * White won.
 * @var R_UNKNOWN
 * Unknown result.
 * @~japanese
 * @brief 対局の結果
 * @var R_JIGO
 * 持碁
 * @var R_BLACK
 * 黒番の勝ち
 * @var R_WHITE
 * 白番の勝ち
 * @var R_UNKNOWN
 * 結果不明
 */
enum KIFU_RESULT {
  R_JIGO,
  R_BLACK,
  R_WHITE,
  R_UNKNOWN,
};


// SGFファイルの読み込み
int ExtractKifu( const char *file_name, SGF_record_t *kifu );

// 着手を抽出
int GetKifuMove( const SGF_record_t *kifu, const int n );

// 置き石を抽出
int GetHandicapStone( const SGF_record_t *kifu, const int n );

#endif
