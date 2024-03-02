/**
 * @file include/board/GoBoard.hpp
 * @author Yuki Kobayashi
 * @~english
 * @brief Operation of game position.
 * @~japanese
 * @brief 対局情報の管理.
 */
#ifndef _GO_BOARD_HPP_
#define _GO_BOARD_HPP_

#include <cstdlib>

#include "board/BoardData.hpp"


/**
 * @def POS(x, y)
 * @~english 
 * @brief Convert x,y-coordinate to internal expression.
 * @~japanese
 * @brief (x, y)から座標を導出
 */
#define POS(x, y) ((x) + (y) * board_size)

/**
 * @def X(pos)
 * @~english
 * @brief Get x-axis coordinate with internal expression.
 * @~japanese
 * @brief posのx座標の取得
 */
#define X(pos)        ((pos) % board_size)

/**
 * @def Y(pos)
 * @~english
 * @brief Get y-axis coordinate with internal expression.
 * @~japanese
 * @brief posのy座標の取得
 */
#define Y(pos)        ((pos) / board_size)

/**
 * @def CORRECT_X(pos)
 * @~english
 * @brief Get x-axis coordinate.
 * @~japanese
 * @brief 実際の盤上のx座標の取得
 */
#define CORRECT_X(pos) ((pos) % board_size - OB_SIZE + 1)

/**
 * @def CORRECT_Y(pos)
 * @~english
 * @brief Get y-axis coordinate.
 * @~japanese
 * @brief 実際の盤上のy座標の取得
 */
#define CORRECT_Y(pos) ((pos) / board_size - OB_SIZE + 1)

/**
 * @def NORTH(pos)
 * @~english
 * @brief Get upper coordinate.
 * @~japanese
 * @brief posの上の座標の取得
 */
#define NORTH(pos) ((pos) - board_size)

/**
 * @def WEST(pos)
 * @~english
 * @brief Get left coordinate.
 * @~japanese
 * @brief posの左の座標の取得
 */
#define  WEST(pos) ((pos) - 1)

/**
 * @def EAST(pos)
 * @~english
 * @brief Get right coordinate.
 * @~japanese
 * @brief posの右の座標の取得
 */
#define  EAST(pos) ((pos) + 1)

/**
 * @def SOUTH(pos)
 * @~english
 * @brief Get bottom coordinate.
 * @~japanese
 * @brief posの下の座標の取得
 */
#define SOUTH(pos) ((pos) + board_size)

/**
 * @def DX(pos1, pos2)
 * @~english
 * @brief X-axis coordinate distance.
 * @~japanese
 * @brief x方向の距離の算出
 */
#define DX(pos1, pos2)  (abs(board_x[(pos1)] - board_x[(pos2)]))


/**
 * @def DY(pos1, pos2)
 * @~english
 * @brief Y-axis coordinate distance.
 * @~japanese
 * @brief y方向の距離の算出
 */
#define DY(pos1, pos2)  (abs(board_y[(pos1)] - board_y[(pos2)]))

/**
 * @def DIS(pos1, pos2)
 * @~english
 * @brief Get move distance.
 * @~japanese
 * @brief 着手距離の取得
 */
#define DIS(pos1, pos2) (move_dis[DX(pos1, pos2)][DY(pos1, pos2)])


/**
 * @enum eye_condition_t
 * @~english
 * @brief Eye shape condition.
 * @var E_NOT_EYE
 * Not eye.
 * @var E_COMPLETE_HALF_EYE
 * Complete half eye.
 * @var E_HALF_3_EYE
 * Half eye which is needed 3 moves to become complete eye.
 * @var E_HALF_2_EYE
 * Half eye which is needed 2 moves to become complete eye.
 * @var E_HALF_1_EYE
 * Half eye which is needed 1 move to become complete eye.
 * @var E_COMPLETE_ONE_EYE
 * Complete eye.
 * @var E_MAX
 * Sentinel.
 * @~japanese
 * @brief 眼の状態
 * @var E_NOT_EYE
 * 眼でない
 * @var E_COMPLETE_HALF_EYE
 * 完全に欠け眼(8近傍に打って1眼にできない)
 * @var E_HALF_3_EYE
 * 欠け眼であるが, 3手で1眼にできる
 * @var E_HALF_2_EYE
 * 欠け眼であるが, 2手で1眼にできる
 * @var E_HALF_1_EYE
 * 欠け眼であるが, 1手で1眼にできる
 * @var E_COMPLETE_ONE_EYE
 * 完全な1眼
 * @var E_MAX
 * 番兵
 */
enum eye_condition_t : unsigned char {
  E_NOT_EYE,
  E_COMPLETE_HALF_EYE,
  E_HALF_3_EYE,
  E_HALF_2_EYE,
  E_HALF_1_EYE,
  E_COMPLETE_ONE_EYE,
  E_MAX,
};


// 碁盤の大きさ
extern int pure_board_size;

// 碁盤の交点の個数
extern int pure_board_max;

// 碁盤の大きさ(盤外込み)
extern int board_size;

// 碁盤の交点の個数(盤外込み)
extern int board_max;

// 碁盤の右端(上端)
extern int board_start;

// 碁盤の左端(下端)
extern int board_end;

// 初手の候補手の個数
extern int first_move_candidates;

// コミ
extern double komi[S_OB];

// Dynamic Komi
extern double dynamic_komi[S_OB];

// 盤上の位置のID
extern int board_pos_id[BOARD_MAX];  

// 盤上のx座標
extern int board_x[BOARD_MAX];  

//  盤上のy座標
extern int board_y[BOARD_MAX];  

// 眼のパターン
extern unsigned char eye[PAT3_MAX];

// 領地のパターン
extern unsigned char territory[PAT3_MAX];

// 上下左右4近傍の空点の数
extern unsigned char nb4_empty[PAT3_MAX];

// 眼の状態
extern eye_condition_t eye_condition[PAT3_MAX];

// x方向の距離
extern int border_dis_x[BOARD_MAX]; 

// y方向の距離
extern int border_dis_y[BOARD_MAX]; 

// 着手距離
extern int move_dis[PURE_BOARD_SIZE][PURE_BOARD_SIZE];

// 盤上の位置からデータ上の位置の対応
extern int onboard_pos[PURE_BOARD_MAX];

// 初手の候補手
extern int first_move_candidate[PURE_BOARD_MAX];


// 超劫の設定
void SetSuperKo( const bool flag );

// 盤の大きさの設定
void SetBoardSize( const int size );

// メモリ領域の確保
game_info_t *AllocateGame( void );

// メモリ領域の解放
void FreeGame( game_info_t *game );

// 盤面情報のコピー
void CopyGame( game_info_t *dst, const game_info_t *src );

// 定数の初期化
void InitializeConst( void );

// 盤面の初期化
void InitializeBoard( game_info_t *game );

// 合法手判定
// 合法手ならばtrueを返す
bool IsLegal( const game_info_t *game, const int pos, const int color );

// 合法手かつ眼でないか判定
// 合法手かつ眼でなければtrueを返す
bool IsLegalNotEye( game_info_t *game, const int pos, const int color );

// 自殺手判定
// 自殺手ならばtrueを返す
bool IsSuicide( const game_info_t *game, const string_t *string, const int color, const int pos );

// 石を置く
void PutStone( game_info_t *game, const int pos, const int color );

// 石を置く(プレイアウト用)
void PoPutStone( game_info_t *game, const int pos, const int color );

// スコアの判定
int CalculateScore( game_info_t *game );

// コミの値の設定
void SetKomi( const double new_komi );

// 上下左右の座標の計算
void GetNeighbor4( int neighbor4[4], const int pos );

// 隅のマガリ四目の確認
void CheckBentFourInTheCorner( game_info_t *game );

#endif
