#ifndef _CONSTANT_HPP_
#define _CONSTANT_HPP_

// 盤の大きさ
constexpr int PURE_BOARD_SIZE = 19;

// 盤外の幅
constexpr int OB_SIZE = 5;

// 盤外を含めた盤の幅
constexpr int BOARD_SIZE = PURE_BOARD_SIZE + 2 * OB_SIZE;

// 盤の大きさ
constexpr int PURE_BOARD_MAX = PURE_BOARD_SIZE * PURE_BOARD_SIZE;

// 盤外を含めた盤の大きさ
constexpr int BOARD_MAX = BOARD_SIZE * BOARD_SIZE;

// 連の最大数
constexpr int MAX_STRING = PURE_BOARD_MAX * 4 / 5;

// 隣接する敵の連の最大数
constexpr int MAX_NEIGHBOR = MAX_STRING;

// 盤の始点
constexpr int BOARD_START = OB_SIZE;

// 盤の終点
constexpr int BOARD_END = PURE_BOARD_SIZE + OB_SIZE - 1;

// 1つのレンガ持つ呼吸点の最大数
constexpr int STRING_LIB_MAX = BOARD_SIZE * (PURE_BOARD_SIZE + OB_SIZE);

// 連が持ちうる座標の最大数
constexpr int STRING_POS_MAX = BOARD_SIZE * (PURE_BOARD_SIZE + OB_SIZE);

// 
constexpr int CAPTURE_MAX = BOARD_SIZE * (PURE_BOARD_SIZE + OB_SIZE);

// 
constexpr int CAPTURE_END = CAPTURE_MAX - 1;

// 連の終端を表す値
constexpr int STRING_END = STRING_POS_MAX - 1;

// 隣接する敵の連の終端を表す値
constexpr int NEIGHBOR_END = MAX_NEIGHBOR - 1;

// 呼吸点の終端を表す値
constexpr int LIBERTY_END = STRING_LIB_MAX - 1;

// 記録する着手の最大数
constexpr int MAX_RECORDS = PURE_BOARD_MAX * 3;

// 着手数の最大値
constexpr int MAX_MOVES = MAX_RECORDS - 1;

// パス
constexpr int PASS = 0;

// 投了
constexpr int RESIGN = -1;

#endif
