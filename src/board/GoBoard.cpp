/**
 * @file src/board/GoBoard.cpp
 * @author Yuki Kobayashi
 * @~english
 * @brief Position controller.
 * @~japanese
 * @brief 盤面の処理
 */
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include "board/GoBoard.hpp"
#include "board/String.hpp"
#include "board/ZobristHash.hpp"
#include "feature/Semeai.hpp"
#include "feature/SimulationFeature.hpp"
#include "mcts/UctRating.hpp"


/**
 * @~english
 * @brief The number of intersections.
 * @~japanese
 * @brief 交点の個数
 */
int pure_board_max = PURE_BOARD_MAX;

/**
 * @~english
 * @brief Size of board.
 * @~japanese
 * @brief 盤の辺の大きさ
 */
int pure_board_size = PURE_BOARD_SIZE;

/**
 * @~english
 * @brief The number of intersections including out of a board.
 * @~japanese
 * @brief 盤外を含む盤の大きさ
 */
int board_max = BOARD_MAX;

/**
 * @~english
 * @brief Size of a board including out of a board.
 * @~japanese
 * @brief 盤外を含む盤の辺の大きさ
 */
int board_size = BOARD_SIZE;

/**
 * @~english
 * @brief Coordinate of left edge (Coordinate of upper edge).
 * @~japanese
 * @brief 盤の左(上)端
 */
int board_start = BOARD_START;

/**
 * @~english
 * @brief Coordinate of right edge (Coordinate of bottom edge).
 * @~japanese
 * @brief 盤の右(下)端
 */
int board_end = BOARD_END;

/**
 * @~english
 * @brief The number of first move candidates.
 * @~japanese
 * @brief 初手の候補手の個数
 */
int first_move_candidates;

/**
 * @~english
 * @brief Komi values for each color.
 * @~japanese
 * @brief コミの値
 */
double komi[S_WHITE + 1];

/**
 * @~english
 * @brief Dynamic komi values for each color.
 * @~japanese
 * @brief ダイナミックコミの値
 */
double dynamic_komi[S_WHITE + 1];

/**
 * @~english
 * @brief Default komi value.
 * @~japanese
 * @brief デフォルトのコミの値
 */
static double default_komi = KOMI;

/**
 * @~english
 * @brief Board position ID.
 * @~japanese
 * @brief 盤上の位置の識別番号
 */
int board_pos_id[BOARD_MAX];

/**
 * @~english
 * @brief Coordinates of X-axis.
 * @~japanese
 * @brief Y方向の座標
 */
int board_x[BOARD_MAX];

/**
 * @~english
 * @brief Coordinates of Y-axis.
 * @~japanese
 * @brief X方向の座標
 */
int board_y[BOARD_MAX];

/**
 * @~english
 * @brief Patterns of eye shape.
 * @~japanese
 * @brief 眼のパターン
 */
unsigned char eye[PAT3_MAX];

/**
 * @~english
 * @brief Patterns of half-eye shape.
 * @~japanese
 * @brief 欠け眼のパターン
 */
unsigned char false_eye[PAT3_MAX];

/**
 * @~english
 * @brief Patterns of territory.
 * @~japanese
 * @brief 領地のパターン
 */
unsigned char territory[PAT3_MAX];

/**
 * @~english
 * @brief The number of neighbor empty points.
 * @~japanese
 * @brief 上下左右の空点の数
 */
unsigned char nb4_empty[PAT3_MAX];

/**
 * @~english
 * @brief Eye shape condition.
 * @~japanese
 * @brief 眼の状態
 */
eye_condition_t eye_condition[PAT3_MAX];

/**
 * @~english
 * @brief X-axis distance from board edge.
 * @~japanese
 * @brief X方向の盤端からの距離
 */
int border_dis_x[BOARD_MAX];

/**
 * @~english
 * @brief Y-axis distance from board edge.
 * @~japanese
 * @brief Y方向の盤端からの距離
 */
int border_dis_y[BOARD_MAX];

/**
 * @~english
 * @brief Move distance.
 * @~japanese
 * @brief 着手距離
 */
int move_dis[PURE_BOARD_SIZE][PURE_BOARD_SIZE];

/**
 * @~english
 * @brief Positions for invalid intersections.
 * @~japanese
 * @brief 実際の盤上の位置との対応
 */
int onboard_pos[PURE_BOARD_MAX];

/**
 * @~english
 * @brief Candidates for first move.
 * @~japanese
 * @brief 初手の候補手
 */
int first_move_candidate[PURE_BOARD_MAX];

/**
 * @~english
 * @brief Corner intersection coordinates.
 * @~japanese
 * @brief 角の座標
 */
static int corner[4];

/**
 * @~english
 * @brief Neighbor coordinates of corner intersection.
 * @~japanese
 * @brief 角に隣接する座標
 */
static int corner_neighbor[4][2];

/**
 * @~english
 * @brief Cross intersection's coordinates.
 * @~japanese
 * @brief 斜めの座標
 */
int cross[4];

/**
 * @~english
 * @brief Super ko flag.
 * @~japanese
 * @brief 超劫の有効化フラグ
 */
static bool check_superko = false;


// 4近傍の空点数の初期化
static void InitializeNeighbor( void );

// 眼のパターンの設定
static void InitializeEye( void );

// 地のパターンの設定
static void InitializeTerritory( void );

//  盤端での処理
static bool IsFalseEyeConnection( const game_info_t *game, const int pos, const int color );


/**
 * @~english
 * @brief Set super ko setting.
 * @param[in] flag Super ko activation flag.
 * @~japanese
 * @brief 超劫の設定
 * @param[in] flag 超劫の有効フラグ
 */
void
SetSuperKo( const bool flag )
{
  check_superko = flag;
}


/**
 * @~english
 * @brief Set board size.
 * @param[in] size Board size.
 * @~japanese
 * @brief 盤の大きさの設定
 * @param[in] size 盤の大きさ
 */
void
SetBoardSize( const int size )
{
  int i;

  pure_board_size = size;
  pure_board_max = size * size;
  board_size = size + 2 * OB_SIZE;
  board_max = board_size * board_size;

  board_start = OB_SIZE;
  board_end = (pure_board_size + OB_SIZE - 1);

  i = 0;
  for (int y = board_start; y <= board_end; y++) {
    for (int x = board_start; x <= board_end; x++) {
      onboard_pos[i++] = POS(x, y);
      board_x[POS(x, y)] = x;
      board_y[POS(x, y)] = y;
    }
  }

  for (int y = board_start; y <= board_end; y++) {
    for (int x = board_start; x <= (board_start + pure_board_size / 2); x++) {
      border_dis_x[POS(x, y)] = x - (OB_SIZE - 1);
      border_dis_x[POS(board_end + OB_SIZE - x, y)] = x - (OB_SIZE - 1);
      border_dis_y[POS(y, x)] = x - (OB_SIZE - 1);
      border_dis_y[POS(y, board_end + OB_SIZE - x)] = x - (OB_SIZE - 1);
    }
  }

  for (int y = 0; y < pure_board_size; y++) {
    for (int x = 0; x < pure_board_size; x++) {
      move_dis[x][y] = x + y + ((x > y) ? x : y);
      if (move_dis[x][y] >= MOVE_DISTANCE_MAX) move_dis[x][y] = MOVE_DISTANCE_MAX - 1;
    }
  }

  std::fill_n(board_pos_id, BOARD_MAX, 0);
  i = 1;
  for (int y = board_start; y <= (board_start + pure_board_size / 2); y++) {
    for (int x = board_start; x <= y; x++) {
      board_pos_id[POS(x, y)] = i;
      board_pos_id[POS(board_end + OB_SIZE - x, y)] = i;
      board_pos_id[POS(y, x)] = i;
      board_pos_id[POS(y, board_end + OB_SIZE - x)] = i;
      board_pos_id[POS(x, board_end + OB_SIZE - y)] = i;
      board_pos_id[POS(board_end + OB_SIZE - x, board_end + OB_SIZE - y)] = i;
      board_pos_id[POS(board_end + OB_SIZE - y, x)] = i;
      board_pos_id[POS(board_end + OB_SIZE - y, board_end + OB_SIZE - x)] = i;
      i++;
    }
  }

  first_move_candidates = 0;
  for (int y = board_start; y <= (board_start + board_end) / 2; y++) {
    for (int x = board_end + board_start - y; x <= board_end; x++) {
      first_move_candidate[first_move_candidates++] = POS(x, y);
    }
  }

  corner[0] = POS(board_start, board_start);
  corner[1] = POS(board_start, board_end);
  corner[2] = POS(board_end, board_start);
  corner[3] = POS(board_end, board_end);

  corner_neighbor[0][0] = EAST(POS(board_start, board_start));
  corner_neighbor[0][1] = SOUTH(POS(board_start, board_start));
  corner_neighbor[1][0] = NORTH(POS(board_start, board_end));
  corner_neighbor[1][1] = EAST(POS(board_start, board_end));
  corner_neighbor[2][0] = WEST(POS(board_end, board_start));
  corner_neighbor[2][1] = SOUTH(POS(board_end, board_start));
  corner_neighbor[3][0] = NORTH(POS(board_end, board_end));
  corner_neighbor[3][1] = WEST(POS(board_end, board_end));
}


/**
 * @~english
 * @brief Set new komi value.
 * @param[in] new_komi New komi value.
 * @~japanese
 * @brief コミの設定
 * @param[in] new_komi 設定するコミの値
 */
void
SetKomi( const double new_komi )
{
  default_komi = new_komi;
  komi[0] = dynamic_komi[0] = default_komi;
  komi[S_BLACK] = dynamic_komi[S_BLACK] = default_komi + 1;
  komi[S_WHITE] = dynamic_komi[S_WHITE] = default_komi - 1;
}


/**
 * @~english
 * @brief Get nearest neighbor intersections coordinates.
 * @param[in, out] neighbor4 Nearest neighbor intersections coordinates.
 * @param[in] pos Intersection coordinates.
 * @~japanese
 * @brief 上下左右の座標の取得
 * @param[in, out] neighbor4 上下左右の座標
 * @param[in] pos 原点の座標
 */
void
GetNeighbor4( int neighbor4[4], const int pos )
{
  neighbor4[0] = NORTH(pos);
  neighbor4[1] =  WEST(pos);
  neighbor4[2] =  EAST(pos);
  neighbor4[3] = SOUTH(pos);
}


/**
 * @~english
 * @brief Allocate board data memory.
 * @return Board data memory.
 * @~japanese
 * @brief 局面情報のメモリ領域の確保
 * @return 局面情報のメモリ領域
 */
game_info_t *
AllocateGame( void )
{
  game_info_t *game;
  game = new game_info_t();
  memset(game, 0, sizeof(game_info_t));

  return game;
}


/**
 * @~english
 * @brief Free board position data memory.
 * @param[in, out] game Board position data.
 * @~japanese
 * @brief 局面情報のメモリ領域の解放
 * @param[in, out] game 局面情報
 */
void
FreeGame( game_info_t *game )
{
  if (game) delete game;
}


/**
 * @~english
 * @brief Initialize board position data.
 * @param[in, out] game Board position data.
 * @~japanese
 * @brief 局面情報の初期化
 * @param[in, out] game 局面情報
 */
void
InitializeBoard( game_info_t *game )
{
  memset(game->record, 0, sizeof(record_t) * MAX_RECORDS);
  memset(game->pat,    0, sizeof(pattern_t) * board_max);

  std::fill_n(game->board, board_max, 0);              
  std::fill_n(game->tactical_features, board_max * ALL_MAX, 0);
  std::fill_n(game->update_num,  static_cast<int>(S_OB), 0);
  std::fill_n(game->capture_num, static_cast<int>(S_OB), 0);
  std::fill(game->update_pos[0],  game->update_pos[S_OB], 0);
  std::fill(game->capture_pos[0], game->capture_pos[S_OB], 0);
  
  game->current_hash = 0;
  game->previous1_hash = 0;
  game->previous2_hash = 0;
  game->positional_hash = 0;
  game->move_hash = 0;

  SetKomi(default_komi);

  game->moves = 1;

  game->pass_count = 0;

  std::fill_n(game->candidates, BOARD_MAX, false);

  for (int y = 0; y < board_size; y++){
    for (int x = 0; x < OB_SIZE; x++) {
      game->board[POS(x, y)] = S_OB;
      game->board[POS(y, x)] = S_OB;
      game->board[POS(y, board_size - 1 - x)] = S_OB;
      game->board[POS(board_size - 1 - x, y)] = S_OB;
    }
  }

  for (int y = board_start; y <= board_end; y++) {
    for (int x = board_start; x <= board_end; x++) {
      int pos = POS(x, y);
      game->candidates[pos] = true;
    }
  }

  for (int i = 0; i < MAX_STRING; i++) {
    game->string[i].flag = false;
  }

  ClearPattern(game->pat);

  InitializeNeighbor();
  InitializeEye();
}


/**
 * @~english
 * @brief Copy board position data.
 * @param[in, out] dst Board position data destination.
 * @param[in] src Board position data source.
 * @~japanese
 * @brief 局面情報のコピー
 * @param[in, out] dst コピー先の局面情報ポインタ
 * @param[in] src コピー元の局面情報ポインタ
 */
void
CopyGame( game_info_t *dst, const game_info_t *src )
{
  memcpy(dst->record,             src->record,             sizeof(record_t) * MAX_RECORDS);
  memcpy(dst->prisoner,           src->prisoner,           sizeof(int) * S_MAX);
  memcpy(dst->board,              src->board,              sizeof(char) * board_max);  
  memcpy(dst->pat,                src->pat,                sizeof(pattern_t) * board_max); 
  memcpy(dst->string_id,          src->string_id,          sizeof(int) * STRING_POS_MAX);
  memcpy(dst->string_next,        src->string_next,        sizeof(int) * STRING_POS_MAX);
  memcpy(dst->candidates,         src->candidates,         sizeof(bool) * board_max); 
  memcpy(dst->capture_num,        src->capture_num,        sizeof(int) * S_OB);
  memcpy(dst->update_num,         src->update_num,         sizeof(int) * S_OB);

  std::fill_n(dst->tactical_features, board_max * ALL_MAX, 0);

  for (int i = 0; i < MAX_STRING; i++) {
    if (src->string[i].flag) {
      memcpy(&dst->string[i], &src->string[i], sizeof(string_t));
    } else {
      dst->string[i].flag = false;
    }
  }

  dst->current_hash = src->current_hash;
  dst->previous1_hash = src->previous1_hash;
  dst->previous2_hash = src->previous2_hash;
  dst->positional_hash = src->positional_hash;
  dst->move_hash = src->move_hash;

  dst->pass_count = src->pass_count;

  dst->moves = src->moves;
  dst->ko_move = src->ko_move;
  dst->ko_pos = src->ko_pos;
}


/**
 * @~english
 * @brief Initialize constant values.
 * @~japanese
 * @brief 定数の初期化
 */
void
InitializeConst( void )
{
  int i;

  komi[0] = default_komi;
  komi[S_BLACK] = default_komi + 1.0;
  komi[S_WHITE] = default_komi - 1.0;

  i = 0;
  for (int y = board_start; y <= board_end; y++) {
    for (int x = board_start; x <= board_end; x++) {
      onboard_pos[i++] = POS(x, y);
      board_x[POS(x, y)] = x;
      board_y[POS(x, y)] = y;
    }
  }

  for (int y = board_start; y <= board_end; y++) {
    for (int x = board_start; x <= (board_start + pure_board_size / 2); x++) {
      border_dis_x[POS(x, y)] = x - (OB_SIZE - 1);
      border_dis_x[POS(board_end + OB_SIZE - x, y)] = x - (OB_SIZE - 1);
      border_dis_y[POS(y, x)] = x - (OB_SIZE - 1);
      border_dis_y[POS(y, board_end + OB_SIZE - x)] = x - (OB_SIZE - 1);
    }
  }

  for (int y = 0; y < pure_board_size; y++) {
    for (int x = 0; x < pure_board_size; x++) {
      move_dis[x][y] = x + y + ((x > y) ? x : y);
      if (move_dis[x][y] >= MOVE_DISTANCE_MAX) move_dis[x][y] = MOVE_DISTANCE_MAX - 1;
    }
  }

  std::fill_n(board_pos_id, BOARD_MAX, 0);
  i = 1;
  for (int y = board_start; y <= (board_start + pure_board_size / 2); y++) {
    for (int x = board_start; x <= y; x++) {
      board_pos_id[POS(x, y)] = i;
      board_pos_id[POS(board_end + OB_SIZE - x, y)] = i;
      board_pos_id[POS(y, x)] = i;
      board_pos_id[POS(y, board_end + OB_SIZE - x)] = i;
      board_pos_id[POS(x, board_end + OB_SIZE - y)] = i;
      board_pos_id[POS(board_end + OB_SIZE - x, board_end + OB_SIZE - y)] = i;
      board_pos_id[POS(board_end + OB_SIZE - y, x)] = i;
      board_pos_id[POS(board_end + OB_SIZE - y, board_end + OB_SIZE - x)] = i;
      i++;
    }
  }

  first_move_candidates = 0;
  for (int y = board_start; y <= (board_start + board_end) / 2; y++) {
    for (int x = board_end + board_start - y; x <= board_end; x++) {
      first_move_candidate[first_move_candidates++] = POS(x, y);
    }
  }

  cross[0] = - board_size - 1;
  cross[1] = - board_size + 1;
  cross[2] = board_size - 1;
  cross[3] = board_size + 1;

  corner[0] = POS(board_start, board_start);
  corner[1] = POS(board_start, board_end);
  corner[2] = POS(board_end, board_start);
  corner[3] = POS(board_end, board_end);

  corner_neighbor[0][0] =  EAST(POS(board_start, board_start));
  corner_neighbor[0][1] = SOUTH(POS(board_start, board_start));
  corner_neighbor[1][0] = NORTH(POS(board_start, board_end));
  corner_neighbor[1][1] =  EAST(POS(board_start, board_end));
  corner_neighbor[2][0] =  WEST(POS(board_end, board_start));
  corner_neighbor[2][1] = SOUTH(POS(board_end, board_start));
  corner_neighbor[3][0] = NORTH(POS(board_end, board_end));
  corner_neighbor[3][1] =  WEST(POS(board_end, board_end));

  InitializeNeighbor();
  InitializeEye();
  InitializeTerritory();
}


/**
 * @~english
 * @brief Set the number of neighbor empty points.
 * @~japanese
 * @brief 隣接する空点数の設定
 */
static void
InitializeNeighbor( void )
{
  for (int i = 0; i < PAT3_MAX; i++) {
    char empty = 0;

    if (((i >>  2) & 0x3) == S_EMPTY) empty++;
    if (((i >>  6) & 0x3) == S_EMPTY) empty++;
    if (((i >>  8) & 0x3) == S_EMPTY) empty++;
    if (((i >> 12) & 0x3) == S_EMPTY) empty++;

    nb4_empty[i] = empty;
  }
}


/**
 * @~english
 * @brief Set eye patterns.
 * @~japanese
 * @brief 眼のパターンの設定
 */
static void
InitializeEye( void )
{
  unsigned int transp[8], pat3_transp16[16];
  //  眼のパターンはそれぞれ1か所あたり2ビットで表現
  //        123
  //        4*5
  //        678
  //  それぞれの番号×2ビットだけシフトさせる
  //        O:自分の石
  //        X:相手の石
  //        +:空点
  //        #:盤外
  const int eye_pat3[] = {
    // +OO     XOO     +O+     XO+
    // O*O     O*O     O*O     O*O
    // OOO     OOO     OOO     OOO
    0x5554, 0x5556, 0x5544, 0x5546,

    // +OO     XOO     +O+     XO+
    // O*O     O*O     O*O     O*O
    // OO+     OO+     OO+     OO+
    0x1554, 0x1556, 0x1544, 0x1546,

    // +OX     XO+     +OO     OOO
    // O*O     O*O     O*O     O*O
    // OO+     +O+     ###     ###
    0x1564, 0x1146, 0xFD54, 0xFD55,

    // +O#     OO#     XOX     XOX
    // O*#     O*#     O+O     O+O
    // ###     ###     OOO     ###
    0xFF74, 0xFF75, 0x5566, 0xFD66,
  };
  const unsigned int false_eye_pat3[4] = {
    // OOX     OOO     XOO     XO# 
    // O*O     O*O     O*O     O*# 
    // XOO     XOX     ###     ### 
    0x5965, 0x9955, 0xFD56, 0xFF76,
  };

  const unsigned int complete_half_eye[12] = {
    // XOX     OOX     XOX     XOX     XOX
    // O*O     O*O     O*O     O*O     O*O
    // OOO     XOO     +OO     XOO     +O+
    0x5566, 0x5965, 0x5166, 0x5966, 0x1166,
    // +OX     XOX     XOX     XOO     XO+
    // O*O     O*O     O*O     O*O     O*O
    // XO+     XO+     XOX     ###     ###
    0x1964, 0x1966, 0x9966, 0xFD56, 0xFD46,
    // XOX     XO#
    // O*O     O*#
    // ###     ###
    0xFD66, 0xFF76
  };
  const unsigned int half_3_eye[2] = {
    // +O+     XO+
    // O*O     O*O
    // +O+     +O+
    0x1144, 0x1146
  };
  const unsigned int half_2_eye[4] = {
    // +O+     XO+     +OX     +O+
    // O*O     O*O     O*O     O*O
    // +OO     +OO     +OO     ###
    0x5144, 0x5146, 0x5164, 0xFD44,
  };
  const unsigned int half_1_eye[6] = {
    // +O+     XO+     OOX     OOX     +OO
    // O*O     O*O     O*O     O*O     O*O
    // OOO     OOO     +OO     +OO     ###
    0x5544, 0x5564, 0x5145, 0x5165, 0xFD54,
    // +O#
    // O*#
    // ###
    0xFF74,
  };
  const unsigned int complete_one_eye[5] = {
    // OOO     +OO     XOO     OOO     OO#
    // O*O     O*O     O*O     O*O     O*#
    // OOO     OOO     OOO     ###     ###
    0x5555, 0x5554, 0x5556, 0xFD55, 0xFF75,
  };

  std::fill_n(eye_condition, PAT3_MAX, E_NOT_EYE);
  
  for (int i = 0; i < 12; i++) {
    Pat3Transpose16(complete_half_eye[i], pat3_transp16);
    for (int j = 0; j < 16; j++) {
      eye_condition[pat3_transp16[j]] = E_COMPLETE_HALF_EYE;
    }
  }

  for (int i = 0; i < 2; i++) {
    Pat3Transpose16(half_3_eye[i], pat3_transp16);
    for (int j = 0; j < 16; j++) {
      eye_condition[pat3_transp16[j]] = E_HALF_3_EYE;
    }
  }

  for (int i = 0; i < 4; i++) {
    Pat3Transpose16(half_2_eye[i], pat3_transp16);
    for (int j = 0; j < 16; j++) {
      eye_condition[pat3_transp16[j]] = E_HALF_2_EYE;
    }
  }

  for (int i = 0; i < 6; i++) {
    Pat3Transpose16(half_1_eye[i], pat3_transp16);
    for (int j = 0; j < 16; j++) {
      eye_condition[pat3_transp16[j]] = E_HALF_1_EYE;
    }
  }

  for (int i = 0; i < 5; i++) {
    Pat3Transpose16(complete_one_eye[i], pat3_transp16);
    for (int j = 0; j < 16; j++) {
      eye_condition[pat3_transp16[j]] = E_COMPLETE_ONE_EYE;
    }
  }

  // BBB
  // B*B
  // BBB
  eye[0x5555] = S_BLACK;

  // WWW
  // W*W
  // WWW
  eye[Pat3Reverse(0x5555)] = S_WHITE;

  // +B+
  // B*B
  // +B+
  eye[0x1144] = S_BLACK;

  // +W+
  // W*W
  // +W+
  eye[Pat3Reverse(0x1144)] = S_WHITE;

  for (int i = 0; i < 14; i++) {
    Pat3Transpose8(eye_pat3[i], transp);
    for (int j = 0; j < 8; j++) {
      eye[transp[j]] = S_BLACK;
      eye[Pat3Reverse(transp[j])] = S_WHITE;
    }
  }

  for (int i = 0; i < 4; i++) {
    Pat3Transpose8(false_eye_pat3[i], transp);
    for (int j = 0; j < 8; j++) {
      false_eye[transp[j]] = S_BLACK;
      false_eye[Pat3Reverse(transp[j])] = S_WHITE;
    }
  }

}


/**
 * @~english
 * @brief Set territory patterns.
 * @~japanese
 * @brief 地のパターン（4近傍が同色）を設定
 */
static void
InitializeTerritory( void )
{
  for (int i = 0; i < PAT3_MAX; i++) {
    if ((i & 0x1144) == 0x1144) {
      territory[i] = S_BLACK;
    } else if ((i & 0x2288) == 0x2288) {
      territory[i] = S_WHITE;
    }
  }
}


/**
 * @~english
 * @brief Check legal move.
 * @param[in] game Board position data.
 * @param[in] pos Move coordinate.
 * @param[in] color Player's color.
 * @return Move is legal or not.
 * @~japanese
 * @brief 合法手判定
 * @param[in] game 局面情報
 * @param[in] pos 着手する座標
 * @param[in] color 手番の色
 * @return 合法手か否かの判定
 */
bool
IsLegal( const game_info_t *game, const int pos, const int color )
{
  // 既に石がある
  if (game->board[pos] != S_EMPTY) {
    return false;
  }

  // 自殺手である
  if (nb4_empty[Pat3(game->pat, pos)] == 0 &&
      IsSuicide(game, game->string, color, pos)) {
    return false;
  }

  // 劫である
  if (game->ko_pos == pos &&
      game->ko_move == (game->moves - 1)) {
    return false;
  }

  // 超劫である
  if (check_superko &&
      pos != PASS) {
    const int other = GetOppositeColor(color);
    const string_t *string = game->string;
    const int *string_id = game->string_id;
    const int *string_next = game->string_next;
    unsigned long long hash = game->positional_hash;
    int neighbor4[4], check[4], checked = 0, id, str_pos;
    bool flag;

    GetNeighbor4(neighbor4, pos);

    // この1手で石を取れる時の処理
    for (int i = 0; i < 4; i++) {
      if (game->board[neighbor4[i]] == other) {
        id = string_id[neighbor4[i]];
        if (string[id].libs == 1) {
          flag = false;
          for (int j = 0; j < checked; j++) {
            if (check[j] == id) {
              flag = true;
            }
          }
          if (flag) {
            continue;
          }
          str_pos = string[id].origin;
          do {
            hash ^= hash_bit[str_pos][other];
            str_pos = string_next[str_pos];
          } while (str_pos != STRING_END);
        }
        check[checked++] = id;
      }
    }

    // posにcolorを置いたと仮定
    hash ^= hash_bit[pos][color];
    
    for (int i = 0; i < game->moves; i++) {
      if (game->record[i].hash == hash) {
        return false;
      }
    }
  }
  
  return true;
}


/**
 * @~english
 * @brief Check edge connection move.
 * @param[in] game Board position data.
 * @param[in] pos Move coordinate.
 * @param[in] color Player's color.
 * @return Edge connection is valid or not.
 * @~japanese
 * @brief 盤端での接続可否判定
 * @param[in] game 局面情報
 * @param[in] pos 判定する座標
 * @param[in] color 手番の色
 * @return 盤端での接続判定
 */
static bool
IsFalseEyeConnection( const game_info_t *game, const int pos, const int color )
{
  // +++++XOO#
  // +++++XO+#
  // +++XXXOO#
  // ++XOOXXO#
  // +++O*OO*#
  // #########
  // シミュレーション中に上の局面の*を眼と認識せずに打つように,
  // ++++XXXX#
  // +++XXOOO#
  // +++XO+XO#
  // +++XOOO*#
  // #########
  // シミュレーション中に上の局面の*を眼と認識して打たないようにする.
  //
  // 隣接する2つの自分の連の呼吸点に共通するものがなければ打ち
  // 共通するものがあれば打たないようにしている.
  // 以下の局面は誤認識して打ってしまうので要対応.
  // ++++XXXX#
  // +++XXOOO#
  // +++XOX+O#
  // +++XO+XO#
  // +++XOOO*#
  // #########
  const string_t *string = game->string;
  const int *string_id = game->string_id;
  const char *board = game->board;
  const int other = GetOppositeColor(color);
  int id, neighbor, count, lib, libs = 0, lib_sum = 0, player_ids = 0, strings = 0;
  int neighbor4[4], player_id[4] = {0}, liberty[STRING_LIB_MAX], checked_string[4] = {0}, string_liberties[4] = {0};
  bool checked, already_checked;

  // 欠け眼を構成する連のIDを取り出す
  GetNeighbor4(neighbor4, pos);
  for (int i = 0; i < 4; i++) {
    checked = false;
    for (int j = 0; j < player_ids; j++) {
      if (player_id[j] == string_id[neighbor4[i]]) {
        checked = true;
      }
    }
    if (!checked) {
      player_id[player_ids++] = string_id[neighbor4[i]];
    }
  }

  // 斜め方向に取れる, または取れそうな石があったらfalseを返す
  for (int i = 0; i < 4; i++) {
    if (board[pos + cross[i]] == other) {
      id = string_id[pos + cross[i]];
      if (IsAlreadyCaptured(game, id, player_id, player_ids)) {
        return false;
      }
    }
  }

  // 隣接する座標が自分の連なら
  // その連の呼吸点を取り出す
  for (int i = 0; i < 4; i++) {
    if (board[neighbor4[i]] == color) {
      id = string_id[neighbor4[i]];
      if (string[id].libs == 2) {
        lib = string[id].lib[0];
        if (lib == pos) lib = string[id].lib[lib];
        if (IsSelfAtari(game, color, lib)) return true;
      }
      already_checked = false;
      for (int j = 0; j < strings; j++) {
        if (checked_string[j] == id) {
          already_checked = true;
          break;
        }
      }
      if (already_checked) continue;
      lib = string[id].lib[0];
      count = 0;
      while (lib != LIBERTY_END) {
        if (lib != pos) {
          checked = false;
          for (i = 0; i < libs; i++) {
            if (liberty[i] == lib) {
              checked = true;
              break;
            }
          }
          if (!checked) {
            liberty[libs + count] = lib;
            count++;
          }
        }
        lib = string[id].lib[lib];
      }
      libs += count;
      string_liberties[strings] = string[id].libs;
      checked_string[strings++] = id;
    }
  }

  // その連が持っている呼吸点を求める
  for (int i = 0; i < strings; i++) {
    lib_sum += string_liberties[i] - 1;
  }

  neighbor = string[checked_string[0]].neighbor[0];
  while (neighbor != NEIGHBOR_END) {
    if (string[neighbor].libs == 1 &&
        string[checked_string[1]].neighbor[neighbor] != 0) {
      return false;
    }
    neighbor = string[checked_string[0]].neighbor[neighbor];
  }

  // 隣接する連が一続きなら眼なのでfalseを返す
  if (strings == 1) {
    return false;
  }

  // 2つの連が呼吸点を共有していなければtrue
  // そうでなければfalseを返す
  if (libs == lib_sum) {
    return true;
  } else {
    return false;
  }
}


/**
 * @~english
 * @brief Check legal move and not eye pattern.
 * @param[in] game Board position data.
 * @param[in] pos Move coordinate.
 * @param[in] color Player's color.
 * @return Move is legal and not eye pattern or not.
 * @~japanese
 * @brief 合法手かつ眼でないかを判定
 * @param[in] game 局面情報
 * @param[in] pos 着手する座標
 * @param[in] color 手番の色
 * @return 合法手かつ眼でないか否かの判定
 */
bool
IsLegalNotEye( game_info_t *game, const int pos, const int color )
{
  const int *string_id = game->string_id;
  const string_t *string = game->string;

  // 既に石がある
  if (game->board[pos] != S_EMPTY) {
    // 候補手から除外
    game->candidates[pos] = false;

    return false;
  }

  if (game->seki[pos]) {
    return false;
  }

  // 眼
  if (eye[Pat3(game->pat, pos)] != color ||
      string[string_id[NORTH(pos)]].libs == 1 ||
      string[string_id[ EAST(pos)]].libs == 1 ||
      string[string_id[SOUTH(pos)]].libs == 1 ||
      string[string_id[ WEST(pos)]].libs == 1){

    // 自殺手かどうか
    if (nb4_empty[Pat3(game->pat, pos)] == 0 &&
        IsSuicide(game, string, color, pos)) {
      return false;
    }

    // 劫
    if (game->ko_pos == pos &&
        game->ko_move == (game->moves - 1)) {
      return false;
    }

    // 盤端の特殊処理
    if (false_eye[Pat3(game->pat, pos)] == color) {
      if (IsFalseEyeConnection(game, pos, color)) {
        return true;
      } else {
        game->candidates[pos] = false;
        return false;
      }
    }

    return true;
  }

  // 候補手から除外
  game->candidates[pos] = false;

  return false;
}


/**
 * @~english
 * @brief Check suicide move.
 * @param[in] game Board position data.
 * @param[in] string Stones string data.
 * @param[in] color Player's color.
 * @param[in] pos Intersection coordinate.
 * @return Move is suicide move or not.
 * @~japanese
 * @brief 自殺手の判定
 * @param[in] game 局面情報
 * @param[in] string 連の情報
 * @param[in] color 手番の色
 * @param[in] pos 確認する交点の座標
 * @return 自殺手か否かの判定
 */
bool
IsSuicide( const game_info_t *game, const string_t *string, const int color, const int pos )
{
  const char *board = game->board;
  const int *string_id = game->string_id;
  const int other = GetOppositeColor(color);
  int neighbor4[4];

  GetNeighbor4(neighbor4, pos);

  // 隣接するの石についての判定
  // 隣接する石が相手でも、その石を含む連の呼吸点が1の時は合法手
  // 隣接する石が自分で、その石を含む連の呼吸点が2以上の時は合法手
  for (int i = 0; i < 4; i++) {
    if (board[neighbor4[i]] == other &&
        string[string_id[neighbor4[i]]].libs == 1) {
      return false;
    } else if (board[neighbor4[i]] == color &&
               string[string_id[neighbor4[i]]].libs > 1) {
      return false;
    }
  }

  return true;
}


/**
 * @~english
 * @brief Set a stone to board.
 * @param[in, out] game Board position data.
 * @param[in] pos Stone's coordinate.
 * @param[in] color Stone's color.
 * @~japanese
 * @brief 石を置く処理
 * @param[in, out] game 局面情報
 * @param[in] pos 石を置く座標
 * @param[in] color 置く石の色
 */
void
PutStone( game_info_t *game, const int pos, const int color )
{
  const int *string_id = game->string_id;
  const int other = GetOppositeColor(color);
  char *board = game->board;
  string_t *string = game->string;
  int connection = 0, prisoner = 0;
  int neighbor[4], connect[4] = { 0 };

  // この手番の着手で打ち上げた石の数を0にする
  game->capture_num[color] = 0;

  // 着手箇所の戦術的特徴を全て消す
  ClearTacticalFeatures(&game->tactical_features[pos * ALL_MAX]);

  game->previous2_hash = game->previous1_hash;
  game->previous1_hash = game->current_hash;

  if (game->ko_move != 0 && game->ko_move == game->moves - 1) {
    game->current_hash ^= hash_bit[game->ko_pos][HASH_KO];
  }

  // 着手箇所と色を記録
  if (game->moves < MAX_RECORDS) {
    game->record[game->moves].color = color;
    game->record[game->moves].pos = pos;
    game->move_hash ^= move_bit[game->moves][pos][color];
  }

  // 着手がパスなら手数を進めて終了
  if (pos == PASS) {
    if (game->moves < MAX_RECORDS) {
      game->record[game->moves].hash = game->positional_hash;
    }
    game->current_hash ^= hash_bit[game->pass_count++][HASH_PASS];
    if (game->pass_count >= BOARD_MAX) { 
      game->pass_count = 0;
    }
    game->moves++;
    return;
  }

  // 石を置く
  board[pos] = (char)color;

  // 候補手から除外
  game->candidates[pos] = false;

  game->current_hash ^= hash_bit[pos][color];
  game->positional_hash ^= hash_bit[pos][color];

  // パターンの更新(MD5)
  UpdatePatternStone(game->pat, color, pos);

  // 着手点の上下左右の座標を導出
  GetNeighbor4(neighbor, pos);

  // 着手箇所の上下左右の確認
  // 自分の連があれば, その連の呼吸点を1つ減らし, 接続候補に入れる
  // 敵の連であれば, その連の呼吸点を1つ減らし, 呼吸点が0になったら取り除く
  for (int i = 0; i < 4; i++) {
    if (board[neighbor[i]] == color) {
      RemoveLiberty(game, &string[string_id[neighbor[i]]], pos);
      connect[connection++] = string_id[neighbor[i]];
    } else if (board[neighbor[i]] == other) {
      RemoveLiberty(game, &string[string_id[neighbor[i]]], pos);
      if (string[string_id[neighbor[i]]].libs == 0) {
        prisoner += RemoveString(game, &string[string_id[neighbor[i]]], color);
      }
    }
  }

  // 打ち上げた石をアゲハマに追加
  game->prisoner[color] += prisoner;

  // 接続候補がなければ, 新しい連を作成して, 劫かどうかの確認をする
  // 接続候補が1つならば, その連に石を追加する
  // 接続候補が2つ以上ならば, 連同士を繋ぎ合わせて, 石を追加する
  if (connection == 0) {
    MakeString(game, pos, color);
    if (prisoner == 1 &&
        string[string_id[pos]].libs == 1) {
      game->ko_move = game->moves;
      game->ko_pos = string[string_id[pos]].lib[0];
      game->current_hash ^= hash_bit[game->ko_pos][HASH_KO];
    }
  } else if (connection == 1) {
    AddStone(game, pos, color, connect[0]);
  } else {
    ConnectString(game, pos, color, connection, connect);
  }

  // ハッシュ値の記録
  if (game->moves < MAX_RECORDS) {
    game->record[game->moves].hash = game->positional_hash;
  }
  
  // 手数を1つだけ進める
  game->moves++;
}


/**
 * @~english
 * @brief Set a stone to board for Monte-Carlo simulation.
 * @param[in, out] game Board position data.
 * @param[in] pos Stone's coordinate.
 * @param[in] color Stone's color.
 * @~japanese
 * @brief モンテカルロ・シミュレーション用の石を置く処理
 * @param[in, out] game 局面情報
 * @param[in] pos 石を置く座標
 * @param[in] color 置く石の色
 */
void
PoPutStone( game_info_t *game, const int pos, const int color )
{
  const int *string_id = game->string_id;
  const int other = GetOppositeColor(color);
  char *board = game->board;
  string_t *string = game->string;
  int connection = 0, prisoner = 0;
  int neighbor[4], connect[4] = { 0 };

  // この手番で取った石の個数を0に
  game->capture_num[color] = 0;

  // 着手制限の限界を超えていなければ記録
  if (game->moves < MAX_RECORDS) {
    game->record[game->moves].color = color;
    game->record[game->moves].pos = pos;
  }

  // 着手がパスなら手数を進めて終了
  if (pos == PASS) {
    game->moves++;
    return;
  }

  // 碁盤に石を置く
  board[pos] = (char)color;

  // 候補酒から除外
  game->candidates[pos] = false;

  // 着手箇所の戦術的特徴を全て消す
  ClearTacticalFeatures(&game->tactical_features[pos * ALL_MAX]);

  // 着手箇所のレートを0に戻す
  game->sum_rate[0] -= game->rate[0][pos];
  game->sum_rate_row[0][board_y[pos]] -= game->rate[0][pos];
  game->rate[0][pos] = 0;
  game->sum_rate[1] -= game->rate[1][pos];
  game->sum_rate_row[1][board_y[pos]] -= game->rate[1][pos];
  game->rate[1][pos] = 0;

  // パターンの更新(MD2)  
  UpdateMD2Stone(game->pat, color, pos);

  // 着手箇所の上下左右の座標の導出
  GetNeighbor4(neighbor, pos);

  // 着手箇所の上下左右の確認
  // 自分の連があれば, その連の呼吸点を1つ減らし, 接続候補に入れる
  // 敵の連であれば, その連の呼吸点を1つ減らし, 呼吸点が0になったら取り除く  
  for (int i = 0; i < 4; i++) {
    if (board[neighbor[i]] == color) {
      PoRemoveLiberty(game, &string[string_id[neighbor[i]]], pos, color);
      connect[connection++] = string_id[neighbor[i]];
    } else if (board[neighbor[i]] == other) {
      PoRemoveLiberty(game, &string[string_id[neighbor[i]]], pos, color);
      if (string[string_id[neighbor[i]]].libs == 0) {
        prisoner += PoRemoveString(game, &string[string_id[neighbor[i]]], color);
      }
    }
  }

  // 打ち上げた石をアゲハマに追加
  game->prisoner[color] += prisoner;

  // 接続候補がなければ, 新しい連を作成して, 劫かどうかの確認をする
  // 接続候補が1つならば, その連に石を追加する
  // 接続候補が2つ以上ならば, 連同士を繋ぎ合わせて, 石を追加する  
  if (connection == 0) {
    MakeString(game, pos, color);
    if (prisoner == 1 &&
        string[string_id[pos]].libs == 1) {
      game->ko_move = game->moves;
      game->ko_pos = string[string_id[pos]].lib[0];
    }
  } else if (connection == 1) {
    AddStone(game, pos, color, connect[0]);
  } else {
    ConnectString(game, pos, color, connection, connect);
  }

  // 手数を進める
  game->moves++;
}


/**
 * @~english
 * @brief Check bent for in the corner.
 * @param[in] game Board position data.
 * @~japanese
 * @brief 隅のマガリ四目の確認
 * @param[in] game 局面情報
 */
void
CheckBentFourInTheCorner( game_info_t *game )
{
  const string_t *string = game->string;
  const int *string_id = game->string_id;
  const int *string_next = game->string_next;
  char *board = game->board;
  int pos, id, neighbor, color, lib1, lib2, neighbor_lib1, neighbor_lib2;

  // 四隅について隅のマガリ四目が存在するか確認し
  // 存在すれば地を訂正する
  for (int i = 0; i < 4; i++) {
    id = string_id[corner[i]];
    if (string[id].size == 3 &&
        string[id].libs == 2 &&
        string[id].neighbors == 1) {
      color = string[id].color;
      lib1 = string[id].lib[0];
      lib2 = string[id].lib[lib1];
      if ((board[corner_neighbor[i][0]] == S_EMPTY ||
          board[corner_neighbor[i][0]] == color) &&
          (board[corner_neighbor[i][1]] == S_EMPTY ||
          board[corner_neighbor[i][1]] == color)) {
        neighbor = string[id].neighbor[0];
        if (string[neighbor].libs == 2 &&
            string[neighbor].size > 6) {
          // 呼吸点を共有しているかの確認
          neighbor_lib1 = string[neighbor].lib[0];
          neighbor_lib2 = string[neighbor].lib[neighbor_lib1];
          if ((neighbor_lib1 == lib1 && neighbor_lib2 == lib2) ||
              (neighbor_lib1 == lib2 && neighbor_lib2 == lib1)) {
            pos = string[neighbor].origin;
            while (pos != STRING_END) {
              board[pos] = static_cast<char>(color);
              pos = string_next[pos];
            }
            pos = string[neighbor].lib[0];
            board[pos] = static_cast<char>(color);
            pos = string[neighbor].lib[pos];
            board[pos] = static_cast<char>(color);
          }
        }
      }
    }
  }
}


/**
 * @~english
 * @brief Calculate score with Tromp-Taylor rules.
 * @param[in] game Board position data.
 * @return Score without komi adjustment
 * @~japanese
 * @brief 領地の計算
 * @param[in] game 局面情報
 * @return コミを考慮しない領地
 */
int
CalculateScore( game_info_t *game )
{
  const char *board = game->board;
  int color;
  int scores[S_MAX] = { 0 };

  // 地の数え上げ
  for (int i = 0; i < pure_board_max; i++) {
    const int pos = onboard_pos[i];
    color = board[pos];
    if (color == S_EMPTY) color = territory[Pat3(game->pat, pos)];
    scores[color]++;
  }

  //  黒−白を返す(コミなし)
  return(scores[S_BLACK] - scores[S_WHITE]);
}
